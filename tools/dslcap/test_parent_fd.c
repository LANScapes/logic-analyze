/* Included by test_spool.c. Exercise the real CLI in forked processes with
 * libsigrok stubs, never USB hardware. Faults apply only to this translation
 * unit; the production binary has no test switches or environment hooks. */
#include <glib.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#undef NDEBUG
#include <assert.h>

enum parent_fault {
    PF_DUP = 1, PF_THREAD = 2, PF_POLL = 4, PF_READ = 8, PF_EINTR = 16,
    PF_SPOOL = 32, PF_TEMP = 64, PF_CONVERT = 128, PF_PUBLISH = 256, PF_STDOUT = 512,
    PF_PREMAIN = 1024,
    PF_FDOPEN = 2048, PF_OUTPUT_CREATE = 4096, PF_STDOUT_ERROR = 8192,
    PF_SIGACTION = 16384,
};
enum parent_mode { PM_GOOD, PM_INIT, PM_CAPTURE, PM_EXIT, PM_INIT_ERROR, PM_LIST_ERROR };
static int test_fault, test_mode, test_phase_fd, test_gate_fd;
static int test_library_calls, test_forbid_library;
static void test_library_call(void)
{
    ++test_library_calls;
    assert(!test_forbid_library); /* Guarded selection must never reach a driver. */
}
static int test_fcntl(int fd, int cmd, ...);
static int test_pthread_create(pthread_t *t, const pthread_attr_t *a,
                              void *(*fn)(void *), void *arg);
static int test_poll(struct pollfd *p, nfds_t n, int timeout);
static ssize_t test_read(int fd, void *buf, size_t count);
static int test_mkstemp(char *name);
static size_t test_fread(void *p, size_t size, size_t n, FILE *f);
static int test_link(const char *old, const char *new);
static int test_fflush(FILE *f);
static FILE *test_fdopen(int fd, const char *mode);
static int test_sigaction(int sig, const struct sigaction *act, struct sigaction *old);
#define fcntl test_fcntl
#define pthread_create test_pthread_create
#define poll test_poll
#define read test_read
#define g_mkstemp test_mkstemp
#define fread test_fread
#define link test_link
#define fflush test_fflush
#define fdopen test_fdopen
#define sigaction(sig, act, old) test_sigaction(sig, act, old)
#define main dslcap_main
#include "dslcap.c"
#undef main
#undef fcntl
#undef pthread_create
#undef poll
#undef read
#undef g_mkstemp
#undef fread
#undef link
#undef fflush
#undef fdopen
#undef sigaction

static void test_phase(char c)
{
    assert(write(test_phase_fd, &c, 1) == 1);
}
static void test_gate(char c)
{
    test_phase(c);
    assert(read(test_gate_fd, &c, 1) == 1);
}
static void test_at_exit(void) { test_phase('Z'); }
static int test_fcntl(int fd, int cmd, ...)
{
    if (cmd == F_GETFL) return fcntl(fd, cmd);
    assert(cmd == F_DUPFD_CLOEXEC);
    va_list ap;
    va_start(ap, cmd);
    int min = va_arg(ap, int);
    va_end(ap);
    if (test_fault & PF_DUP) { errno = EMFILE; return -1; }
    return fcntl(fd, cmd, min);
}
static int test_pthread_create(pthread_t *t, const pthread_attr_t *a,
                              void *(*fn)(void *), void *arg)
{
    if (test_fault & PF_THREAD) return EAGAIN;
    assert(fcntl(g_parent_fd, F_GETFD) & FD_CLOEXEC);
    return pthread_create(t, a, fn, arg);
}
static int test_poll(struct pollfd *p, nfds_t n, int timeout)
{
    if (test_fault & PF_POLL) { errno = EIO; return -1; }
    return poll(p, n, timeout);
}
static ssize_t test_read(int fd, void *buf, size_t count)
{
    static int interrupted;  /* accessed only by the watcher */
    if ((test_fault & PF_EINTR) && !interrupted) {
        interrupted = 1;
        errno = EINTR;
        return -1;
    }
    ssize_t n = read(fd, buf, count);
    if (n > 0 && (test_fault & PF_READ)) { errno = EIO; return -1; }
    return n;
}
static int test_mkstemp(char *name)
{
    if ((test_fault & PF_OUTPUT_CREATE) && strstr(name, ".bin.")) {
        errno = ENOSPC;
        return -1;
    }
    int fd = g_mkstemp(name);
    if (fd >= 0 && (test_fault & PF_SPOOL) && strstr(name, ".raw-")) test_gate('S');
    if (fd >= 0 && (test_fault & PF_TEMP) && strstr(name, ".bin.")) test_gate('T');
    return fd;
}
static size_t test_fread(void *p, size_t size, size_t n, FILE *f)
{
    if (test_fault & PF_CONVERT) test_gate('V');
    return fread(p, size, n, f);
}
static int test_link(const char *old, const char *new)
{
    if (test_fault & PF_PUBLISH) test_gate('L');
    if (test_fault & PF_READ) {
        /* Main holds the publication mutex here. A read error must be latched
         * before the watcher waits for that mutex; poll sees no pipe HUP. */
        gint64 deadline = g_get_monotonic_time() + 2 * G_TIME_SPAN_SECOND;
        while (!g_atomic_int_get(&g_parent_dead)) {
            assert(g_get_monotonic_time() < deadline);
            g_usleep(1000);
        }
    }
    return link(old, new);
}
static int test_fflush(FILE *f)
{
    if (f == stdout && (test_fault & PF_STDOUT) && g_parent_bin) {
        /* Hold the stdio lock to prove the watcher neither flushes nor waits
         * for stdout, even if reporting is stuck. */
        flockfile(f);
        test_gate('F');
        funlockfile(f);
    }
    if (f == stdout && (test_fault & PF_STDOUT_ERROR) && g_parent_bin) close(STDOUT_FILENO);
    return fflush(f);
}
static FILE *test_fdopen(int fd, const char *mode)
{
    if ((test_fault & PF_FDOPEN) && g_parent_tmp) { errno = ENOMEM; return NULL; }
    return fdopen(fd, mode);
}
static int test_sigaction(int sig, const struct sigaction *act, struct sigaction *old)
{
    if (test_fault & PF_SIGACTION) { errno = EINVAL; return -1; }
    return sigaction(sig, act, old);
}

/* Small synchronous DSLogic substitute for full CLI lifecycle tests. The real
 * pick_device() is used unchanged; list-only tests also expose Demo Device. */
static struct sr_channel test_channel = { .index = 0, .enabled = TRUE };
static GSList test_channels = { .data = &test_channel };
static uint64_t test_rate, test_limit;
void ds_log_level(int level) { test_library_call(); (void)level; test_phase('d'); }
void ds_set_firmware_resource_dir(const char *dir) { test_library_call(); (void)dir; test_phase('d'); }
/* This parent-only harness omits the manifest flag. Its main still references
 * the opt-in API; actual combined preflight is covered by test_resources.c. */
int ds_set_firmware_resource_manifest(int fd, GError **error)
{
    test_library_call();
    (void)error;
    assert(fd == -1);
    return SR_OK;
}
void ds_set_event_callback(dslib_event_callback_t cb) { test_library_call(); (void)cb; test_phase('d'); }
void ds_set_datafeed_callback(ds_datafeed_callback_t cb) { test_library_call(); (void)cb; test_phase('d'); }
int ds_lib_init(void)
{
    test_library_call();
    struct sigaction sa;
    assert(!sigaction(SIGPIPE, NULL, &sa));
    assert(sa.sa_handler == (g_parent_fd >= 0 ? SIG_IGN : SIG_DFL));
    test_phase('I');
    if (test_mode == PM_INIT) test_gate('i');
    return test_mode == PM_INIT_ERROR ? SR_ERR : SR_OK;
}
int ds_lib_exit(void)
{
    test_library_call();
    test_phase('E');
    if (test_mode == PM_EXIT) test_gate('e');
    return SR_OK;
}
int ds_get_device_list(struct ds_device_base_info **list, int *count)
{
    test_library_call();
    if (test_mode == PM_LIST_ERROR) return SR_ERR;
    *count = 2;
    *list = g_new0(struct ds_device_base_info, 2);
    strcpy((*list)[0].name, "Demo Device");
    strcpy((*list)[1].name, "DSLogic (test stub)");
    (*list)[1].handle = 1;
    return SR_OK;
}
int ds_active_device(ds_device_handle handle) { test_library_call(); assert(handle == 1); return SR_OK; }
int ds_get_actived_device_info(struct ds_device_full_info *info)
{
    test_library_call();
    strcpy(info->name, "DSLogic (test stub)");
    return SR_OK;
}
int ds_set_actived_device_config(const struct sr_channel *ch, const struct sr_channel_group *cg,
                                 int key, GVariant *v)
{
    test_library_call();
    (void)ch; (void)cg;
    if (key == SR_CONF_SAMPLERATE) test_rate = g_variant_get_uint64(v);
    if (key == SR_CONF_LIMIT_SAMPLES) test_limit = g_variant_get_uint64(v);
    g_variant_ref_sink(v);
    g_variant_unref(v);
    return SR_OK;
}
int ds_get_actived_device_config(const struct sr_channel *ch, const struct sr_channel_group *cg,
                                 int key, GVariant **v)
{
    test_library_call();
    (void)ch; (void)cg;
    if (key == SR_CONF_VLD_CH_NUM) *v = g_variant_new_int16(1);
    else if (key == SR_CONF_SAMPLERATE) *v = g_variant_new_uint64(test_rate);
    else if (key == SR_CONF_LIMIT_SAMPLES) *v = g_variant_new_uint64(test_limit);
    else return SR_ERR;
    return SR_OK;
}
int ds_get_actived_device_config_list(const struct sr_channel_group *cg, int key, GVariant **v)
{
    test_library_call();
    (void)cg;
    static const struct sr_list_item modes[] = {{0, "test"}, {-1, NULL}};
    if (key == SR_CONF_CHANNEL_MODE) *v = g_variant_new_uint64((uint64_t)(uintptr_t)modes);
    else if (key == SR_CONF_DEVICE_OPTIONS) *v = g_variant_new_fixed_array(G_VARIANT_TYPE_INT32, NULL, 0, 4);
    else if (key == SR_CONF_SAMPLERATE) {
        GVariantBuilder b;
        uint64_t rate = 10000000;
        g_variant_builder_init(&b, G_VARIANT_TYPE_VARDICT);
        g_variant_builder_add(&b, "{sv}", "samplerates",
                             g_variant_new_fixed_array(G_VARIANT_TYPE_UINT64, &rate, 1, 8));
        *v = g_variant_builder_end(&b);
    } else return SR_ERR;
    return SR_OK;
}
GSList *ds_get_actived_device_channels(void) { test_library_call(); return &test_channels; }
int ds_enable_device_channel(const struct sr_channel *ch, gboolean enable)
{
    test_library_call();
    (void)ch; test_channel.enabled = enable; return SR_OK;
}
int ds_enable_device_channel_index(int index, gboolean enable)
{
    test_library_call();
    assert(index == 0); test_channel.enabled = enable; return SR_OK;
}
int ds_trigger_reset(void) { test_library_call(); return SR_OK; }
int ds_trigger_set_mode(uint16_t mode) { test_library_call(); (void)mode; return SR_OK; }
int ds_trigger_set_pos(uint16_t pos) { test_library_call(); (void)pos; return SR_OK; }
int ds_trigger_set_en(uint16_t enable) { test_library_call(); (void)enable; return SR_OK; }
int ds_trigger_probe_set(uint16_t probe, unsigned char a, unsigned char b)
{
    test_library_call();
    (void)probe; (void)a; (void)b; return SR_OK;
}
int ds_start_collect(void)
{
    test_library_call();
    if (test_mode == PM_CAPTURE) {
        pthread_mutex_lock(&g_lock);
        test_gate('K');  /* watcher must not need the held callback mutex */
        pthread_mutex_unlock(&g_lock);
    }
    uint64_t word = 0x0123456789abcdefULL;
    struct sr_datafeed_logic logic = {.length = 8, .format = LA_CROSS_DATA, .data = &word};
    struct sr_datafeed_packet p = {.type = SR_DF_LOGIC, .payload = &logic};
    on_data(NULL, &p);
    p.type = SR_DF_END;
    on_data(NULL, &p);
    on_event(DS_EV_COLLECT_TASK_END);
    return SR_OK;
}
int ds_is_collecting(void) { test_library_call(); return 0; }
int ds_stop_collect(void) { test_library_call(); return SR_OK; }
int ds_release_actived_device(void) { test_library_call(); return SR_OK; }

struct parent_child { pid_t pid; int writer, out, err, phase, gate, fault; };
/* NULL uses the actual read fd; "omit" leaves the flag out. Other special
 * values build open but unsuitable descriptors, otherwise pass literal text. */
static struct parent_child parent_spawn(const char *base, const char *value,
                                        int fault, int mode, int list)
{
    int watch[2], out[2], err[2], phase[2], gate[2];
    assert(!pipe(watch) && !pipe(out) && !pipe(err) && !pipe(phase) && !pipe(gate));
    pid_t pid = fork();
    assert(pid >= 0);
    if (!pid) {
        assert(dup2(out[1], STDOUT_FILENO) >= 0 && dup2(err[1], STDERR_FILENO) >= 0);
        close(out[0]); close(out[1]); close(err[0]); close(err[1]);
        close(phase[0]); close(gate[1]);
        int fd = watch[0];
        if (value && !strcmp(value, "writer")) fd = dup(watch[1]);
        else if (value && !strcmp(value, "null")) fd = open("/dev/null", O_RDONLY);
        else if (value && !strcmp(value, "dir")) fd = open(".", O_RDONLY);
        else if (value && !strcmp(value, "file")) {
            FILE *f = tmpfile();
            assert(f);
            fd = fileno(f);
        } else if (value && !strcmp(value, "socket")) {
            int pair[2];
            assert(!socketpair(AF_UNIX, SOCK_STREAM, 0, pair));
            fd = pair[0];
            close(pair[1]);
        }
        else if (value && !strcmp(value, "nonblock")) assert(!fcntl(fd, F_SETFL, O_NONBLOCK));
        else if (value && !strcmp(value, "closed")) { close(fd); fd = -1; }
        close(watch[1]); /* only the test parent keeps the actual write end */
        char number[32];
        snprintf(number, sizeof number, "%d", fd);
        const char *arg = !value || !strcmp(value, "writer") || !strcmp(value, "null") ||
            !strcmp(value, "dir") || !strcmp(value, "file") || !strcmp(value, "socket") ||
            !strcmp(value, "nonblock") ? number : value;
        if (value && !strcmp(value, "closed")) {
            snprintf(number, sizeof number, "%d", watch[0]);
            arg = number;
        }
        char *argv[24] = {"dslcap", "--res", ".", "--out", (char *)base,
                         "--samples", "64", "--trigpos", "0", "--timeout", "300"};
        int argc = 11;
        if (!value || strcmp(value, "omit")) {
            argv[argc++] = "--parent-fd";
            if (!value || strcmp(value, "missing")) argv[argc++] = (char *)arg;
        }
        if (list) argv[argc++] = "--list";
        if (value && !strcmp(value, "duplicate")) {
            argv[argc - 1] = number;
            argv[argc++] = "--parent-fd";
            argv[argc++] = number;
        }
        test_fault = fault; test_mode = mode; test_phase_fd = phase[1]; test_gate_fd = gate[0];
        assert(signal(SIGPIPE, SIG_DFL) != SIG_ERR);
        assert(!atexit(test_at_exit));
        if (test_fault & PF_PREMAIN) test_gate('B');
        exit(dslcap_main(argc, argv));
    }
    close(watch[0]); close(out[1]); close(err[1]); close(phase[1]); close(gate[0]);
    return (struct parent_child){pid, watch[1], out[0], err[0], phase[0], gate[1], fault};
}
static void parent_phase(struct parent_child *p, char expected)
{
    for (;;) {
        struct pollfd fd = {.fd = p->phase, .events = POLLIN};
        assert(poll(&fd, 1, 2000) == 1);
        char c;
        assert(read(p->phase, &c, 1) == 1);
        if (c == expected) return;
    }
}
static void parent_resume(struct parent_child *p) { assert(write(p->gate, "x", 1) == 1); }
static void parent_close(struct parent_child *p) { close(p->writer); p->writer = -1; }
static int parent_wait(struct parent_child *p)
{
    gint64 deadline = g_get_monotonic_time() + 2 * G_TIME_SPAN_SECOND;
    int status;
    for (;;) {
        pid_t got = waitpid(p->pid, &status, WNOHANG);
        assert(got >= 0);
        if (got) break;
        if (g_get_monotonic_time() >= deadline) {
            kill(p->pid, SIGKILL);
            waitpid(p->pid, &status, 0);
            assert(!"child did not exit promptly");
        }
        g_usleep(1000);
    }
    if (WIFSIGNALED(status)) fprintf(stderr, "parent-fd test: child died from signal %d\n", WTERMSIG(status));
    assert(WIFEXITED(status));
    return WEXITSTATUS(status);
}
static void parent_result(struct parent_child *p, int rc, const char *json, int orphan)
{
    assert(parent_wait(p) == rc);
    char output[4096], errors[4096], phases[128];
    ssize_t n = p->out < 0 ? 0 : read(p->out, output, sizeof output - 1);
    assert(n >= 0); output[n] = '\0';
    ssize_t err_n = read(p->err, errors, sizeof errors - 1);
    assert(err_n >= 0); errors[err_n] = '\0';
    if ((p->fault & PF_STDOUT_ERROR) || (p->out < 0 && p->writer >= 0))
        assert(strstr(errors, "cannot write the result to stdout"));
    else if (p->out < 0) assert(!err_n || strstr(errors, "cannot write the result to stdout"));
    else assert(!err_n);
    n = read(p->phase, phases, sizeof phases - 1);
    assert(n >= 0); phases[n] = '\0';
    if (json) {
        if (output[0] != '{' || !strstr(output, json))
            fprintf(stderr, "expected JSON fragment [%s], got [%s]\n", json, output);
        assert(output[0] == '{' && strstr(output, json));
        assert(strchr(output, '\n') == output + strlen(output) - 1);
    } else assert(!output[0]);
    if (orphan) assert(!strchr(phases, 'Z'));
    if (rc == 2 || (orphan && !json && !n)) assert(!strchr(phases, 'd') && !strchr(phases, 'I'));
    if (p->writer >= 0) close(p->writer);
    close(p->out); close(p->err); close(p->phase); close(p->gate);
}
static void parent_no_files(const char *dir)
{
    GDir *d = g_dir_open(dir, 0, NULL);
    assert(d && !g_dir_read_name(d));
    g_dir_close(d);
}
static void test_parent_fd(void)
{
    char *dir = g_dir_make_tmp("dslcap-parent-XXXXXX", NULL);
    assert(dir);
    char *base = g_build_filename(dir, "capture", NULL);
    char *bin = g_strdup_printf("%s.bin", base);
    struct parent_child p;
    const char *bad[] = {"0", "1", "2", "-1", "+3", " 3", "3x", "", "2147483648",
                        "18446744073709551616", "2147483647", "closed", "writer", "null",
                        "dir", "file", "socket", "nonblock", "missing", "duplicate"};
    for (size_t i = 0; i < G_N_ELEMENTS(bad); i++) {
        p = parent_spawn(base, bad[i], 0, PM_GOOD, 0);
        parent_result(&p, 2, "\"option\":\"--parent-fd\"", 0);
        parent_no_files(dir);
    }
    p = parent_spawn(base, "0", 0, PM_GOOD, 1);
    parent_result(&p, 2, "\"option\":\"--parent-fd\"", 0);
    const int setup[] = {PF_DUP, PF_THREAD, PF_POLL, PF_SIGACTION};
    for (size_t i = 0; i < G_N_ELEMENTS(setup); i++) {
        p = parent_spawn(base, NULL, setup[i], PM_GOOD, 0);
        parent_result(&p, 2, "\"option\":\"--parent-fd\"", 0);
        parent_no_files(dir);
    }
    /* Already-closed pipes (also with unread bytes) never reach any ds_* call. */
    for (int buffered = 0; buffered < 2; buffered++) {
        p = parent_spawn(base, NULL, PF_PREMAIN, PM_GOOD, 0);
        parent_phase(&p, 'B');
        if (buffered) assert(write(p.writer, "bytes", 5) == 5);
        parent_close(&p);
        parent_resume(&p);
        parent_result(&p, 1, NULL, 1);
        parent_no_files(dir);
    }
    /* Pipe data is ignored; it is closure, not data, that ends the process. */
    p = parent_spawn(base, NULL, 0, PM_INIT, 0);
    parent_phase(&p, 'i');
    assert(write(p.writer, "bytes", 5) == 5);
    assert(waitpid(p.pid, NULL, WNOHANG) == 0);
    parent_close(&p);
    parent_result(&p, 1, NULL, 1);
    parent_no_files(dir);
    /* Fatal read error, interrupted read, init, held callback lock and exit. */
    p = parent_spawn(base, NULL, PF_READ, PM_INIT, 0);
    parent_phase(&p, 'i');
    assert(write(p.writer, "x", 1) == 1);
    parent_result(&p, 1, NULL, 1);
    const int modes[] = {PM_INIT, PM_CAPTURE, PM_EXIT};
    const char stages[] = {'i', 'K', 'e'};
    for (size_t i = 0; i < G_N_ELEMENTS(modes); i++) {
        p = parent_spawn(base, NULL, PF_EINTR, modes[i], 0);
        parent_phase(&p, stages[i]);
        parent_close(&p);
        parent_result(&p, 1, NULL, 1);
        parent_no_files(dir);
    }
    /* Creation/link serialize name bookkeeping. Close the parent then let the
     * syscall return to cover the otherwise hard-to-hit publication races. */
    const int races[] = {PF_SPOOL, PF_TEMP, PF_CONVERT, PF_PUBLISH, PF_STDOUT};
    const char race_stages[] = {'S', 'T', 'V', 'L', 'F'};
    for (size_t i = 0; i < G_N_ELEMENTS(races); i++) {
        p = parent_spawn(base, NULL, races[i], PM_GOOD, 0);
        parent_phase(&p, race_stages[i]);
        parent_close(&p);
        if (races[i] == PF_SPOOL || races[i] == PF_TEMP || races[i] == PF_PUBLISH) parent_resume(&p);
        parent_result(&p, 1, NULL, 1);
        parent_no_files(dir);
    }
    p = parent_spawn(base, NULL, PF_PUBLISH | PF_READ, PM_GOOD, 0);
    parent_phase(&p, 'L');
    assert(write(p.writer, "x", 1) == 1); /* read error, while writer stays open */
    parent_resume(&p);
    parent_result(&p, 1, NULL, 1);
    parent_no_files(dir);
    const int io_faults[] = {PF_FDOPEN, PF_OUTPUT_CREATE, PF_STDOUT_ERROR};
    for (size_t i = 0; i < G_N_ELEMENTS(io_faults); i++) {
        p = parent_spawn(base, NULL, io_faults[i], PM_GOOD, 0);
        parent_result(&p, 1, io_faults[i] == PF_STDOUT_ERROR ? NULL : "cannot write capture data", 0);
        parent_no_files(dir);
    }
    /* A real broken pipe sends SIGPIPE by default. Keep the parent writer open
     * first to make stdout failure win deterministically over the EOF watcher. */
    p = parent_spawn(base, NULL, PF_STDOUT, PM_GOOD, 0);
    parent_phase(&p, 'F');
    close(p.out); p.out = -1;
    parent_resume(&p);
    parent_result(&p, 1, NULL, 0);
    parent_no_files(dir);
    /* The supervising parent can close both endpoints while reporting resumes.
     * Either EPIPE or EOF may win, but both must exit normally and clean up. */
    p = parent_spawn(base, NULL, PF_STDOUT, PM_GOOD, 0);
    parent_phase(&p, 'F');
    close(p.out); p.out = -1;
    parent_resume(&p);
    parent_close(&p);
    parent_result(&p, 1, NULL, 0);
    parent_no_files(dir);
    /* Normal capture and list behavior with and without the optional watch. */
    const char *flags[] = {NULL, "omit"};
    for (size_t i = 0; i < G_N_ELEMENTS(flags); i++) {
        p = parent_spawn(base, flags[i], 0, PM_GOOD, 0);
        parent_result(&p, 0, "\"bin\":", 0);
        gchar *data;
        gsize size;
        assert(g_file_get_contents(bin, &data, &size, NULL) && size == 8);
        uint64_t word;
        memcpy(&word, data, 8);
        assert(word == 0x0123456789abcdefULL);
        g_free(data);
        p = parent_spawn(base, flags[i], 0, PM_GOOD, 0);
        parent_result(&p, 1, "cannot write capture data", 0);
        assert(g_file_get_contents(bin, &data, &size, NULL) && size == 8);
        memcpy(&word, data, 8);
        assert(word == 0x0123456789abcdefULL);
        g_free(data);
        /* Orphan cleanup cannot remove a pre-existing destination either. */
        if (!flags[i]) {
            p = parent_spawn(base, NULL, 0, PM_EXIT, 0);
            parent_phase(&p, 'e');
            parent_close(&p);
            parent_result(&p, 1, NULL, 1);
            assert(g_file_test(bin, G_FILE_TEST_IS_REGULAR));
        }
        assert(!unlink(bin));
        p = parent_spawn(base, flags[i], 0, PM_GOOD, 1);
        parent_result(&p, 0, "\"devices\":[\"Demo Device\",\"DSLogic (test stub)\"]", 0);
        p = parent_spawn(base, flags[i], 0, PM_INIT_ERROR, 0);
        parent_result(&p, 1, "lib init failed", 0);
        p = parent_spawn(base, flags[i], 0, PM_LIST_ERROR, 0);
        parent_result(&p, 1, "cannot list devices", 0);
        parent_no_files(dir);
    }
    /* No flag preserves behavior when the test parent's pipe closes. */
    p = parent_spawn(base, "omit", 0, PM_CAPTURE, 0);
    parent_phase(&p, 'K');
    parent_close(&p);
    parent_resume(&p);
    parent_result(&p, 0, "\"bin\":", 0);
    assert(!unlink(bin));
    assert(!rmdir(dir));
    g_free(dir); g_free(base); g_free(bin);
    puts("parent-fd tests passed: validation, pre-init setup faults, EOF/read error, "
         "blocked lifecycle, publication races, protocol, existing files, normal completion");
}
