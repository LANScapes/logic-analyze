/* Hardware-free CLI startup and log routing regression harness.
 *
 * The first libsigrok4DSL call in main is intercepted by a noreturn shim.
 * It exercises the real logger, then jumps back before library initialization.
 * The compiler drops all later device references; no driver is linked.
 * See README.md for macOS and Linux build commands.
 */
#include "libsigrok.h"
#include <setjmp.h>

static void first_library_call(int level) __attribute__((noreturn));
#define ds_log_level first_library_call
#define main dslcap_main
#include "dslcap.c"
#undef main
#undef ds_log_level
#include "log.h"
#undef NDEBUG
#include <assert.h>
#include <stdarg.h>
#include <sys/wait.h>

/* This harness must remain independent of USB. The dedicated listing harness
 * exercises the actual guarded implementation; these logger cases never list IDs. */
int dslcap_list_ids(void)
{
    assert(!"unexpected --list-ids in log-level harness");
    return 1;
}

static jmp_buf library_boundary;
static int library_calls, chosen_level, expect_parent_watch;

static void first_library_call(int level)
{
    library_calls++;
    chosen_level = level;
    assert((g_parent_fd >= 0) == expect_parent_watch);
    if (expect_parent_watch)
        assert(fcntl(g_parent_fd, F_GETFD) & FD_CLOEXEC);
    ds_log_level(level);
    sr_log_init();
    sr_err("error");
    sr_warn("warning");
    sr_info("info");
    sr_dbg("debug");
    sr_detail("detail");
    sr_log_uninit();
    longjmp(library_boundary, 1);
}

static void read_stream(FILE *stream, char *buf, size_t size)
{
    assert(fseek(stream, 0, SEEK_SET) == 0);
    size_t n = fread(buf, 1, size - 1, stream);
    assert(!ferror(stream) && feof(stream));
    buf[n] = '\0';
}

/* Capture both streams and intercept startup at the first library call. */
static void check_cli(const char *error, int level, ...)
{
    char *argv[32] = { "dslcap" };
    int argc = 1;
    va_list ap;
    va_start(ap, level);
    for (char *a; (a = va_arg(ap, char *));) {
        assert(argc < 31);
        argv[argc++] = a;
    }
    va_end(ap);

    FILE *out = tmpfile(), *err = tmpfile();
    assert(out && err);
    assert(fflush(stdout) == 0 && fflush(stderr) == 0);
    int saved_out = dup(STDOUT_FILENO), saved_err = dup(STDERR_FILENO);
    assert(saved_out >= 0 && saved_err >= 0);
    assert(dup2(fileno(out), STDOUT_FILENO) >= 0);
    assert(dup2(fileno(err), STDERR_FILENO) >= 0);
    library_calls = 0;
    chosen_level = -1;
    int rc;
    if (setjmp(library_boundary) == 0)
        rc = dslcap_main(argc, argv);
    else
        rc = -1; /* Valid arguments reached the intercepted library call. */
    assert(fflush(stdout) == 0 && fflush(stderr) == 0);
    assert(dup2(saved_out, STDOUT_FILENO) >= 0);
    assert(dup2(saved_err, STDERR_FILENO) >= 0);
    close(saved_out);
    close(saved_err);

    char output[1024], logs[1024];
    read_stream(out, output, sizeof output);
    read_stream(err, logs, sizeof logs);
    fclose(out);
    fclose(err);
    if (error) {
        assert(rc == 2 && library_calls == 0 && chosen_level == -1);
        assert(g_parent_fd == -1); /* Invalid log arguments also precede watcher startup. */
        assert(strcmp(output, error) == 0 && logs[0] == '\0');
    } else {
        assert(rc == -1 && library_calls == 1 && chosen_level == level);
        assert(output[0] == '\0');
        const char *messages[] = {
            "sr: error\n", "sr: warning\n", "sr: info\n", "sr: debug\n", "sr: detail\n",
        };
        char expected[1024] = "";
        for (int i = 0; i < level; i++) strcat(expected, messages[i]);
        assert(strcmp(logs, expected) == 0);
    }
}

/* Each combined case owns a fresh watcher, which ends with the child process.
 * The test parent holds the write end until the child's checks finish. */
static void check_parent_cli(const char *error, int level, const char *value, int duplicate)
{
    int watch[2];
    assert(pipe(watch) == 0);
    pid_t pid = fork();
    assert(pid >= 0);
    if (pid == 0) {
        close(watch[1]);
        char fd[32];
        snprintf(fd, sizeof fd, "%d", watch[0]);
        expect_parent_watch = !error;
        if (!value)
            check_cli(NULL, 1, "--res", "/unused", "--out", "x", "--parent-fd", fd, NULL);
        else if (duplicate)
            check_cli(error, -1, "--res", "/unused", "--out", "x", "--parent-fd", fd,
                      "--log-level", value, "--log-level", value, NULL);
        else
            check_cli(error, level, "--res", "/unused", "--out", "x", "--parent-fd", fd,
                      "--log-level", value, NULL);
        _exit(0);
    }
    close(watch[0]);
    int status;
    gint64 deadline = g_get_monotonic_time() + 3 * G_TIME_SPAN_SECOND;
    for (;;) {
        pid_t done = waitpid(pid, &status, WNOHANG);
        assert(done >= 0);
        if (done == pid) break;
        if (g_get_monotonic_time() >= deadline) {
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);
            assert(!"combined parent/log-level child did not exit promptly");
        }
        g_usleep(1000);
    }
    close(watch[1]);
    assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

int main(void)
{
    /* Explicit --res avoids executable-path or firmware-directory discovery. */
    check_cli(NULL, 1, "--res", "/unused", "--out", "x", NULL);
    for (int level = 0; level <= 5; level++) {
        char value[2] = { (char)('0' + level), '\0' };
        check_cli(NULL, level, "--res", "/unused", "--out", "x", "--log-level", value, NULL);
    }
    check_cli(NULL, 5, "--res", "/unused", "--out", "x", "--log-level", "05", NULL);

    const char *bad[] = {
        "6", "18446744073709551615", "18446744073709551616", "-1", "+1",
        " 1", "1 ", "1.0", "1e0", "0x1", "debug", "1x", "",
    };
    for (size_t i = 0; i < G_N_ELEMENTS(bad); i++) {
        char *expected = g_strdup_printf("{\"error\":\"invalid option value\","
            "\"option\":\"--log-level\",\"value\":\"%s\"}\n", bad[i]);
        check_cli(expected, -1, "--log-level", bad[i], NULL);
        g_free(expected);
    }
    check_cli("{\"error\":\"missing option value\",\"option\":\"--log-level\"}\n",
              -1, "--log-level", NULL);
    const char *duplicate = "{\"error\":\"duplicate option\",\"option\":\"--log-level\"}\n";
    check_cli(duplicate, -1, "--log-level", "1", "--log-level", "1", NULL);
    check_cli(duplicate, -1, "--log-level", "0", "--log-level", "5", NULL);
    check_cli(duplicate, -1, "--log-level", "1", "--log-level", NULL);
    check_cli("{\"error\":\"invalid option value\",\"option\":\"--log-level\","
              "\"value\":\"debug\\\"\\u000a\"}\n", -1, "--log-level", "debug\"\n", NULL);

    check_parent_cli(NULL, 1, NULL, 0);
    for (int level = 0; level <= 5; level++) {
        char value[2] = { (char)('0' + level), '\0' };
        check_parent_cli(NULL, level, value, 0);
    }
    check_parent_cli("{\"error\":\"invalid option value\",\"option\":\"--log-level\","
                     "\"value\":\"6\"}\n", -1, "6", 0);
    check_parent_cli("{\"error\":\"invalid option value\",\"option\":\"--log-level\","
                     "\"value\":\"debug\"}\n", -1, "debug", 0);
    check_parent_cli(duplicate, -1, "1", 1);

    puts("log-level tests passed: default, levels 0..5, strict decimal parsing, "
         "JSON errors before library calls, stderr routing, combined parent-watch startup");
    return 0;
}
