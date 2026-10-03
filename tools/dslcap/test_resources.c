/* Hardware-free manifest/loader regression test. Build instructions: README.md. */
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#undef NDEBUG
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

static int init_calls, combined_phase = -1, manifest_reported;
static int log_calls, observed_level, expected_level = 1;
static int test_lib_init(void);
static void test_set_log_level(int level);
#include "../../libsigrok4DSL/libsigrok.h"
#define ds_lib_init test_lib_init
#define ds_log_level test_set_log_level
#define main dslcap_main
#include "dslcap.c"
#undef main
#undef ds_lib_init
#undef ds_log_level
#include "log.h"

/* Inject short reads, EINTR, EOF, I/O errors and growth in the actual preflight
 * reader, rather than testing a separate hashing implementation. */
static int manifest_fd = -1, read_mode, read_calls;
static ssize_t resource_read(int fd, void *data, size_t size)
{
    if (fd == manifest_fd) {
        if (combined_phase >= 0 && !manifest_reported) {
            /* The real watcher must already exist before the first read. */
            assert(g_parent_fd >= 0 && (fcntl(g_parent_fd, F_GETFD) & FD_CLOEXEC));
            assert(log_calls == 1 && observed_level == expected_level);
            assert(write(combined_phase, "M", 1) == 1);
            manifest_reported = 1;
        }
        if (read_mode == 6) {
            errno = EIO;
            return -1;
        }
        if (read_mode == 5 && read_calls++ == 0) {
            errno = EINTR;
            return -1;
        }
        return read(fd, data, MIN(size, (size_t)17));
    }
    if (read_mode && read_calls++ == 0) {
        errno = EINTR;
        return -1;
    }
    if (read_calls > 3 && read_mode >= 2 && read_mode <= 3) {
        if (read_mode == 2) return 0;
        errno = EIO;
        return -1;
    }
    ssize_t n = read(fd, data, read_mode ? MIN(size, (size_t)3) : size);
    if (!n && read_mode == 4) {
        *(unsigned char *)data = 0;
        return 1;
    }
    return n;
}
#define read resource_read
static int allocation_failure, allocations;
static void *resource_malloc(gsize size)
{
    return allocation_failure && ++allocations == allocation_failure ? NULL : g_try_malloc(size);
}
static void *resource_malloc0(gsize size)
{
    return allocation_failure && ++allocations == allocation_failure ? NULL : g_try_malloc0(size);
}
#define g_try_malloc resource_malloc
#define g_try_malloc0 resource_malloc0
#include "../../libsigrok4DSL/resource.c"
#undef read
#undef g_try_malloc
#undef g_try_malloc0
#include "../../libsigrok4DSL/hardware/DSL/command.h"
#include "../../libsigrok4DSL/hardware/DSL/dsl.h"

/* Replace every USB side effect reachable from the real loaders. */
static int usb_calls, open_calls, close_calls, short_transfer;
static const unsigned char *expected_firmware, *expected_fpga;
static gsize firmware_size, fpga_size, uploaded;

static void test_set_log_level(int level)
{
    log_calls++;
    observed_level = level;
    if (combined_phase >= 0) {
        assert(g_parent_fd >= 0 && level == expected_level);
        assert(write(combined_phase, "L", 1) == 1);
    }
    ds_log_level(level);
}

static int test_lib_init(void)
{
    init_calls++;
    if (combined_phase >= 0) {
        assert(write(combined_phase, "I", 1) == 1);
        sr_log_init();
        sr_err("combined error");
        sr_warn("combined warning");
        sr_info("combined info");
        sr_dbg("combined debug");
        sr_detail("combined detail");
        sr_log_uninit();
    }
    return SR_ERR; /* CLI test must never initialize USB or enumerate devices. */
}

int LIBUSB_CALL libusb_open(libusb_device *dev, libusb_device_handle **hdl)
{
    (void)dev;
    open_calls++;
    *hdl = (libusb_device_handle *)(uintptr_t)1;
    return 0;
}
void LIBUSB_CALL libusb_close(libusb_device_handle *hdl)
{
    (void)hdl;
    close_calls++;
}
int LIBUSB_CALL libusb_set_configuration(libusb_device_handle *hdl, int config)
{
    (void)hdl; (void)config;
    usb_calls++;
    return 0;
}
int LIBUSB_CALL libusb_kernel_driver_active(libusb_device_handle *hdl, int interface)
{
    (void)hdl; (void)interface;
    usb_calls++;
    return 0;
}
int LIBUSB_CALL libusb_detach_kernel_driver(libusb_device_handle *hdl, int interface)
{
    (void)hdl; (void)interface;
    usb_calls++;
    return 0;
}
int LIBUSB_CALL libusb_control_transfer(libusb_device_handle *hdl, uint8_t type,
        uint8_t request, uint16_t value, uint16_t index, unsigned char *data,
        uint16_t length, unsigned int timeout)
{
    (void)hdl; (void)type; (void)index; (void)timeout;
    usb_calls++;
    if (request == 0xa0 && value != 0xe600) {
        assert(expected_firmware && value == uploaded);
        assert(uploaded + length <= firmware_size);
        assert(data == expected_firmware + uploaded); /* pointer identity */
        uploaded += length;
    } else if (request == CMD_CTL_RD) {
        memset(data, bmFPGA_INIT_B | bmGPIF_DONE | bmFPGA_DONE, length);
    }
    return short_transfer ? length - 1 : length;
}
int LIBUSB_CALL libusb_bulk_transfer(libusb_device_handle *hdl, unsigned char endpoint,
        unsigned char *data, int length, int *transferred, unsigned int timeout)
{
    (void)hdl; (void)endpoint; (void)timeout;
    usb_calls++;
    assert(data == expected_fpga && length == (int)fpga_size);
    *transferred = short_transfer ? length - 1 : length;
    return 0;
}

static char *directory, *firmware, *fpga;
static unsigned char firmware_bytes[5001], fpga_bytes[777];
static char *good_manifest;

static void write_bytes(const char *path, const void *data, gsize size)
{
    assert(g_file_set_contents(path, data, size, NULL));
}
static int setup_bytes(const char *text, gsize length)
{
    /* Pipe ownership and direct descriptor consumption, with short reads. */
    int pipefd[2];
    assert(pipe(pipefd) == 0);
    assert(write(pipefd[1], text, length) == (ssize_t)length);
    close(pipefd[1]);
    manifest_fd = pipefd[0];
    GError *error = NULL;
    read_calls = 0;
    int ret = ds_set_firmware_resource_manifest(manifest_fd, &error);
    assert(fcntl(manifest_fd, F_GETFD) >= 0); /* still caller-owned */
    assert((ret == SR_OK) == (error == NULL));
    g_clear_error(&error);
    close(manifest_fd);
    manifest_fd = -1;
    return ret;
}
static int setup(const char *text)
{
    return setup_bytes(text, strlen(text));
}
static void reset_usb(void)
{
    usb_calls = open_calls = close_calls = uploaded = short_transfer = 0;
}
static void assert_no_upload(void)
{
    reset_usb();
    assert(ezusb_upload_firmware(NULL, 1, firmware) != SR_OK);
    assert(ezusb_install_firmware(NULL, firmware) != SR_OK);
    assert(dsl_fpga_config(NULL, fpga) != SR_OK);
    assert(!usb_calls && !open_calls && !close_calls);
}
static void expect_failure(const char *manifest)
{
    assert(setup(manifest) != SR_OK);
    assert(ds_resource_manifest_enabled()); /* no fallback after failed setup */
    assert_no_upload();
}

static void test_preflight(void)
{
    char *fw_hash = g_compute_checksum_for_data(G_CHECKSUM_SHA256, firmware_bytes, sizeof firmware_bytes);
    char *bit_hash = g_compute_checksum_for_data(G_CHECKSUM_SHA256, fpga_bytes, sizeof fpga_bytes);
    good_manifest = g_strdup_printf("%s device.fw\n%s nested/device.bin\n", fw_hash, bit_hash);
    char *missing = g_strdup_printf("%s device.fw\n", fw_hash);
    expect_failure(missing); /* no firmware uploaded before missing FPGA is found */
    char *corrupt = g_strdup(good_manifest);
    gsize second = 65 + strlen("device.fw\n");
    corrupt[second] = corrupt[second] == '0' ? '1' : '0';
    expect_failure(corrupt);
    expect_failure("");
    expect_failure("\n");
    expect_failure("xyz device.fw\n");
    char *duplicate = g_strconcat(good_manifest, missing, NULL);
    expect_failure(duplicate);
    const char *paths[] = {"/absolute.bin", "../escape.bin", "nested/../device.bin", "./device.fw",
                           "nested//device.bin", "nested/", "nested\\device.bin", "bad\tname.bin"};
    for (gsize i = 0; i < G_N_ELEMENTS(paths); i++) {
        char *bad = g_strdup_printf("%s %s\n", fw_hash, paths[i]);
        expect_failure(bad);
        g_free(bad);
    }
    char nul[] = "bad\0manifest";
    assert(setup_bytes(nul, sizeof nul) != SR_OK);
    assert_no_upload();

    for (read_mode = 1; read_mode <= 5; read_mode++) {
        int ret = setup(good_manifest);
        assert((ret == SR_OK) == (read_mode == 1 || read_mode == 5));
        if (ret != SR_OK) assert_no_upload();
    }
    read_mode = 0;
    for (allocation_failure = 1; allocation_failure <= 3; allocation_failure++) {
        allocations = 0;
        expect_failure(good_manifest);
    }
    allocation_failure = 0;
    read_mode = 6;
    expect_failure(good_manifest);
    read_mode = 0;
    /* Uppercase hex, CRLF and final line without newline are accepted. */
    char *upper = g_ascii_strup(fw_hash, -1);
    char *crlf = g_strdup_printf("%s device.fw\r\n%s nested/device.bin", upper, bit_hash);
    assert(setup(crlf) == SR_OK);
    g_free(upper); g_free(crlf); g_free(duplicate); g_free(corrupt);
    g_free(missing); g_free(fw_hash); g_free(bit_hash);
}

static void test_file_errors(void)
{
    assert(unlink(firmware) == 0);
    expect_failure(good_manifest);
    assert(symlink(fpga, firmware) == 0);
    expect_failure(good_manifest);
    assert(unlink(firmware) == 0);
    assert(mkfifo(firmware, 0600) == 0);
    expect_failure(good_manifest); /* cannot hang waiting for a FIFO writer */
    assert(unlink(firmware) == 0);
    write_bytes(firmware, "", 0);
    expect_failure(good_manifest);
    int fd = open(firmware, O_WRONLY);
    assert(fd >= 0 && ftruncate(fd, 0x10001) == 0);
    close(fd);
    expect_failure(good_manifest);
    write_bytes(firmware, firmware_bytes, sizeof firmware_bytes);
    fd = open(fpga, O_WRONLY);
    assert(fd >= 0 && ftruncate(fd, 0x1000000) == 0);
    close(fd);
    expect_failure(good_manifest);
    write_bytes(fpga, fpga_bytes, sizeof fpga_bytes);
    char *link = g_build_filename(directory, "linked", NULL);
    char *nested = g_build_filename(directory, "nested", NULL);
    assert(symlink(nested, link) == 0);
    expect_failure(good_manifest);
    unlink(link); g_free(link); g_free(nested);
    /* An unlisted nested resource also makes the entire preflight fail. */
    char *extra = g_build_filename(directory, "nested", "extra.fw", NULL);
    write_bytes(extra, "x", 1);
    expect_failure(good_manifest);
    unlink(extra); g_free(extra);
    GError *error = NULL;
    assert(ds_set_firmware_resource_manifest(-2, &error) != SR_OK && error);
    g_clear_error(&error);
    assert_no_upload();
    /* Direct descriptor errors and the manifest byte bound. */
    FILE *large = tmpfile();
    char *large_text = g_malloc(MANIFEST_LIMIT + 1);
    memset(large_text, 'a', MANIFEST_LIMIT + 1);
    assert(large && fwrite(large_text, 1, MANIFEST_LIMIT + 1, large) == MANIFEST_LIMIT + 1);
    g_free(large_text);
    rewind(large);
    assert(ds_set_firmware_resource_manifest(fileno(large), &error) != SR_OK && error);
    g_clear_error(&error);
    fclose(large);
    assert_no_upload();
}

static void test_verified_uploads(void)
{
    assert(setup(good_manifest) == SR_OK);
    assert(ds_resource_buffer(firmware, 0x10000, &expected_firmware, &firmware_size) == SR_OK);
    assert(ds_resource_buffer(fpga, 0xffffff, &expected_fpga, &fpga_size) == SR_OK);
    assert(!memcmp(expected_firmware, firmware_bytes, firmware_size));
    assert(!memcmp(expected_fpga, fpga_bytes, fpga_size));
    /* Replace one path and delete the other after verification. No reread. */
    write_bytes(firmware, "changed", 7);
    unlink(fpga);
    reset_usb();
    assert(ezusb_upload_firmware(NULL, 1, firmware) == SR_OK);
    assert(uploaded == firmware_size && open_calls == 1 && close_calls == 1);
    reset_usb();
    assert(ezusb_install_firmware(NULL, firmware) == SR_OK && uploaded == firmware_size);
    reset_usb();
    assert(dsl_fpga_config(NULL, fpga) == SR_OK && usb_calls);
    reset_usb(); short_transfer = 1;
    assert(ezusb_upload_firmware(NULL, 1, firmware) != SR_OK && close_calls == 1);
    reset_usb(); short_transfer = 1;
    assert(ezusb_install_firmware(NULL, firmware) != SR_OK);
    reset_usb(); short_transfer = 1;
    assert(dsl_fpga_config(NULL, fpga) != SR_OK);
    reset_usb();
    char *absent = g_build_filename(directory, "absent.bin", NULL);
    assert(dsl_fpga_config(NULL, absent) != SR_OK);
    assert(ezusb_upload_firmware(NULL, 1, absent) != SR_OK);
    assert(ezusb_install_firmware(NULL, absent) != SR_OK);
    assert(!usb_calls && !open_calls);
    g_free(absent);
    strcpy(DS_RES_PATH, "different-directory");
    assert_no_upload(); /* changing the configured root cannot disable verification */
    ds_set_firmware_resource_dir(directory);
    assert(ds_set_firmware_resource_manifest(-1, NULL) == SR_OK);
    assert(!ds_resource_manifest_enabled() && !resources && !resource_dir);
    write_bytes(firmware, firmware_bytes, sizeof firmware_bytes);
    write_bytes(fpga, fpga_bytes, sizeof fpga_bytes);
}

/* Exercise the real CLI main. ds_lib_init is a counter stub returning an error,
 * so even a successful manifest test never touches the host's devices. */
static int run_cli(const char *manifest)
{
    FILE *input = tmpfile();
    assert(input && fwrite(manifest, 1, strlen(manifest), input) == strlen(manifest));
    rewind(input);
    char fd[32];
    snprintf(fd, sizeof fd, "%d", fileno(input));
    char *args[] = {"dslcap", "--out", "unused", "--res", directory, "--res-manifest", fd};
    fflush(stdout);
    int saved = dup(STDOUT_FILENO), null = open("/dev/null", O_WRONLY);
    assert(saved >= 0 && null >= 0 && dup2(null, STDOUT_FILENO) >= 0);
    init_calls = 0;
    log_calls = 0;
    int ret = dslcap_main(G_N_ELEMENTS(args), args);
    fflush(stdout);
    assert(dup2(saved, STDOUT_FILENO) >= 0);
    close(saved); close(null); fclose(input);
    return ret;
}

/* Both flags, with the real watcher and preflight. In the blocked case the
 * manifest writer stays open: only parent loss can terminate the child. */
static void test_combined_level(const char *manifest, int blocked, int parent_dead,
        int expected_rc, int expected_init, const char *level, int wanted_level, int duplicate)
{
    int watch[2], input[2], phase[2], output[2], errors[2];
    assert(!pipe(watch) && !pipe(input) && !pipe(phase) && !pipe(output) && !pipe(errors));
    if (!blocked) {
        assert(write(input[1], manifest, strlen(manifest)) == (ssize_t)strlen(manifest));
        close(input[1]); input[1] = -1;
    }
    if (parent_dead) { close(watch[1]); watch[1] = -1; }
    pid_t child = fork();
    assert(child >= 0);
    if (!child) {
        close(phase[0]); close(output[0]); close(errors[0]);
        if (watch[1] >= 0) close(watch[1]);
        if (input[1] >= 0) close(input[1]);
        assert(dup2(output[1], STDOUT_FILENO) >= 0);
        assert(dup2(errors[1], STDERR_FILENO) >= 0);
        close(output[1]);
        close(errors[1]);
        char parent_fd[32], resource_fd[32];
        snprintf(parent_fd, sizeof parent_fd, "%d", watch[0]);
        snprintf(resource_fd, sizeof resource_fd, "%d", input[0]);
        char *args[16] = {"dslcap", "--out", "unused", "--res", directory,
                       "--parent-fd", parent_fd, "--res-manifest", resource_fd};
        int argc = 9;
        if (level) {
            args[argc++] = "--log-level";
            args[argc++] = (char *)level;
            if (duplicate) {
                args[argc++] = "--log-level";
                args[argc++] = (char *)level;
            }
        }
        combined_phase = phase[1]; manifest_reported = 0; manifest_fd = input[0];
        expected_level = wanted_level; log_calls = 0; observed_level = -1;
        init_calls = 0; read_mode = 0; reset_usb();
        int rc = dslcap_main(argc, args);
        assert(init_calls == expected_init && !usb_calls && !open_calls);
        if (wanted_level < 0)
            assert(log_calls == 0 && g_parent_fd == -1 && !manifest_reported);
        _exit(rc);
    }
    close(watch[0]); close(input[0]); close(phase[1]); close(output[1]); close(errors[1]);
    char stages[16] = {0};
    size_t consumed = 0;
    if (blocked && !parent_dead) {
        struct pollfd fd = {.fd = phase[0], .events = POLLIN};
        /* Logging must be configured before the actual preflight reader blocks. */
        for (size_t i = 0; i < 2; i++) {
            assert(poll(&fd, 1, 2000) == 1 && read(phase[0], stages + i, 1) == 1);
            assert(stages[i] == (i == 0 ? 'L' : 'M'));
        }
        consumed = 2;
        close(watch[1]); watch[1] = -1;
    }
    gint64 deadline = g_get_monotonic_time() + 3 * G_TIME_SPAN_SECOND;
    int status;
    for (;;) {
        pid_t result = waitpid(child, &status, WNOHANG);
        assert(result >= 0);
        if (result == child) break;
        if (g_get_monotonic_time() >= deadline) {
            kill(child, SIGKILL);
            waitpid(child, &status, 0);
            assert(!"combined parent/manifest child timed out");
        }
        g_usleep(1000);
    }
    assert(WIFEXITED(status) && WEXITSTATUS(status) == expected_rc);
    char json[1024] = {0}, logs[512] = {0};
    ssize_t phases = read(phase[0], stages + consumed, sizeof stages - 1 - consumed);
    ssize_t bytes = read(output[0], json, sizeof json - 1);
    ssize_t logged = read(errors[0], logs, sizeof logs - 1);
    assert(phases >= 0 && bytes >= 0 && logged >= 0);
    phases += consumed;
    assert((strchr(stages, 'I') != NULL) == expected_init);
    if (blocked || parent_dead) {
        assert(bytes == 0 && !strchr(stages, 'I'));
        if (parent_dead) assert(phases == 0); /* no logging/preflight before parent check */
        else assert(strcmp(stages, "LM") == 0);
    } else {
        const char *error = wanted_level < 0 ? "--log-level" :
                            expected_init ? "lib init failed" : "--res-manifest";
        assert(strstr(json, error));
        assert(bytes >= 3 && json[0] == '{' && json[bytes - 2] == '}' &&
               strchr(json, '\n') == json + bytes - 1); /* exactly one JSON line */
        assert(strcmp(stages, wanted_level < 0 ? "" : expected_init ? "LMI" : "LM") == 0);
    }
    const char *messages[] = {
        "sr: combined error\n", "sr: combined warning\n", "sr: combined info\n",
        "sr: combined debug\n", "sr: combined detail\n",
    };
    char expected[512] = "";
    if (expected_init)
        for (int i = 0; i < wanted_level; i++) strcat(expected, messages[i]);
    assert(strcmp(logs, expected) == 0);
    close(phase[0]); close(output[0]); close(errors[0]);
    if (watch[1] >= 0) close(watch[1]);
    if (input[1] >= 0) close(input[1]);
}

static void test_combined(const char *manifest, int blocked, int parent_dead,
        int expected_rc, int expected_init)
{
    test_combined_level(manifest, blocked, parent_dead, expected_rc, expected_init, NULL, 1, 0);
}

int main(void)
{
    directory = g_dir_make_tmp("dslcap-res-test-XXXXXX", NULL);
    assert(directory);
    char *nested = g_build_filename(directory, "nested", NULL);
    assert(mkdir(nested, 0700) == 0);
    firmware = g_build_filename(directory, "device.fw", NULL);
    fpga = g_build_filename(nested, "device.bin", NULL);
    for (gsize i = 0; i < sizeof firmware_bytes; i++) firmware_bytes[i] = i;
    for (gsize i = 0; i < sizeof fpga_bytes; i++) fpga_bytes[i] = i * 3;
    write_bytes(firmware, firmware_bytes, sizeof firmware_bytes);
    write_bytes(fpga, fpga_bytes, sizeof fpga_bytes);
    ds_set_firmware_resource_dir(directory);
    test_preflight();
    test_file_errors();
    test_verified_uploads();
    assert(run_cli("") == 2 && init_calls == 0);
    char *missing = g_strndup(good_manifest, 64 + 1 + strlen("device.fw\n"));
    assert(run_cli(missing) == 2 && init_calls == 0);
    write_bytes(fpga, "corrupt", 7);
    assert(run_cli(good_manifest) == 2 && init_calls == 0);
    write_bytes(fpga, fpga_bytes, sizeof fpga_bytes);
    assert(run_cli(good_manifest) == 1 && init_calls == 1);
    clear_resource_manifest();
    test_combined(good_manifest, 0, 0, 1, 1);
    test_combined(missing, 0, 0, 2, 0);
    test_combined("", 1, 0, 1, 0);
    test_combined("", 1, 1, 1, 0);
    for (int level = 0; level <= 5; level++) {
        char value[2] = {(char)('0' + level), '\0'};
        test_combined_level(good_manifest, 0, 0, 1, 1, value, level, 0);
    }
    test_combined_level(missing, 0, 0, 2, 0, "4", 4, 0);
    test_combined_level("", 1, 0, 1, 0, "4", 4, 0);
    test_combined_level("", 1, 1, 1, 0, "4", 4, 0);
    test_combined_level(good_manifest, 0, 0, 2, 0, "6", -1, 0);
    test_combined_level(good_manifest, 0, 0, 2, 0, "debug", -1, 0);
    test_combined_level(good_manifest, 0, 0, 2, 0, "1", -1, 1);
    unlink(firmware); unlink(fpga); rmdir(nested); rmdir(directory);
    g_free(missing); g_free(good_manifest); g_free(firmware); g_free(fpga);
    g_free(nested); g_free(directory);
    puts("resource manifest, verified loaders and combined parent/preflight/logging tests passed (no hardware accessed)");
    return 0;
}
