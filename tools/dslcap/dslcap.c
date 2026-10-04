/*
 * dslcap: headless DSLogic capture on libsigrok4DSL (GPL-3.0, as DSView).
 *
 *   dslcap --list [--res DIR] [--parent-fd N] [--res-manifest FD] [--log-level N]
 *   dslcap --list-ids [--parent-fd N]
 *   dslcap --channels 0,1 --samplerate 10000000 --samples 1000000
 *          [--vth 1.6] [--mode buffer|stream] [--trigger CH[:R|F|C|1|0]]
 *          [--trigpos PERCENT] [--timeout SEC] [--res DIR] [--device NAME]
 *          [--parent-fd N] [--res-manifest FD] [--log-level N]
 *          --out /path/base
 *
 * The device is the first whose name contains NAME (default "DSLogic"). The
 * settings, recording, file layout and record are capcore.c's, shared with the
 * App Store GUI's MCP captures.
 *
 * Log level N is a whole decimal 0..5 (default 1); logs go to stderr.
 *
 * Writes <base>.bin: for each enabled channel in ascending order, the
 * channel's samples packed LSB-first, ceil(samples/64)*8 bytes per channel.
 * Prints one JSON object describing the capture on stdout; every failure
 * prints an object with an "error" key instead, and <base>.bin is only
 * created for a complete capture.
 *
 * Exit status: 0 success, 1 runtime or I/O error, 2 invalid arguments or
 * unavailable settings (including parent-watch setup), 3 the capture itself failed.
 * With --parent-fd, parent loss exits immediately with status 1 without flushing
 * stdout or emitting a parent-loss JSON result.
 */
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#include <glib.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <math.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif
#include "libsigrok.h"
#include "list_ids.h"
#include "capcore.h"

#define STOP_GRACE_US (5 * G_TIME_SPAN_SECOND)

/* Independent of g_lock and all stdio/library locks: parent loss must also
 * interrupt blocked initialization, callbacks, stdout and library teardown.
 * Only temporary-name creation/removal and atomic publication hold this lock;
 * never hold it while converting data, flushing stdout or calling the library.
 * The watch descriptor and thread live until process exit. */
static pthread_mutex_t g_parent_lock = PTHREAD_MUTEX_INITIALIZER;
static int g_parent_fd = -1;
static gint g_parent_dead = 0;          /* atomic; latch read errors before locking */
static char *g_parent_tmp = NULL;
static char *g_parent_bin = NULL;       /* our link, until the result is delivered */

/* Caller holds g_parent_lock. Do not flush stdio or attempt library cleanup. */
static void parent_lost(void)
{
    if (g_parent_tmp) unlink(g_parent_tmp);
    if (g_parent_bin) unlink(g_parent_bin);
    _exit(1);
}

/* Check synchronously at publication boundaries too: a runnable watcher may
 * not yet have observed EOF, and buffered pipe bytes do not keep a parent alive. */
static void parent_check_locked(void)
{
    if (g_atomic_int_get(&g_parent_dead)) parent_lost();
    struct pollfd p = { .fd = g_parent_fd, .events = POLLIN };
    int rc;
    do { rc = poll(&p, 1, 0); } while (rc < 0 && errno == EINTR);
    if (rc < 0 || (p.revents & (POLLHUP | POLLERR | POLLNVAL))) parent_lost();
}

static void parent_check(void)
{
    if (g_parent_fd < 0) return;
    pthread_mutex_lock(&g_parent_lock);
    parent_check_locked();
    pthread_mutex_unlock(&g_parent_lock);
}

static void *watch_parent(void *unused)
{
    (void)unused;
    char bytes[256];
    for (;;) {
        ssize_t n = read(g_parent_fd, bytes, sizeof bytes);
        if (n > 0 || (n < 0 && errno == EINTR)) continue;
        g_atomic_int_set(&g_parent_dead, 1);
        pthread_mutex_lock(&g_parent_lock);
        parent_lost();
    }
    return NULL;
}


/* Publication through capcore keeps the parent watch: a half-published file
 * is removed when the parent dies (see parent_lost). */
static int hooks_active(void) { return g_parent_fd >= 0; }
static void hooks_lock_checked(void) { pthread_mutex_lock(&g_parent_lock); parent_check_locked(); }
static void hooks_unlock(void) { pthread_mutex_unlock(&g_parent_lock); }
static const struct cap_publish_hooks parent_hooks = {
    hooks_active, hooks_lock_checked, parent_check_locked, hooks_unlock, &g_parent_tmp, &g_parent_bin,
};
#define CAP_PUBLISH_HOOKS parent_hooks
#include "capcore.c"

static void print_json_str(const char *s)
{
    putchar('"');
    for (; *s; s++) {
        if ((unsigned char)*s < 0x20)
            printf("\\u%04x", (unsigned char)*s);
        else {
            if (*s == '"' || *s == '\\') putchar('\\');
            putchar(*s);
        }
    }
    putchar('"');
}

/* The JSON result is the tool's output; failing to deliver it is an error. */
static int finish_stdout(int rc)
{
    parent_check();
    if (fflush(stdout) || ferror(stdout)) {
        fprintf(stderr, "dslcap: cannot write the result to stdout\n");
        rc = rc ? rc : 1;
    }
    if (g_parent_fd >= 0) {
        pthread_mutex_lock(&g_parent_lock);
        parent_check_locked();
        /* A complete file is retained only after its result is delivered.
         * Never unlink an existing destination: g_parent_bin is set only when
         * our link() succeeded. */
        if (g_parent_bin && ferror(stdout)) unlink(g_parent_bin);
        char *bin = g_parent_bin;
        g_parent_bin = NULL;
        pthread_mutex_unlock(&g_parent_lock);
        g_free(bin);
    }
    return rc;
}

static void arg_error(const char *what, const char *option, const char *value)
{
    printf("{\"error\":");
    print_json_str(what);
    if (option) {
        printf(",\"option\":");
        print_json_str(option);
    }
    if (value) {
        printf(",\"value\":");
        print_json_str(value);
    }
    printf("}\n");
}

/* Fail closed before any ds_* call. A join is deliberately unnecessary: the
 * watcher remains active through every normal return and dies with the process. */
static int start_parent_watch(int fd, const char *value)
{
    if (fd < 0) return 0;
    /* A supervising parent normally closes both pipes. Default SIGPIPE could
     * kill main while writing stdout/stderr before the EOF watcher can remove
     * our output. With this flag, let stdio report EPIPE and preserve cleanup. */
    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = SIG_IGN;
    if (sigemptyset(&sa.sa_mask) || sigaction(SIGPIPE, &sa, NULL)) {
        arg_error("cannot configure parent watcher signals", "--parent-fd", value);
        return 2;
    }
    int owned = fcntl(fd, F_DUPFD_CLOEXEC, 3);
    if (owned < 0) {
        arg_error("cannot duplicate parent pipe", "--parent-fd", value);
        return 2;
    }
    g_parent_fd = owned;
    struct pollfd p = { .fd = owned, .events = POLLIN };
    int poll_rc;
    do { poll_rc = poll(&p, 1, 0); } while (poll_rc < 0 && errno == EINTR);
    if (poll_rc < 0 || (p.revents & (POLLERR | POLLNVAL))) {
        g_parent_fd = -1;
        close(owned);
        arg_error("cannot check parent pipe", "--parent-fd", value);
        return 2;
    }
    /* An already-dead parent must not reach library initialization. */
    if (p.revents & POLLHUP) {
        pthread_mutex_lock(&g_parent_lock);
        parent_lost();
    }
    pthread_t thread;
    int rc = pthread_create(&thread, NULL, watch_parent, NULL);
    if (rc) {
        g_parent_fd = -1;
        close(owned);
        arg_error("cannot start parent watcher", "--parent-fd", value);
        return 2;
    }
    return 0;
}

/* Whole-string unsigned decimal; no sign, whitespace or trailing text. */
static int parse_u64(const char *s, uint64_t *out)
{
    char *end;
    if (!g_ascii_isdigit(*s)) return -1;
    errno = 0;
    unsigned long long v = strtoull(s, &end, 10);
    if (errno || *end) return -1;
    *out = v;
    return 0;
}

/* Whole-string finite number, independent of the locale. */
static int parse_double(const char *s, double *out)
{
    char *end;
    if (!*s || g_ascii_isspace(*s)) return -1;
    errno = 0;
    double v = g_ascii_strtod(s, &end);
    if (errno || end == s || *end || !isfinite(v)) return -1;
    *out = v;
    return 0;
}

struct options {
    const char *res, *out, *chans, *mode, *trig, *device;
    const char *parent_fd_value;
    uint64_t rate, samples;
    double vth, timeout;
    int trigpos, list_only, list_ids, stream, log_level;
    int res_manifest;
    int enabled[CAP_MAX_CHANNELS], nch;
    int trig_ch;
    char trig_type;
    int parent_fd;
};

/* Validates every option before the library or the device is touched.
 * Returns 0, or 2 after printing a JSON error. */
static int parse_args(int argc, char **argv, struct options *o)
{
    uint64_t u;
    int log_level_given = 0;
    memset(o, 0, sizeof *o);
    o->res = getenv("DSLCAP_RES");
    o->chans = "0";
    o->device = "DSLogic";
    o->mode = NULL;                   /* the device's current mode */
    o->rate = 10000000;
    o->samples = 1000000;
    o->vth = 1.6;
    o->timeout = 30;
    o->trigpos = 10;
    o->trig_ch = -1;
    o->log_level = 1;
    o->res_manifest = -1;
    o->parent_fd = -1;

    for (int i = 1; i < argc; i++) {
        const char *a = argv[i];
        if (!strcmp(a, "--list")) {
            o->list_only = 1;
            continue;
        }
        if (!strcmp(a, "--list-ids")) {
            if (o->list_ids) {
                arg_error("duplicate option", a, NULL);
                return 2;
            }
            o->list_ids = 1;
            continue;
        }
        static const char *const valued[] = {
            "--res", "--res-manifest", "--out", "--channels", "--samplerate", "--samples", "--vth",
            "--mode", "--trigger", "--trigpos", "--timeout", "--parent-fd", "--log-level", "--device",
        };
        int known = 0;
        for (size_t k = 0; k < G_N_ELEMENTS(valued); k++)
            if (!strcmp(a, valued[k])) known = 1;
        if (!known) {
            arg_error("unknown argument", a, NULL);
            return 2;
        }
        if (!strcmp(a, "--log-level") && log_level_given) {
            arg_error("duplicate option", a, NULL);
            return 2;
        }
        if (i + 1 >= argc) {
            arg_error("missing option value", a, NULL);
            return 2;
        }
        if (o->list_ids && strcmp(a, "--parent-fd")) {
            arg_error("--list-ids accepts only --parent-fd", a, NULL);
            return 2;
        }
        const char *v = argv[++i];
        /* A swallowed listing-mode token must never reach the legacy scan. */
        if (!strcmp(v, "--list-ids") || g_str_has_prefix(v, "--list-ids=")) {
            arg_error("listing option token is not an option value", a, v);
            return 2;
        }
        int bad = 0;
        if (!strcmp(a, "--res")) o->res = v;
        else if (!strcmp(a, "--res-manifest")) {
            bad = parse_u64(v, &u) || u > INT_MAX;
            if (!bad) {
                int flags = fcntl((int)u, F_GETFL);
                bad = flags < 0 || (flags & O_ACCMODE) == O_WRONLY;
                if (!bad) o->res_manifest = (int)u;
            }
        }
        else if (!strcmp(a, "--out")) o->out = v;
        else if (!strcmp(a, "--channels")) o->chans = v;
        else if (!strcmp(a, "--mode")) o->mode = v;
        else if (!strcmp(a, "--trigger")) o->trig = v;
        else if (!strcmp(a, "--device")) bad = !*(o->device = v);
        else if (!strcmp(a, "--parent-fd")) {
            if (o->parent_fd_value) {
                arg_error("duplicate option", a, v);
                return 2;
            }
            bad = parse_u64(v, &u) || u <= 2 || u > INT_MAX;
            if (!bad) {
                int flags = fcntl((int)u, F_GETFL);
                struct stat st;
                bad = flags < 0 || (flags & O_ACCMODE) != O_RDONLY ||
                      (flags & O_NONBLOCK) || fstat((int)u, &st) || !S_ISFIFO(st.st_mode);
            }
            if (!bad) {
                o->parent_fd = (int)u;
                o->parent_fd_value = v;
            }
        }
        else if (!strcmp(a, "--samplerate")) bad = parse_u64(v, &o->rate) || o->rate == 0;
        else if (!strcmp(a, "--samples"))
            /* The driver rounds the limit up to SAMPLES_ALIGN + 1. */
            bad = parse_u64(v, &o->samples) || o->samples == 0 ||
                  o->samples > UINT64_MAX - SAMPLES_ALIGN;
        else if (!strcmp(a, "--vth")) {
            /* The range DSView offers for the threshold voltage. */
            bad = parse_double(v, &o->vth) || o->vth < 0.0 || o->vth > 5.0;
        }
        else if (!strcmp(a, "--trigpos")) {
            bad = parse_u64(v, &u) || u > 100;
            if (!bad) o->trigpos = (int)u;
        } else if (!strcmp(a, "--log-level")) {
            log_level_given = 1;
            bad = parse_u64(v, &u) || u > 5;
            if (!bad) o->log_level = (int)u;
        } else if (!strcmp(a, "--timeout"))
            bad = parse_double(v, &o->timeout) || o->timeout <= 0 || o->timeout > 1e9;
        if (bad) {
            arg_error("invalid option value", a, v);
            return 2;
        }
    }

    if (o->list_ids) {
        for (int i = 1; i < argc; i++) {
            if (!strcmp(argv[i], "--list-ids")) continue;
            if (!strcmp(argv[i], "--parent-fd")) { i++; continue; }
            arg_error("--list-ids accepts only --parent-fd", argv[i], NULL);
            return 2;
        }
        return 0;
    }
    if (o->mode && strcmp(o->mode, "buffer") && strcmp(o->mode, "stream")) {
        arg_error("invalid option value", "--mode", o->mode);
        return 2;
    }
    o->stream = o->mode ? !strcmp(o->mode, "stream") : -1;
    if (o->list_only)
        return 0;
    /* The driver converts trigpos% of the (aligned) sample limit to a 32-bit
     * trigger position before applying its depth limit, so keep it in range. */
    struct cap_request check = { .samples = o->samples, .trigpos = o->trigpos };
    struct cap_error e;
    if (cap_check(&check, &e)) {
        printf("%s\n", e.json);
        return e.rc;
    }
    if (!o->out) {
        arg_error("--out is required", NULL, NULL);
        return 2;
    }

    /* Comma-separated channel indexes, each a plain decimal number. */
    const char *s = o->chans;
    for (;;) {
        const char *comma = strchr(s, ',');
        size_t len = comma ? (size_t)(comma - s) : strlen(s);
        char token[8];
        int bad = len == 0 || len >= sizeof token;
        if (!bad) {
            memcpy(token, s, len);
            token[len] = '\0';
            bad = parse_u64(token, &u) || u >= CAP_MAX_CHANNELS;
        }
        if (bad) {
            arg_error("invalid option value", "--channels", o->chans);
            return 2;
        }
        for (int i = 0; i < o->nch; i++) {
            if (o->enabled[i] == (int)u) {
                printf("{\"error\":\"duplicate channel\",\"channel\":%d}\n", (int)u);
                return 2;
            }
        }
        if (o->nch == CAP_MAX_CHANNELS) {
            printf("{\"error\":\"too many channels\"}\n");
            return 2;
        }
        o->enabled[o->nch++] = (int)u;
        if (!comma) break;
        s = comma + 1;
    }

    /* CH, or CH:T with T exactly one of R F C 1 0. */
    if (o->trig) {
        const char *colon = strchr(o->trig, ':');
        size_t len = colon ? (size_t)(colon - o->trig) : strlen(o->trig);
        char token[8];
        int bad = len == 0 || len >= sizeof token;
        if (!bad) {
            memcpy(token, o->trig, len);
            token[len] = '\0';
            bad = parse_u64(token, &u) || u >= MaxTriggerProbes;
        }
        o->trig_type = 'R';
        if (!bad && colon) {
            o->trig_type = colon[1];
            bad = !o->trig_type || !strchr("RFC10", o->trig_type) || colon[2];
        }
        if (bad) {
            arg_error("invalid option value", "--trigger", o->trig);
            return 2;
        }
        o->trig_ch = (int)u;
    }
    return 0;
}

static int pick_device(int list_only, const char *want)
{
    struct ds_device_base_info *list = NULL;
    int count = 0, pick = -1;
    if (ds_get_device_list(&list, &count) != SR_OK) {
        g_free(list);
        return -3;
    }
    if (list_only) printf("{\"devices\":[");
    for (int i = 0; i < count; i++) {
        if (list_only) {
            printf("%s", i ? "," : "");
            print_json_str(list[i].name);
        }
        if (pick < 0 && strstr(list[i].name, want))
            pick = i;
    }
    if (list_only) printf("]}\n");
    if (pick >= 0 && !list_only) {
        if (ds_active_device(list[pick].handle) != SR_OK)
            pick = -2;
    }
    g_free(list);
    return pick;
}


/* Firmware directory: next to this executable (app bundle Contents/Resources/res,
 * build tree DSView/res, installed ../share/DSView/res), then the configured
 * install location. */
static char *default_res_dir(void)
{
    char exe[4096] = {0};
    char *dir = NULL;
#ifdef __APPLE__
    uint32_t size = sizeof exe;
    if (_NSGetExecutablePath(exe, &size) != 0) exe[0] = '\0';
#else
    ssize_t n = readlink("/proc/self/exe", exe, sizeof exe - 1);
    if (n < 0 || n >= (ssize_t)sizeof exe - 1) exe[0] = '\0';
#endif
    char *real = exe[0] ? realpath(exe, NULL) : NULL;
    if (real) {
        dir = g_path_get_dirname(real);
        free(real);
    }
    const char *candidates[] = { "../Resources/res", "../DSView/res", "res", "../share/DSView/res" };
    char *found = NULL;
    for (size_t i = 0; dir && i < G_N_ELEMENTS(candidates) && !found; i++) {
        char *path = g_build_filename(dir, candidates[i], NULL);
        if (g_file_test(path, G_FILE_TEST_IS_DIR)) found = path;
        else g_free(path);
    }
    g_free(dir);
#ifdef DSLCAP_INSTALL_RES_DIR
    if (!found && g_file_test(DSLCAP_INSTALL_RES_DIR, G_FILE_TEST_IS_DIR))
        found = g_strdup(DSLCAP_INSTALL_RES_DIR);
#endif
    return found;
}


static void clear_resource_manifest(void)
{
    ds_set_firmware_resource_manifest(-1, NULL);
}

/* The request capcore applies, from the parsed options. */
static void to_request(const struct options *o, struct cap_request *r)
{
    memset(r, 0, sizeof *r);
    memcpy(r->channels, o->enabled, sizeof r->channels);
    r->nch = o->nch;
    r->rate = o->rate;
    r->samples = o->samples;
    r->vth = o->vth;
    r->stream = o->stream;
    r->trig_ch = o->trig ? o->trig_ch : -1;
    r->trig_type = o->trig_type;
    r->trigpos = o->trigpos;
}

int main(int argc, char **argv)
{
    struct options o;
    int rc = parse_args(argc, argv, &o);
    if (rc) return finish_stdout(rc);
    rc = start_parent_watch(o.parent_fd, o.parent_fd_value);
    if (rc) return finish_stdout(rc);

    if (o.list_ids) {
        parent_check();
        return finish_stdout(dslcap_list_ids());
    }

    char *res_found = NULL;
    const char *res = o.res;
    if (!res) res = res_found = default_res_dir();
    if (!res) {
        printf("{\"error\":\"firmware directory not found; set --res or DSLCAP_RES\"}\n");
        return finish_stdout(2);
    }
    if (strlen(res) >= 500) {
        printf("{\"error\":\"firmware directory path is too long (limit 499 bytes)\"}\n");
        g_free(res_found);
        return finish_stdout(2);
    }

    parent_check();
    ds_log_level(o.log_level);
    ds_set_firmware_resource_dir(res);
    /* The demo device's patterns are in demo/ next to res/. */
    char *usr = g_path_get_dirname(res);
    ds_set_user_data_dir(usr);
    g_free(usr);
    g_free(res_found);
    if (o.res_manifest >= 0) {
        GError *error = NULL;
        if (ds_set_firmware_resource_manifest(o.res_manifest, &error) != SR_OK) {
            arg_error(error ? error->message : "resource manifest failed", "--res-manifest", NULL);
            g_clear_error(&error);
            clear_resource_manifest();
            return finish_stdout(2);
        }
        if (atexit(clear_resource_manifest)) {
            arg_error("cannot register resource cleanup", "--res-manifest", NULL);
            clear_resource_manifest();
            return finish_stdout(2);
        }
    }
    /* The demo device needs no USB scan; a connected analyzer is left alone. */
    if (!strcmp(o.device, "Demo"))
        ds_set_no_hardware(1);
    ds_set_event_callback(on_event);
    ds_set_datafeed_callback(on_data);
    if (ds_lib_init() != SR_OK) { printf("{\"error\":\"lib init failed\"}\n"); return finish_stdout(1); }

    int dev = pick_device(o.list_only, o.device);
    if (dev == -3) {
        printf("{\"error\":\"cannot list devices\"}\n");
        ds_lib_exit();
        return finish_stdout(1);
    }
    if (o.list_only) { ds_lib_exit(); return finish_stdout(0); }
    if (dev < 0) {
        printf("{\"error\":\"%s\"}\n", dev == -1 ? "no DSLogic found" : "device activation failed");
        ds_lib_exit();
        return finish_stdout(1);
    }

    /* The same settings the GUI applies for an MCP capture (capcore.c). */
    struct cap_request req;
    struct cap_setup setup;
    struct cap_error err;
    to_request(&o, &req);
    cap_resolve_mode(&req);
    if ((rc = cap_apply(&req, &setup, &err)) != 0) {
        printf("%s\n", err.json);
        ds_lib_exit();
        return finish_stdout(rc);
    }

    if (cap_record_begin(o.out)) {
        printf("{\"error\":\"cannot create capture spool\"}\n");
        ds_lib_exit();
        return finish_stdout(1);
    }

    /* One monotonic clock for both the deadline and the reported time. */
    gint64 t0 = g_get_monotonic_time();
    gint64 deadline = t0 + (gint64)(o.timeout * G_TIME_SPAN_SECOND);
    rc = ds_start_collect();
    if (rc != SR_OK) {
        printf("{\"error\":\"start failed\",\"code\":%d}\n", rc);
        cap_record_end();
        ds_lib_exit();
        return finish_stdout(1);
    }
    int timed_out = !cap_wait_done(deadline);

    /* Stop early on timeout or a reported error; either way wait for the
     * collect task's own end event before trusting the data and status. */
    if (ds_is_collecting()) ds_stop_collect();
    cap_wait_task_end(g_get_monotonic_time() + STOP_GRACE_US);
    gint64 t1 = g_get_monotonic_time();

    struct cap_end end = { .timed_out = timed_out, .secs = (t1 - t0) / (double)G_TIME_SPAN_SECOND };
    char *report = NULL;
    char *bin = g_strdup_printf("%s.bin", o.out);
    int exit_code = cap_finish(&req, &setup, o.out, bin, &end, &report, NULL);
    fputs(report, stdout);
    g_free(report);
    g_free(bin);

    ds_release_actived_device();
    ds_lib_exit();
    return finish_stdout(exit_code);
}
