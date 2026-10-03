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
        "usb-1-2.3:ABC123", "usb-0-1:serial", "usb-255-255:serial",
        "usb-1-1.2.3.4.5.6.7:serial", "usb-1-2:serial:with:colons",
        "usb-1-2:\xe5\xba\x8f\xe5\x8f\xb7", "usb-1-2: leading and trailing ",
        "usb-1-2:quote\" slash\\ newline\n",
    };
    const char *invalid[] = {
        "", "usb-1-2", "usb-1-2:", ":serial", "null:serial", "usb-1:serial",
        "1.2:serial", "DSLogic:serial", "usb-01-2:serial", "usb-1-02:serial",
        "usb-256-2:serial", "usb-1-256:serial", "usb-1-0:serial",
        "usb--1-2:serial", "usb-+1-2:serial", "usb-1-2.:serial",
        "usb-1-2..3:serial", "usb-1-1.2.3.4.5.6.7.8:serial",
        " usb-1-2:serial", "usb-1-2 :serial", "usb-1-2:\xff",
        "usb-999999999999999999999999999-2:serial",
        "usb-", "usb-0", "usb-1-", "usb-1-1.",
    };
    char *dir = g_dir_make_tmp("dslcap-device-guard-XXXXXX", NULL);
    assert(dir);
    for (size_t i = 0; i < G_N_ELEMENTS(valid); ++i) {
        assert(parse(&o, "--out", "unused", "--device", valid[i], NULL) == 0);
        assert(o.device == valid[i]); /* No trimming, normalizing or serial fallback. */
        for (int watch = 0; watch < 2; ++watch)
            device_guard_result(dir, "\"code\":\"device_selection_unavailable\"", watch,
                                "--device", valid[i], NULL);
    }
    for (size_t i = 0; i < G_N_ELEMENTS(invalid); ++i) {
        assert(parse(&o, "--out", "unused", "--device", invalid[i], NULL) == 2);
        device_guard_result(dir, "\"option\":\"--device\"", 0, "--device", invalid[i], NULL);
    }
    device_guard_result(dir, "duplicate option", 0,
                        "--device", valid[0], "--device", valid[1], NULL);
    device_guard_result(dir, "missing option value", 0, "--device", NULL);
    device_guard_result(dir, "cannot combine with --list", 0, "--device", valid[0], "--list", NULL);
    device_guard_result(dir, "cannot combine with --list", 0, "--list", "--device", valid[0], NULL);
    device_guard_result(dir, "unknown argument", 0, "--device", valid[0], "--bogus", NULL);
    device_guard_result(dir, "unknown argument", 0, "--device=usb-1-2:serial", NULL);
    device_guard_result(dir, "\"value\":\"usb-1-2:serial:with:colons\"", 0,
                        "--device", valid[4], NULL);
    device_guard_result(dir, "\"value\":\"usb-1-2:quote\\\" slash\\\\ newline\\u000a", 0,
                        "--device", valid[7], NULL);
    /* Invalid UTF-8 is omitted from JSON, so it remains a valid JSON result. */
    device_guard_result(dir, "\"option\":\"--device\"}\n", 0, "--device", invalid[20], NULL);
    assert(!rmdir(dir));
    g_free(dir);
    puts("device guard tests passed: canonical identity, verbatim UTF-8 serial, "
         "JSON exit 2, zero library calls, no files, rejection before manifest read");
    assert(!fflush(stdout)); /* Subsequent harness forks must not inherit this text. */
}
