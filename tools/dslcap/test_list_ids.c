/* Included by test_spool.c. Real CLI/core, fake libusb, bounded child processes.
 * No analyzer or USB backend is touched, including in the guard test. */
#define DSLCAP_LIST_IDS_TEST
#include "list_ids.c"
#undef DSLCAP_LIST_IDS_TEST

struct libusb_device {
    struct libusb_device_descriptor desc;
    int descriptor_error, port_error, open_error, lang_n, serial_n;
    uint8_t bus, ports[7];
    int depth, descriptor_calls, bus_calls, port_calls, open_attempts, requests;
    unsigned char lang[256], serial[256];
    libusb_device_handle handle;
};
static struct libusb_device ids_fake[32];
static libusb_device *ids_fake_list[33];
static libusb_context ids_context;
static int ids_count, ids_init_error, ids_list_error, ids_block_init;
static int ids_usb_calls, ids_open_calls, ids_close_calls, ids_requests, ids_init_calls;
static int ids_free_calls, ids_exit_calls;

static void ids_call(void) { ids_usb_calls++; }
int libusb_init(libusb_context **ctx)
{
    ids_call(); ids_init_calls++;
    assert(ctx && !*ctx && !ids_context.active);
    if (ids_block_init) test_gate('U');
    if (ids_init_error) return -1;
    ids_context.active = 1;
    *ctx = &ids_context;
    return 0;
}
void libusb_exit(libusb_context *ctx)
{
    ids_call(); ids_exit_calls++;
    assert(ctx == &ids_context && ctx->active && ids_open_calls == ids_close_calls);
    ctx->active = 0;
}
ssize_t libusb_get_device_list(libusb_context *ctx, libusb_device ***list)
{
    ids_call(); assert(ctx == &ids_context && ctx->active && !*list);
    if (ids_list_error) return -1;
    for (int i = 0; i < ids_count; i++) ids_fake_list[i] = &ids_fake[i];
    ids_fake_list[ids_count] = NULL;
    *list = ids_fake_list;
    return ids_count;
}
void libusb_free_device_list(libusb_device **list, int unref)
{
    ids_call(); ids_free_calls++;
    assert(list == ids_fake_list && unref == 1 && ids_context.active);
}
int libusb_get_device_descriptor(libusb_device *dev, struct libusb_device_descriptor *desc)
{
    ids_call(); assert(ids_context.active && ++dev->descriptor_calls == 1);
    if (dev->descriptor_error) return -1;
    *desc = dev->desc;
    return 0;
}
uint8_t libusb_get_bus_number(libusb_device *dev)
{
    ids_call(); assert(ids_context.active && ids_model(dev->desc.idVendor, dev->desc.idProduct) && ++dev->bus_calls == 1);
    return dev->bus;
}
int libusb_get_port_numbers(libusb_device *dev, uint8_t *ports, int size)
{
    ids_call(); assert(ids_context.active && size == 7 && ++dev->port_calls == 1);
    if (dev->port_error) return -1;
    memcpy(ports, dev->ports, sizeof dev->ports);
    return dev->depth;
}
int libusb_open(libusb_device *dev, libusb_device_handle **handle)
{
    ids_call(); assert(ids_context.active && dev->desc.iSerialNumber && !*handle &&
        ids_model(dev->desc.idVendor, dev->desc.idProduct) && ++dev->open_attempts == 1);
    if (dev->open_error) return -1;
    ids_open_calls++;
    dev->handle.dev = dev;
    *handle = &dev->handle;
    return 0;
}
void libusb_close(libusb_device_handle *handle)
{
    ids_call(); ids_close_calls++;
    assert(ids_context.active && handle->dev);
    handle->dev = NULL;
}
int libusb_control_transfer(libusb_device_handle *handle, uint8_t type,
    uint8_t request, uint16_t value, uint16_t index, unsigned char *data,
    uint16_t length, unsigned int timeout)
{
    ids_call(); ids_requests++;
    assert(ids_context.active && handle && handle->dev);
    /* Any reset/config/status/vendor/OUT/claim/firmware request fails here. */
    assert(type == 0x80 && request == 6 && (value >> 8) == 3);
    assert(length == 256 && timeout == 1000);
    libusb_device *dev = handle->dev;
    int n;
    const unsigned char *src;
    if (!(value & 0xff)) {
        assert(++dev->requests == 1 && index == 0); n = dev->lang_n; src = dev->lang;
    } else {
        assert(++dev->requests == 2 && (value & 0xff) == dev->desc.iSerialNumber &&
               index == (dev->lang[2] | (dev->lang[3] << 8)));
        n = dev->serial_n; src = dev->serial;
    }
    if (n > 0 && n <= length) memcpy(data, src, (size_t)n);
    return n;
}

static void ids_reset(void)
{
    memset(ids_fake, 0, sizeof ids_fake);
    ids_context.active = 0;
    ids_count = ids_init_error = ids_list_error = ids_block_init = 0;
    ids_usb_calls = ids_open_calls = ids_close_calls = ids_requests = ids_init_calls = 0;
    ids_free_calls = ids_exit_calls = 0;
    ids_test_backend_supported = 1;
}
static libusb_device *ids_add(uint16_t vid, uint16_t pid, uint8_t serial_index)
{
    assert(ids_count < 32);
    libusb_device *dev = &ids_fake[ids_count++];
    dev->desc = (struct libusb_device_descriptor){.idVendor = vid, .idProduct = pid,
        .iManufacturer = 17, .iProduct = 33, .iSerialNumber = serial_index};
    dev->bus = 1; dev->depth = 2; dev->ports[0] = 2; dev->ports[1] = 3;
    dev->lang_n = 4; dev->lang[0] = 4; dev->lang[1] = 3;
    dev->lang[2] = 9; dev->lang[3] = 4;
    dev->serial_n = 4; dev->serial[0] = 4; dev->serial[1] = 3; dev->serial[2] = 'X';
    return dev;
}
static struct parent_child ids_spawn(int watched, int close_parent, int bad_stdout,
                                     int conflict, int core_expected)
{
    int out[2], err[2], phase[2], gate[2], watch[2];
    assert(!pipe(out) && !pipe(err) && !pipe(phase) && !pipe(gate) && !pipe(watch));
    fflush(NULL);
    pid_t pid = fork(); assert(pid >= 0);
    if (!pid) {
        close(out[0]); close(err[0]); close(phase[0]); close(gate[1]); close(watch[1]);
        assert(dup2(out[1], STDOUT_FILENO) == STDOUT_FILENO);
        assert(dup2(err[1], STDERR_FILENO) == STDERR_FILENO);
        close(out[1]); close(err[1]);
        if (bad_stdout) close(STDOUT_FILENO);
        test_fault = 0; test_phase_fd = phase[1]; test_gate_fd = gate[0];
        test_ids_forbid_library = 1;
        char fd[32]; snprintf(fd, sizeof fd, "%d", watch[0]);
        char *argv[12] = {"dslcap", "--list-ids", NULL}; int argc = 2;
        if (watched) { argv[argc++] = "--parent-fd"; argv[argc++] = fd; }
        if (conflict == 1) argv[argc++] = "--list";
        if (conflict == 2) { argv[argc++] = "--out"; argv[argc++] = "unused"; }
        if (conflict == 3) { argv[argc++] = "--res-manifest"; argv[argc++] = fd; }
        if (conflict == 4) argv[argc++] = "--list-ids";
        if (conflict == 5) { argv[1] = "--list"; argv[argc++] = "--list-ids"; }
        if (close_parent) { test_gate('B'); }
        int rc = dslcap_main(argc, argv);
        if (!core_expected) assert(!ids_usb_calls);
        else {
            assert(ids_init_calls == 1 && ids_open_calls == ids_close_calls && !ids_context.active);
            assert(ids_free_calls == (!ids_init_error && !ids_list_error));
            assert(ids_exit_calls == !ids_init_error);
        }
        exit(rc);
    }
    close(out[1]); close(err[1]); close(phase[1]); close(gate[0]); close(watch[0]);
    return (struct parent_child){pid, watch[1], out[0], err[0], phase[0], gate[1], 0};
}
static void ids_result(struct parent_child *p, int expected_rc,
                       const char *expected_json, const char *error)
{
    assert(parent_wait(p) == expected_rc);
    char out[8192], err[8192], phase[256];
    ssize_t n = read(p->out, out, sizeof out - 1); assert(n >= 0); out[n] = 0;
    n = read(p->err, err, sizeof err - 1); assert(n >= 0); err[n] = 0;
    n = read(p->phase, phase, sizeof phase - 1); assert(n >= 0); phase[n] = 0;
    /* Every library stub forbids initialization/scan/config/callback/teardown. */
    assert(!strchr(phase, 'I') && !strchr(phase, 'd') && !strchr(phase, 'E'));
    if (expected_json) assert(!strcmp(out, expected_json));
    else assert(!out[0]);
    if (error) assert(strstr(err, error)); else assert(!err[0]);
    if (p->writer >= 0) close(p->writer);
    close(p->out); close(p->err); close(p->phase); close(p->gate);
}
#define IDS_X "{\"devices\":[{\"vid\":10766,\"pid\":1,\"model\":\"DSLogic\",\"location\":\"usb-1-2.3\",\"serial\":\"X\",\"state\":\"unknown\"}]}\n"
#define IDS_NULL "{\"devices\":[{\"vid\":10766,\"pid\":1,\"model\":\"DSLogic\",\"location\":\"usb-1-2.3\",\"serial\":null,\"state\":\"unknown\"}]}\n"

static void ids_check_profiles(void)
{
    gchar *source = NULL;
    assert(g_file_get_contents("libsigrok4DSL/hardware/DSL/dsl.h", &source, NULL, NULL));
    GRegex *r = g_regex_new("\\{(DS_VENDOR_ID|0x[0-9A-Fa-f]+),\\s*(0x[0-9A-Fa-f]+),\\s*"
        "LIBUSB_SPEED_[A-Z]+,\\s*\"DreamSourceLab\",\\s*\"([^\"]+)\"", 0, 0, NULL);
    GMatchInfo *m = NULL;
    g_regex_match(r, source, 0, &m);
    int seen[G_N_ELEMENTS(ids_profiles)] = {0}, rows = 0;
    while (g_match_info_matches(m)) {
        gchar *vid_text = g_match_info_fetch(m, 1), *pid_text = g_match_info_fetch(m, 2);
        gchar *model = g_match_info_fetch(m, 3);
        uint16_t vid = !strcmp(vid_text, "DS_VENDOR_ID") ? 0x2a0e : (uint16_t)strtoul(vid_text, NULL, 16);
        uint16_t pid = (uint16_t)strtoul(pid_text, NULL, 16);
        const char *actual = ids_model(vid, pid);
        assert(actual && !strcmp(actual, model));
        for (size_t i = 0; i < G_N_ELEMENTS(ids_profiles); i++)
            if (ids_profiles[i].vid == vid && ids_profiles[i].pid == pid) seen[i]++;
        rows++;
        g_free(vid_text); g_free(pid_text); g_free(model);
        g_match_info_next(m, NULL);
    }
    assert(rows == 25); /* Includes duplicate USB-speed entries. */
    for (size_t i = 0; i < G_N_ELEMENTS(ids_profiles); i++) assert(seen[i]);
    g_match_info_free(m); g_regex_unref(r); g_free(source);
}

static void test_list_ids(void)
{
    ids_check_profiles();
    struct parent_child p;
    ids_reset(); ids_test_backend_supported = ids_production_backend_supported();
    assert(!ids_test_backend_supported);
    p = ids_spawn(0, 0, 0, 0, 0);
    ids_result(&p, 1, "{\"devices\":[]}\n", "unavailable");
    ids_reset();
    p = ids_spawn(0, 0, 0, 0, 1); ids_result(&p, 0, "{\"devices\":[]}\n", NULL);
    for (int fault = 0; fault < 2; fault++) {
        ids_reset(); ids_init_error = !fault; ids_list_error = fault;
        p = ids_spawn(0, 0, 0, 0, 1); ids_result(&p, 1, "{\"devices\":[]}\n", fault ? "enumerate" : "initialize");
    }
    for (int watched = 0; watched < 2; watched++) {
        ids_reset(); ids_add(0x2a0e, 1, 7);
        ids_add(0x2a0e, 0xffff, 7); ids_add(0x1234, 1, 7);
        p = ids_spawn(watched, 0, 0, 0, 1); ids_result(&p, 0, IDS_X, NULL);
    }
    for (size_t i = 0; i < G_N_ELEMENTS(ids_profiles); i++) {
        ids_reset(); ids_add(ids_profiles[i].vid, ids_profiles[i].pid, 7);
        char *expected = g_strdup_printf("{\"devices\":[{\"vid\":%u,\"pid\":%u,\"model\":\"%s\","
            "\"location\":\"usb-1-2.3\",\"serial\":\"X\",\"state\":\"unknown\"}]}\n",
            ids_profiles[i].vid, ids_profiles[i].pid, ids_profiles[i].model);
        p = ids_spawn(0, 0, 0, 0, 1); ids_result(&p, 0, expected, NULL); g_free(expected);
    }
    ids_reset(); ids_add(0x2a0e, 1, 7); ids_add(0x2a0e, 2, 7);
    p = ids_spawn(0, 0, 0, 0, 1);
    ids_result(&p, 0, "{\"devices\":[{\"vid\":10766,\"pid\":1,\"model\":\"DSLogic\",\"location\":\"usb-1-2.3\",\"serial\":\"X\",\"state\":\"unknown\"},"
        "{\"vid\":10766,\"pid\":2,\"model\":\"DSCope\",\"location\":\"usb-1-2.3\",\"serial\":\"X\",\"state\":\"unknown\"}]}\n", NULL);
    ids_reset(); libusb_device *maximum = ids_add(0x2a0e, 1, 255);
    maximum->bus = 255; maximum->depth = 7;
    memset(maximum->ports, 255, sizeof maximum->ports);
    maximum->serial_n = maximum->serial[0] = 254;
    for (int i = 0; i < 126; i++) { maximum->serial[2 + i*2] = 'Z'; maximum->serial[3 + i*2] = 0; }
    char serial[127]; memset(serial, 'Z', 126); serial[126] = 0;
    char *expected = g_strdup_printf("{\"devices\":[{\"vid\":10766,\"pid\":1,\"model\":\"DSLogic\","
        "\"location\":\"usb-255-255.255.255.255.255.255.255\",\"serial\":\"%s\",\"state\":\"unknown\"}]}\n", serial);
    p = ids_spawn(0, 0, 0, 0, 1); ids_result(&p, 0, expected, NULL); g_free(expected);
    ids_reset(); ids_add(0x2a0e, 1, 0);
    p = ids_spawn(0, 0, 0, 0, 1); ids_result(&p, 0, IDS_NULL, NULL);
    for (int fault = 0; fault < 14; fault++) {
        ids_reset(); libusb_device *dev = ids_add(0x2a0e, 1, 7);
        switch (fault) {
        case 0: dev->open_error = 1; break; /* Permissions/busy/detach. */
        case 1: dev->lang_n = -1; break;
        case 2: dev->lang_n = 3; break;
        case 3: dev->lang[1] = 1; break;
        case 4: dev->lang[2] = dev->lang[3] = 0; break;
        case 5: dev->serial_n = -1; break;
        case 6: dev->serial_n = 3; break;
        case 7: dev->serial[0] = 6; break; /* Truncated/short response. */
        case 8: dev->serial[1] = 1; break;
        case 9: dev->serial[2] = 0; break; /* Embedded NUL. */
        case 10: dev->serial[3] = 0xd8; break; /* Unpaired high surrogate. */
        case 11: dev->serial[3] = 0xdc; break; /* Unpaired low surrogate. */
        case 12: dev->serial_n = 2; dev->serial[0] = 2; break; /* Empty. */
        case 13: dev->serial_n = 256; break; /* Impossible bLength. */
        }
        p = ids_spawn(0, 0, 0, 0, 1); ids_result(&p, 1, IDS_NULL, "serial descriptor index 7 unreadable");
    }
    ids_reset(); libusb_device *dev = ids_add(0x2a0e, 1, 7);
    const unsigned char unicode[] = {16, 3, '"', 0, '\\', 0, 10, 0, 0xe9, 0, ':', 0, 0x3d, 0xd8, 0x80, 0xde};
    memcpy(dev->serial, unicode, sizeof unicode); dev->serial_n = sizeof unicode;
    /* Another language is advertised; only the first is requested. */
    dev->lang_n = dev->lang[0] = 6; dev->lang[4] = 0x11; dev->lang[5] = 4;
    p = ids_spawn(0, 0, 0, 0, 1);
    ids_result(&p, 0, "{\"devices\":[{\"vid\":10766,\"pid\":1,\"model\":\"DSLogic\",\"location\":\"usb-1-2.3\",\"serial\":\"\\\"\\\\\\u000aé:🚀\",\"state\":\"unknown\"}]}\n", NULL);
    for (int fault = 0; fault < 5; fault++) {
        ids_reset(); dev = ids_add(0x2a0e, 1, 7);
        if (fault == 0) dev->port_error = 1;
        if (fault == 1) dev->depth = 0;
        if (fault == 2) dev->depth = 8;
        if (fault == 3) dev->bus = 0;
        if (fault == 4) dev->ports[1] = 0;
        p = ids_spawn(0, 0, 0, 0, 1);
        ids_result(&p, 1, "{\"devices\":[{\"vid\":10766,\"pid\":1,\"model\":\"DSLogic\",\"location\":null,\"serial\":\"X\",\"state\":\"unknown\"}]}\n", "location unavailable");
    }
    ids_reset(); dev = ids_add(0x2a0e, 1, 7); dev->descriptor_error = 1;
    p = ids_spawn(0, 0, 0, 0, 1); ids_result(&p, 1, "{\"devices\":[]}\n", "inventory is incomplete");
    for (int conflict = 1; conflict <= 5; conflict++) {
        ids_reset(); p = ids_spawn(0, 0, 0, conflict, 0);
        assert(parent_wait(&p) == 2); /* No USB/library calls: checked in child. */
        char out[1024]; ssize_t n = read(p.out, out, sizeof out - 1); assert(n > 0); out[n] = 0;
        assert(strstr(out, "\"error\":") && strchr(out, '\n') == out + strlen(out) - 1);
        close(p.writer); close(p.out); close(p.err); close(p.phase); close(p.gate);
    }
    ids_reset(); p = ids_spawn(1, 1, 0, 0, 0);
    parent_phase(&p, 'B'); parent_close(&p); parent_resume(&p);
    ids_result(&p, 1, NULL, NULL); /* Parent gone BEFORE any USB call. */
    ids_reset(); ids_block_init = 1;
    p = ids_spawn(1, 0, 0, 0, 1); parent_phase(&p, 'U'); parent_close(&p);
    ids_result(&p, 1, NULL, NULL); /* Watcher interrupts blocked fake USB init. */
    ids_reset(); p = ids_spawn(1, 0, 1, 0, 1);
    ids_result(&p, 1, NULL, "cannot write the result to stdout");
    puts("list-ids tests passed: pre-init backend guard, CLI/no-library calls, table coverage, "
         "GET_DESCRIPTOR allowlist, Unicode/JSON, absent/unreadable identity, cleanup, parent and stdout");
}
