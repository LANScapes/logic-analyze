/* Optional firmware manifest preflight. GPL-3.0-or-later, as libsigrok4DSL. */
#include "libsigrok-internal.h"
#include "log.h"
#include <string.h>

/* Configured before ds_lib_init, immutable while library threads run. */
static gboolean manifest_enabled;
static GHashTable *resources;
static char *resource_dir;

struct resource_buffer {
    unsigned char *data;
    gsize size;
};

static void free_resource(gpointer value)
{
    struct resource_buffer *buffer = value;
    g_free(buffer->data);
    g_free(buffer);
}

SR_PRIV gboolean ds_resource_manifest_enabled(void)
{
    return manifest_enabled;
}

/* Borrowed storage: never reopened, copied or rehashed by a loader. */
SR_PRIV int ds_resource_buffer(const char *filename, gsize max_size,
        const unsigned char **data, gsize *size)
{
    struct resource_buffer *buffer;
    gsize prefix;

    *data = NULL;
    *size = 0;
    if (!filename || !resources || !resource_dir || strcmp(resource_dir, DS_RES_PATH))
        return SR_ERR;
    prefix = strlen(resource_dir);
    if (strncmp(filename, resource_dir, prefix) || filename[prefix] != '/')
        return SR_ERR;
    buffer = g_hash_table_lookup(resources, filename + prefix + 1);
    if (!buffer || !buffer->size || buffer->size > max_size) {
        sr_err("Unverified or oversized resource: %s", filename);
        return SR_ERR;
    }
    *data = buffer->data;
    *size = buffer->size;
    return SR_OK;
}

#ifndef _WIN32
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>

#define MANIFEST_LIMIT (1024 * 1024)
#define RESOURCE_TOTAL_LIMIT (256 * 1024 * 1024)
/* FX2 RAM addresses are 16 bits; FPGA length commands are 24 bits. */
#define FIRMWARE_LIMIT 0x10000
#define FPGA_LIMIT 0xffffff

static int resource_error(GError **error, const char *format, ...)
{
    va_list args;
    char *message;
    va_start(args, format);
    message = g_strdup_vprintf(format, args);
    va_end(args);
    g_set_error_literal(error, G_FILE_ERROR, G_FILE_ERROR_FAILED, message);
    g_free(message);
    return SR_ERR;
}

/* Portable relative names, without aliases, traversal or control characters. */
static gboolean valid_path(const char *path)
{
    const char *component = path;
    if (!*path || strlen(path) > 4095)
        return FALSE;
    for (const char *p = path; ; p++) {
        if (*p == '/' || !*p) {
            gsize len = (gsize)(p - component);
            if (!len || (len == 1 && *component == '.') ||
                    (len == 2 && component[0] == '.' && component[1] == '.'))
                return FALSE;
            if (!*p)
                return TRUE;
            component = p + 1;
        } else if ((unsigned char)*p < 0x20 || *p == 0x7f || *p == '\\') {
            return FALSE;
        }
    }
}

/* Consume the inherited descriptor at its current position; caller owns it.
 * read(), rather than /dev/fd or fdopen(), also works for a pipe. */
static char *read_manifest(int fd, GError **error)
{
    char *text = g_try_malloc(MANIFEST_LIMIT + 1);
    gsize used = 0;
    if (!text) {
        resource_error(error, "Cannot allocate resource manifest");
        return NULL;
    }
    for (;;) {
        ssize_t n = read(fd, text + used, MANIFEST_LIMIT + 1 - used);
        if (n < 0 && errno == EINTR)
            continue;
        if (n < 0 || used + (gsize)n > MANIFEST_LIMIT) {
            resource_error(error, "Cannot read resource manifest (read error or limit exceeded)");
            g_free(text);
            return NULL;
        }
        if (!n)
            break;
        if (memchr(text + used, 0, (gsize)n)) {
            resource_error(error, "NUL byte in resource manifest");
            g_free(text);
            return NULL;
        }
        used += (gsize)n;
    }
    text[used] = 0;
    return text;
}

/* Resolve each component beneath the opened root; no symlinks or pathname
 * stat/reopen pairs. Every resource is opened just once for the full read. */
static int open_resource(int root, const char *path)
{
    char **parts = g_strsplit(path, "/", -1);
    int parent = dup(root), fd = -1;
    for (gsize i = 0; parent >= 0 && parts[i]; i++) {
        int flags = O_RDONLY | O_CLOEXEC | O_NOFOLLOW | O_NONBLOCK;
        if (parts[i + 1])
            flags |= O_DIRECTORY;
        fd = openat(parent, parts[i], flags);
        close(parent);
        parent = fd;
    }
    g_strfreev(parts);
    return parent;
}

static int load_resource(int root, const char *path, const char *digest,
        struct resource_buffer **out, gsize *total, GError **error)
{
    int fd = open_resource(root, path);
    struct stat st;
    struct resource_buffer *buffer = NULL;
    gsize used = 0, limit = g_str_has_suffix(path, ".fw") ? FIRMWARE_LIMIT : FPGA_LIMIT;
    unsigned char extra;
    char *actual = NULL;
    int ret = SR_ERR;

    if (fd < 0) {
        resource_error(error, "Cannot open resource: %s", path);
        return SR_ERR;
    }
    if (fstat(fd, &st) || !S_ISREG(st.st_mode) || st.st_size <= 0 ||
            (uint64_t)st.st_size > limit || (uint64_t)st.st_size > RESOURCE_TOTAL_LIMIT - *total) {
        resource_error(error, "Invalid resource type or size: %s", path);
        goto done;
    }
    buffer = g_try_malloc0(sizeof *buffer);
    if (!buffer || !(buffer->data = g_try_malloc((gsize)st.st_size))) {
        resource_error(error, "Cannot allocate resource: %s", path);
        goto done;
    }
    buffer->size = (gsize)st.st_size;
    while (used < buffer->size) {
        ssize_t n = read(fd, buffer->data + used, buffer->size - used);
        if (n < 0 && errno == EINTR)
            continue;
        if (n <= 0) {
            resource_error(error, "Incomplete resource read: %s", path);
            goto done;
        }
        used += (gsize)n;
    }
    /* A growing file must not be accepted as a verified prefix. */
    ssize_t n;
    do { n = read(fd, &extra, 1); } while (n < 0 && errno == EINTR);
    if (n != 0) {
        resource_error(error, "Resource changed size or read failed: %s", path);
        goto done;
    }
    actual = g_compute_checksum_for_data(G_CHECKSUM_SHA256, buffer->data, buffer->size);
    if (!actual || g_ascii_strcasecmp(actual, digest)) {
        resource_error(error, "Resource SHA-256 mismatch: %s", path);
        goto done;
    }
    *total += buffer->size;
    *out = buffer;
    buffer = NULL;
    ret = SR_OK;
done:
    if (buffer)
        free_resource(buffer);
    g_free(actual);
    if (close(fd) && ret == SR_OK) {
        free_resource(*out);
        *out = NULL;
        ret = resource_error(error, "Cannot close resource: %s", path);
    }
    return ret;
}

/* Require entries for all firmware/bitstream files present before any USB
 * initialization. A later file cannot be loaded unless it is in this cache. */
static int check_inventory(int fd, const char *prefix, GHashTable *table,
        unsigned depth, GError **error)
{
    DIR *dir = fdopendir(fd); /* takes ownership */
    struct dirent *entry;
    int ret = SR_OK;
    if (!dir) {
        close(fd);
        return resource_error(error, "Cannot enumerate resource directory: %s", prefix);
    }
    for (;;) {
        errno = 0;
        entry = readdir(dir);
        if (!entry) {
            if (errno)
                ret = resource_error(error, "Resource directory read failed: %s", prefix);
            break;
        }
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;
        char *path = *prefix ? g_strconcat(prefix, "/", entry->d_name, NULL) : g_strdup(entry->d_name);
        struct stat st;
        if (fstatat(dirfd(dir), entry->d_name, &st, AT_SYMLINK_NOFOLLOW)) {
            ret = resource_error(error, "Cannot inspect resource: %s", path);
        } else if (S_ISDIR(st.st_mode)) {
            int child = openat(dirfd(dir), entry->d_name, O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
            if (child < 0 || depth >= 64) {
                if (child >= 0) close(child);
                ret = resource_error(error, "Cannot enumerate resource subdirectory: %s", path);
            } else {
                ret = check_inventory(child, path, table, depth + 1, error);
            }
        } else if (S_ISLNK(st.st_mode)) {
            ret = resource_error(error, "Symlink in resource directory: %s", path);
        } else if (g_str_has_suffix(path, ".fw") || g_str_has_suffix(path, ".bin")) {
            if (!S_ISREG(st.st_mode) || !g_hash_table_contains(table, path))
                ret = resource_error(error, "Missing manifest entry or invalid resource: %s", path);
        }
        g_free(path);
        if (ret != SR_OK)
            break;
    }
    if (closedir(dir) && ret == SR_OK)
        ret = resource_error(error, "Cannot close resource directory: %s", prefix);
    return ret;
}
#endif

SR_API int ds_set_firmware_resource_manifest(int fd, GError **error)
{
    if (resources) g_hash_table_destroy(resources);
    resources = NULL;
    g_free(resource_dir);
    resource_dir = NULL;
    manifest_enabled = fd != -1;
    if (fd == -1)
        return SR_OK;
    /* Failed setup stays enabled with an empty cache: never fall back. */
#ifdef _WIN32
    g_set_error_literal(error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
            "Resource manifest descriptors require POSIX");
    return SR_ERR;
#else
    char *text = read_manifest(fd, error);
    if (!text)
        return SR_ERR;
    int root = open(DS_RES_PATH, O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
    if (root < 0) {
        g_free(text);
        return resource_error(error, "Cannot open resource directory");
    }
    GHashTable *table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free_resource);
    gsize total = 0;
    int ret = SR_OK;
    char *line = text;
    while (*line) {
        char *end = strchr(line, '\n');
        if (end) *end = 0;
        /* Accept CRLF, including a final unterminated line. */
        gsize len = strlen(line);
        if (len && line[len - 1] == '\r') line[--len] = 0;
        gboolean valid = len > 65 && line[64] == ' ' && valid_path(line + 65);
        for (gsize i = 0; valid && i < 64; i++)
            valid = g_ascii_isxdigit(line[i]);
        if (!valid || g_hash_table_contains(table, line + 65)) {
            ret = resource_error(error, "Malformed or duplicate resource manifest entry");
            break;
        }
        line[64] = 0;
        struct resource_buffer *buffer = NULL;
        ret = load_resource(root, line + 65, line, &buffer, &total, error);
        if (ret != SR_OK)
            break;
        g_hash_table_insert(table, g_strdup(line + 65), buffer);
        if (!end)
            break;
        line = end + 1;
    }
    g_free(text);
    if (ret == SR_OK && !g_hash_table_size(table))
        ret = resource_error(error, "Empty resource manifest");
    if (ret == SR_OK)
        ret = check_inventory(dup(root), "", table, 0, error);
    if (close(root) && ret == SR_OK)
        ret = resource_error(error, "Cannot close resource root");
    if (ret != SR_OK) {
        g_hash_table_destroy(table);
        return ret;
    }
    resource_dir = g_strdup(DS_RES_PATH);
    resources = table;
    return SR_OK;
#endif
}
