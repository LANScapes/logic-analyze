/* Included by test_spool.c after its parse helper. The CLI and all of its
 * libsigrok APIs are fake-linked by test_parent_fd.c; libusb is not linked.
 * These tests prove an unconditional pre-driver rejection, not functioning
 * device discovery, claim or capture. */

static void device_guard_result(const char *dir, const char *expected, int watch, ...)
{
    int lifetime[2], out[2], err[2], phase[2], gate[2];
    assert(!pipe(lifetime) && !pipe(out) && !pipe(err) && !pipe(phase) && !pipe(gate));
    char *base = g_build_filename(dir, "capture", NULL);
    char parent_fd[32], manifest_fd[32];
    snprintf(parent_fd, sizeof parent_fd, "%d", lifetime[0]);
    snprintf(manifest_fd, sizeof manifest_fd, "%d", gate[0]);
    char *argv[40] = {"dslcap", "--out", base, "--res", "/no-such-device-guard-res"};
    int argc = 5;
    if (watch) {
        argv[argc++] = "--parent-fd";
        argv[argc++] = parent_fd;
    }
    /* An open empty pipe would block manifest preflight if it were reached.
     * Its writer is held by the test parent until bounded child completion. */
    argv[argc++] = "--res-manifest";
    argv[argc++] = manifest_fd;
    va_list ap;
    va_start(ap, watch);
    for (char *a; (a = va_arg(ap, char *));) {
        assert(argc < (int)G_N_ELEMENTS(argv) - 1);
        argv[argc++] = a;
    }
    va_end(ap);
    pid_t pid = fork();
    assert(pid >= 0);
    if (!pid) {
        assert(dup2(out[1], STDOUT_FILENO) >= 0 && dup2(err[1], STDERR_FILENO) >= 0);
        close(lifetime[1]); close(out[0]); close(out[1]); close(err[0]); close(err[1]);
        close(phase[0]); close(gate[1]);
        test_phase_fd = phase[1];
        test_gate_fd = gate[0];
        test_library_calls = 0;
        test_forbid_library = 1; /* Every library API, including init, is a trap. */
        assert(signal(SIGPIPE, SIG_DFL) != SIG_ERR);
        int rc = dslcap_main(argc, argv);
        assert(rc == 2 && test_library_calls == 0);
        exit(rc);
    }
    close(lifetime[0]); close(out[1]); close(err[1]); close(phase[1]); close(gate[0]);
    struct parent_child p = {pid, lifetime[1], out[0], err[0], phase[0], gate[1], 0};
    parent_result(&p, 2, expected, 0);
    parent_no_files(dir);
    g_free(base);
}

static void test_device_guard(void)
{
    struct options o;
    const char *valid[] = {
        "loc-20121500:100003421", "loc-00000000:0", "loc-00000001:1",
        "loc-ffffffff:ffffffffffffffff", "loc-abcdef12:a",
        "loc-20120000:10", "loc-20121500:8000000000000000",
        "loc-20121500:ffffffff", "loc-20121500:100000000",
        "loc-20121500:100003422", "loc-20121501:100003421",
    };
    const char *invalid[] = {
        "", "loc-", "loc-:1", "loc-20121500", "loc-20121500:", ":1",
        "loc-2012150:1", "loc-020121500:1", "loc-100000000:1",
        "loc-ffffffffffffffff:1", "loc-2012150A:1", "LOC-20121500:1",
        "loc-0x20121500:1", "loc--2012150:1", "loc-+2012150:1",
        "loc-2012150g:1", " loc-20121500:1", "loc-20121500 :1",
        "loc-20121500:00", "loc-20121500:01", "loc-20121500:0abcdef",
        "loc-20121500:0fffffffffffffff", "loc-20121500:0000000000000000",
        "loc-20121500:10000000000000000", "loc-20121500:fffffffffffffffff",
        "loc-20121500:99999999999999999999999999999999999999999999999",
        "loc-20121500:A", "loc-20121500:10000342A", "loc-20121500:0x1",
        "loc-20121500:+1", "loc-20121500:-1", "loc-20121500:1.0",
        "loc-20121500: 1", "loc-20121500:1 ", "loc-20121500:1\n",
        "loc-20121500:\t1", "loc-20121500:1:2", "loc-20121500::1",
        /* Old serial/topology/name/address syntax has no compatibility path. */
        "usb-1-2.3:ABC123", "usb-1-2:100003421", "usb-0-1:0",
        "loc-20121500:ABC123", "loc-20121500:serial",
        "loc-20121500:serial:with:colons", "loc-20121500:\xe5\xba\x8f\xe5\x8f\xb7",
        "loc-20121500: leading and trailing ", "loc-20121500:quote\" slash\\ newline\n",
        "1.2:100003421", "DSLogic:100003421", "null:100003421",
        "loc-20121500:\xff", "loc-20121500:\xc0\x80", "loc-20121500:\xed\xa0\x80",
    };
    char *dir = g_dir_make_tmp("dslcap-device-guard-XXXXXX", NULL);
    assert(dir);
    for (size_t i = 0; i < G_N_ELEMENTS(valid); ++i) {
        assert(parse(&o, "--out", "unused", "--device", valid[i], NULL) == 0);
        assert(!strcmp(o.device, valid[i])); /* No trimming or normalization. */
        for (int watch = 0; watch < 2; ++watch)
            device_guard_result(dir, "\"code\":\"device_selection_unavailable\"", watch,
                                "--device", valid[i], NULL);
    }
    /* Every canonical generation width fits uint64 without truncation. */
    for (size_t width = 1; width <= 16; ++width) {
        char identity[30] = "loc-20121500:";
        memset(identity + 13, 'f', width);
        identity[13 + width] = '\0';
        assert(parse(&o, "--out", "unused", "--device", identity, NULL) == 0);
        assert(!strcmp(o.device, identity));
        device_guard_result(dir, "\"code\":\"device_selection_unavailable\"", 0,
                            "--device", identity, NULL);
    }
    for (size_t i = 0; i < G_N_ELEMENTS(invalid); ++i) {
        assert(parse(&o, "--out", "unused", "--device", invalid[i], NULL) == 2);
        device_guard_result(dir, "\"option\":\"--device\"", 0, "--device", invalid[i], NULL);
    }
    device_guard_result(dir, "duplicate option", 0,
                        "--device", valid[0], "--device", valid[1], NULL);
    device_guard_result(dir, "duplicate option", 0,
                        "--device", valid[0], "--device", valid[0], NULL);
    device_guard_result(dir, "duplicate option", 0,
                        "--device", valid[0], "--device", "loc-20121500:100003422", NULL);
    device_guard_result(dir, "duplicate option", 0,
                        "--device", valid[0], "--device", NULL);
    device_guard_result(dir, "missing option value", 0, "--device", NULL);
    device_guard_result(dir, "cannot combine with --list", 0, "--device", valid[0], "--list", NULL);
    device_guard_result(dir, "cannot combine with --list", 0, "--list", "--device", valid[0], NULL);
    device_guard_result(dir, "unknown argument", 0, "--device", valid[0], "--bogus", NULL);
    device_guard_result(dir, "unknown argument", 0, "--device=loc-20121500:100003421", NULL);
    /* Incomplete earlier flags cannot consume the selector and enable legacy
     * scans. This also guards unsupported --device=... syntax as a value. */
    device_guard_result(dir, "missing option value before --device", 0, "--out", "--device", NULL);
    device_guard_result(dir, "missing option value before --device", 0, "--res", "--device", NULL);
    device_guard_result(dir, "missing option value before --device", 0,
                        "--res", "--device=loc-20121500:100003421", NULL);
    /* Logging from current main is still downstream of the capability gate. */
    for (int level = 0; level <= 5; ++level) {
        char number[2] = {(char)('0' + level), '\0'};
        device_guard_result(dir, "\"code\":\"device_selection_unavailable\"", 1,
                            "--device", valid[0], "--log-level", number, NULL);
    }
    device_guard_result(dir, "\"option\":\"--log-level\"", 0,
                        "--device", valid[0], "--log-level", "6", NULL);
    device_guard_result(dir, "duplicate option", 0,
                        "--device", valid[0], "--log-level", "1", "--log-level", "2", NULL);
    device_guard_result(dir, "\"value\":\"loc-20121500:100003421\"", 0,
                        "--device", valid[0], NULL);
    device_guard_result(dir, "\"value\":\"loc-00000000:0\"", 0,
                        "--device", valid[1], NULL);
    /* JSON echoes malformed UTF-8-valid input safely; it is still rejected. */
    device_guard_result(dir, "\"value\":\"loc-00000001:serial:with:colons\"", 0,
                        "--device", "loc-00000001:serial:with:colons", NULL);
    device_guard_result(dir, "\"value\":\"loc-20121500:quote\\\" slash\\\\ newline\\u000a", 0,
                        "--device", "loc-20121500:quote\" slash\\ newline\n", NULL);
    /* Invalid UTF-8 is omitted from JSON, so it remains a valid JSON result. */
    device_guard_result(dir, "\"option\":\"--device\"}\n", 0,
                        "--device", "loc-20121500:\xff", NULL);
    assert(!rmdir(dir));
    g_free(dir);
    puts("device guard tests passed: canonical location/uint64 generation, rejected old syntax, "
         "JSON exit 2, zero library calls, no files, rejection before manifest read");
    assert(!fflush(stdout)); /* Subsequent harness forks must not inherit this text. */
}
