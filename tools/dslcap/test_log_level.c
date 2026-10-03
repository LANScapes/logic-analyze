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

static jmp_buf library_boundary;
static int library_calls, chosen_level;

static void first_library_call(int level)
{
    library_calls++;
    chosen_level = level;
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

    puts("log-level tests passed: default, levels 0..5, strict decimal parsing, "
         "JSON errors before library calls, stderr routing");
    return 0;
}
