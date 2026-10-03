/* Included by test_spool.c. All registry functions are fake. Neither IOKit
 * nor libusb is linked. test_registry_cf.c also checks actual in-memory CF types. */
#define DSLCAP_LIST_IDS_TEST
#include "test_list_ids_registry.h"
static void ids_source_release(CFTypeRef value);
static void *ids_try_malloc(gsize size);
static CFIndex ids_string_bytes(CFStringRef value, CFRange range, CFStringEncoding encoding,
    UInt8 loss, Boolean external, UInt8 *buffer, CFIndex size, CFIndex *used);
#define CFRelease ids_source_release
#define g_try_malloc ids_try_malloc
#define CFStringGetBytes ids_string_bytes
#include "list_ids.c"
#undef CFStringGetBytes
#undef g_try_malloc
#undef CFRelease
#undef DSLCAP_LIST_IDS_TEST

enum ids_key { ID_VENDOR, ID_PRODUCT, ID_LOCATION, ID_SERIAL, ID_NAME, ID_BCD, ID_KEYS };
static const char *ids_keys[] = {"idVendor", "idProduct", "locationID",
    "USB Serial Number", "USB Product Name", "bcdDevice"};
struct ids_fixture {
    CFTypeRef property[ID_KEYS]; unsigned classes; int live, reads[ID_KEYS];
    uint64_t generation; int generation_error, generation_calls;
};
static struct ids_fixture ids_fake[32];
static int ids_count, ids_registry_calls, ids_property_copies, ids_property_releases;
static int ids_dictionaries_created, ids_dictionaries_consumed, ids_entry_live, ids_iterator_live;
static int ids_class_queries[2], ids_position[2], ids_matching_class;
static int ids_match_error, ids_query_error, ids_query_iterator, ids_null_iterator;
static int ids_invalid_iterator, ids_release_error, ids_alloc_error, ids_block, ids_string_fault;

#ifndef DSLCAP_TEST_REAL_CF
enum { IDS_NUMBER = 1, IDS_STRING, IDS_DICTIONARY, IDS_DATA, IDS_BOOLEAN };
struct ids_cf { int type, refs, floating, number_error; int64_t number; guint16 *units; CFIndex length; };
static CFTypeRef ids_cf_new(int type)
{
    CFTypeRef p = g_new0(struct ids_cf, 1); p->type = type; p->refs = 1; return p;
}
CFTypeRef CFRetain(CFTypeRef p) { assert(p && p->refs > 0); p->refs++; return p; }
void CFRelease(CFTypeRef p)
{
    assert(p && p->refs > 0);
    if (!--p->refs) { g_free(p->units); g_free(p); }
}
CFTypeID CFGetTypeID(CFTypeRef p) { assert(p && p->refs > 0); return (CFTypeID)p->type; }
CFTypeID CFNumberGetTypeID(void) { return IDS_NUMBER; }
CFTypeID CFStringGetTypeID(void) { return IDS_STRING; }
Boolean CFNumberIsFloatType(CFNumberRef p) { assert(p->type == IDS_NUMBER); return p->floating; }
Boolean CFNumberGetValue(CFNumberRef p, CFNumberType type, void *out)
{
    assert(p->type == IDS_NUMBER && type == kCFNumberSInt64Type);
    if (p->number_error) return false;
    *(int64_t *)out = p->number; return true;
}
CFIndex CFStringGetLength(CFStringRef p) { assert(p->type == IDS_STRING); return p->length; }
CFIndex CFStringGetBytes(CFStringRef p, CFRange range, CFStringEncoding encoding,
    UInt8 loss, Boolean external, UInt8 *buffer, CFIndex size, CFIndex *used)
{
    assert(p->type == IDS_STRING && range.location == 0 && range.length == p->length);
    assert(encoding == kCFStringEncodingUTF8 && !loss && !external);
    GByteArray *bytes = g_byte_array_new();
    CFIndex i;
    for (i = 0; i < p->length; i++) {
        gunichar code = p->units[i];
        if (code >= 0xd800 && code <= 0xdbff) {
            if (i + 1 >= p->length || p->units[i+1] < 0xdc00 || p->units[i+1] > 0xdfff) break;
            code = 0x10000 + ((code - 0xd800) << 10) + p->units[++i] - 0xdc00;
        } else if (code >= 0xdc00 && code <= 0xdfff) break;
        char utf8[6]; int n = g_unichar_to_utf8(code, utf8);
        g_byte_array_append(bytes, (const guint8 *)utf8, (guint)n);
    }
    *used = bytes->len;
    if (buffer) { assert(size >= (CFIndex)bytes->len); memcpy(buffer, bytes->data, bytes->len); }
    g_byte_array_unref(bytes); return i;
}
#endif

static CFTypeRef ids_new_number(int64_t n)
{
#ifdef DSLCAP_TEST_REAL_CF
    return CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt64Type, &n);
#else
    CFTypeRef p = ids_cf_new(IDS_NUMBER); p->number = n; return p;
#endif
}
static CFTypeRef ids_new_float(void)
{
#ifdef DSLCAP_TEST_REAL_CF
    double n = 1.25; return CFNumberCreate(kCFAllocatorDefault, kCFNumberFloat64Type, &n);
#else
    CFTypeRef p = ids_new_number(1); p->floating = 1; return p;
#endif
}
static CFTypeRef ids_new_string(const guint16 *units, CFIndex length)
{
#ifdef DSLCAP_TEST_REAL_CF
    return CFStringCreateWithCharacters(kCFAllocatorDefault, units, length);
#else
    CFTypeRef p = ids_cf_new(IDS_STRING); p->length = length;
    p->units = g_new(guint16, (gsize)length); memcpy(p->units, units, (size_t)length * sizeof *units);
    return p;
#endif
}
static CFTypeRef ids_new_text(const char *text)
{
    glong units = 0; gunichar2 *utf16 = g_utf8_to_utf16(text, -1, NULL, &units, NULL);
    assert(utf16); CFTypeRef p = ids_new_string(utf16, units); g_free(utf16); return p;
}
static CFTypeRef ids_new_wrong(int boolean)
{
#ifdef DSLCAP_TEST_REAL_CF
    return boolean ? CFRetain(kCFBooleanTrue) : CFDataCreate(kCFAllocatorDefault, NULL, 0);
#else
    return ids_cf_new(boolean ? IDS_BOOLEAN : IDS_DATA);
#endif
}
static void ids_source_release(CFTypeRef value) { ids_property_releases++; CFRelease(value); }
static void *ids_try_malloc(gsize size) { return ids_alloc_error ? NULL : g_try_malloc(size); }
static CFIndex ids_string_bytes(CFStringRef value, CFRange range, CFStringEncoding encoding,
    UInt8 loss, Boolean external, UInt8 *buffer, CFIndex size, CFIndex *used)
{
    CFIndex n = CFStringGetBytes(value, range, encoding, loss, external, buffer, size, used);
    if (ids_string_fault == 1 && !buffer) return n - 1;
    if (ids_string_fault == 2 && buffer) return n - 1;
    if (ids_string_fault == 3 && buffer) (*used)--;
    if (ids_string_fault == 4 && buffer && *used) buffer[0] = 0xff;
    return n;
}

CFMutableDictionaryRef IOServiceMatching(const char *name)
{
    ids_registry_calls++;
    ids_matching_class = !strcmp(name, "IOUSBHostDevice") ? 0 : 1;
    assert(!strcmp(name, ids_matching_class ? "IOUSBDevice" : "IOUSBHostDevice"));
    if (ids_match_error) return NULL;
    ids_dictionaries_created++;
#ifdef DSLCAP_TEST_REAL_CF
    return CFDictionaryCreateMutable(kCFAllocatorDefault, 0, NULL, NULL);
#else
    return ids_cf_new(IDS_DICTIONARY);
#endif
}
kern_return_t IOServiceGetMatchingServices(mach_port_t port, CFDictionaryRef matching,
                                           io_iterator_t *iterator)
{
    ids_registry_calls++;
    assert(port == kIOMainPortDefault && matching && iterator && !*iterator);
    int cls = ids_matching_class;
    assert(++ids_class_queries[cls] == 1); /* No retry, broad scan, or duplicate class query. */
    ids_dictionaries_consumed++; CFRelease(matching);
    if (ids_block == 1) test_gate('U');
    if (!ids_query_error || ids_query_iterator) {
        if (!ids_null_iterator) { *iterator = (io_iterator_t)(128 + cls); ids_iterator_live++; }
    }
    return ids_query_error ? -17 : KERN_SUCCESS;
}
io_object_t IOIteratorNext(io_iterator_t iterator)
{
    ids_registry_calls++;
    int cls = (int)iterator - 128;
    assert(cls >= 0 && cls < 2 && ids_iterator_live == 1 && !ids_entry_live);
    while (ids_position[cls] < ids_count) {
        int i = ids_position[cls]++;
        if (ids_fake[i].classes & (1U << cls)) {
            assert(!ids_fake[i].live); ids_fake[i].live = 1; ids_entry_live++;
            return (io_object_t)(i + 1);
        }
    }
    return 0;
}
boolean_t IOIteratorIsValid(io_iterator_t iterator)
{
    ids_registry_calls++; assert(iterator >= 128 && iterator <= 129 && ids_iterator_live == 1);
    return !ids_invalid_iterator;
}
kern_return_t IOObjectRelease(io_object_t object)
{
    ids_registry_calls++;
    if (object >= 128) {
        assert(object <= 129 && ids_iterator_live == 1 && !ids_entry_live); ids_iterator_live--;
        if (!ids_query_error && !ids_invalid_iterator) {
            int cls = (int)object - 128;
            for (int i = 0; i < ids_count; i++) if (ids_fake[i].classes & (1U << cls)) {
                struct ids_fixture *f = &ids_fake[i];
                assert(f->reads[ID_VENDOR] == 1 && f->reads[ID_PRODUCT] == 1);
                if (f->reads[ID_SERIAL]) {
                    for (int k = 0; k < ID_KEYS; k++) assert(f->reads[k] == 1);
                    assert(f->generation_calls == 1);
                } else {
                    for (int k = ID_LOCATION; k < ID_KEYS; k++) assert(!f->reads[k]);
                    assert(!f->generation_calls);
                }
            }
            /* A DSL row in the modern class prohibits any legacy query. */
            if (!cls && ids_position[0]) assert(!ids_class_queries[1]);
        }
    } else {
        assert(object && object <= (unsigned)ids_count);
        struct ids_fixture *f = &ids_fake[object - 1];
        assert(f->live && ids_entry_live == 1); f->live = 0; ids_entry_live--;
    }
    return ids_release_error ? -18 : KERN_SUCCESS;
}
kern_return_t IORegistryEntryGetRegistryEntryID(io_registry_entry_t entry, uint64_t *entry_id)
{
    ids_registry_calls++;
    assert(entry && entry <= (unsigned)ids_count && ids_fake[entry - 1].live && entry_id);
    struct ids_fixture *f = &ids_fake[entry - 1];
    assert(++f->generation_calls == 1 && f->reads[ID_VENDOR] == 1 && f->reads[ID_PRODUCT] == 1);
    assert(f->reads[ID_LOCATION] == 1 && !f->reads[ID_SERIAL]);
    if (ids_block == 3) test_gate('G');
    *entry_id = f->generation; /* Even a written value is invalid on API error. */
    return f->generation_error ? -23 : KERN_SUCCESS;
}
CFTypeRef IORegistryEntryCreateCFProperty(io_registry_entry_t entry, CFStringRef key,
                                         CFAllocatorRef allocator, IOOptionBits options)
{
    ids_registry_calls++;
    assert(entry && entry <= (unsigned)ids_count && ids_fake[entry - 1].live);
    assert(allocator == kCFAllocatorDefault && options == 0);
    if (ids_block == 2) test_gate('P');
    int k;
    for (k = 0; k < ID_KEYS; k++) {
#ifdef DSLCAP_TEST_REAL_CF
        char name[64]; assert(CFStringGetCString(key, name, sizeof name, kCFStringEncodingUTF8));
        if (!strcmp(name, ids_keys[k])) break;
#else
        if (!strcmp((const char *)key, ids_keys[k])) break;
#endif
    }
    assert(k < ID_KEYS && ++ids_fake[entry - 1].reads[k] == 1);
    CFTypeRef p = ids_fake[entry - 1].property[k];
    if (!p) return NULL;
    ids_property_copies++; return CFRetain(p);
}

static void ids_set(struct ids_fixture *f, int key, CFTypeRef value)
{
    if (f->property[key]) CFRelease(f->property[key]);
    f->property[key] = value;
}
static void ids_reset(void)
{
    for (int i = 0; i < ids_count; i++) for (int k = 0; k < ID_KEYS; k++)
        if (ids_fake[i].property[k]) CFRelease(ids_fake[i].property[k]);
    memset(ids_fake, 0, sizeof ids_fake);
    ids_count = ids_registry_calls = ids_property_copies = ids_property_releases = 0;
    ids_dictionaries_created = ids_dictionaries_consumed = ids_entry_live = ids_iterator_live = 0;
    memset(ids_class_queries, 0, sizeof ids_class_queries); memset(ids_position, 0, sizeof ids_position);
    ids_match_error = ids_query_error = ids_query_iterator = ids_null_iterator = 0;
    ids_invalid_iterator = ids_release_error = ids_alloc_error = ids_block = ids_string_fault = 0;
    ids_test_use_registry = 1;
}
static struct ids_fixture *ids_add(uint16_t vid, uint16_t pid, unsigned classes)
{
    assert(ids_count < 32);
    struct ids_fixture *f = &ids_fake[ids_count++]; f->classes = classes;
    f->generation = UINT64_C(0x100003421) + (uint64_t)ids_count - 1;
    f->property[ID_VENDOR] = ids_new_number(vid); f->property[ID_PRODUCT] = ids_new_number(pid);
    /* Raw locationID, not masked or rounded: 538055936 == 0x20121500. */
    f->property[ID_LOCATION] = ids_new_number(538055936);
    f->property[ID_SERIAL] = ids_new_text("X");
    f->property[ID_NAME] = ids_new_text("DSLogic Plus bootloader runtime");
    f->property[ID_BCD] = ids_new_number(0x1234);
    return f;
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
        assert(signal(SIGPIPE, SIG_DFL) != SIG_ERR);
        char fd[32]; snprintf(fd, sizeof fd, "%d", watch[0]);
        char *argv[12] = {"dslcap", "--list-ids", NULL}; int argc = 2;
        if (watched) { argv[argc++] = "--parent-fd"; argv[argc++] = fd; }
        if (conflict == 1) argv[argc++] = "--list";
        if (conflict == 2) { argv[argc++] = "--out"; argv[argc++] = "unused"; }
        if (conflict == 3) { argv[argc++] = "--res-manifest"; argv[argc++] = fd; }
        if (conflict == 4) argv[argc++] = "--list-ids";
        if (conflict == 5) { argv[1] = "--list"; argv[argc++] = "--list-ids"; }
        if (conflict == 6) { argv[argc++] = "--log-level"; argv[argc++] = "1"; }
        if (conflict == 7) { argv[1] = "--out"; argv[argc++] = "--list-ids"; }
        if (conflict == 8) { argv[1] = "--res"; argv[argc++] = "--list-ids"; }
        if (conflict == 9) {
            argv[1] = "--out"; argv[argc++] = "x";
            argv[argc++] = "--res"; argv[argc++] = "--list-ids";
        }
        if (conflict == 10) {
            argv[1] = "--out"; argv[argc++] = "x";
            argv[argc++] = "--res"; argv[argc++] = "--list-ids=true";
        }
        if (conflict == 11) argv[1] = "--list-ids=true";
        if (conflict == 12) { argv[1] = "--out"; argv[argc++] = "--list-ids="; }
        if (close_parent) { test_gate('B'); }
        int rc = dslcap_main(argc, argv);
        if (!core_expected) assert(!ids_registry_calls);
        else {
            assert(ids_registry_calls > 0);
            assert(ids_property_copies == ids_property_releases);
            assert(ids_dictionaries_created == ids_dictionaries_consumed);
            assert(!ids_entry_live && !ids_iterator_live);
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
    ssize_t n = p->out < 0 ? 0 : read(p->out, out, sizeof out - 1); assert(n >= 0); out[n] = 0;
    n = read(p->err, err, sizeof err - 1); assert(n >= 0); err[n] = 0;
    n = read(p->phase, phase, sizeof phase - 1); assert(n >= 0); phase[n] = 0;
    /* Every library stub forbids initialization/scan/config/callback/teardown. */
    assert(!strchr(phase, 'I') && !strchr(phase, 'd') && !strchr(phase, 'E'));
    if (expected_json) assert(!strcmp(out, expected_json));
    else assert(!out[0]);
    if (error) assert(strstr(err, error)); else assert(!err[0]);
    if (p->writer >= 0) close(p->writer);
    if (p->out >= 0) close(p->out);
    close(p->err); close(p->phase); close(p->gate);
}
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


#define IDS_X "{\"devices\":[{\"vid\":10766,\"pid\":1,\"model\":\"DSLogic\",\"location\":\"loc-20121500\",\"generation\":\"100003421\",\"serial\":\"X\",\"state\":\"unknown\"}]}\n"
#define IDS_NULL "{\"devices\":[{\"vid\":10766,\"pid\":1,\"model\":\"DSLogic\",\"location\":\"loc-20121500\",\"generation\":\"100003421\",\"serial\":null,\"state\":\"unknown\"}]}\n"
#define IDS_NOLOC "{\"devices\":[{\"vid\":10766,\"pid\":1,\"model\":\"DSLogic\",\"location\":null,\"generation\":\"100003421\",\"serial\":\"X\",\"state\":\"unknown\"}]}\n"
static void ids_case(int rc, const char *json, const char *error)
{
    struct parent_child p = ids_spawn(0, 0, 0, 0, 1); ids_result(&p, rc, json, error);
}
static void test_list_ids(void)
{
    ids_check_profiles();
    struct parent_child p;
    ids_reset(); ids_test_use_registry = 0;
    p = ids_spawn(0, 0, 0, 0, 0); ids_result(&p, 1, "{\"devices\":[]}\n", "unavailable");
    ids_reset(); ids_case(0, "{\"devices\":[]}\n", NULL);
    ids_reset(); ids_null_iterator = 1; ids_case(0, "{\"devices\":[]}\n", NULL);
    for (int fault = 0; fault < 3; fault++) {
        ids_reset(); ids_match_error = !fault; ids_query_error = !!fault; ids_query_iterator = fault == 2;
        ids_case(1, "{\"devices\":[]}\n", fault ? "enumeration failed" : "matching dictionary");
    }
    for (int watched = 0; watched < 2; watched++) {
        ids_reset(); ids_add(0x2a0e, 1, 3); /* Also visible through the legacy class: one row. */
        ids_add(0x2a0e, 0xffff, 1); ids_add(0x1234, 1, 1);
        p = ids_spawn(watched, 0, 0, 0, 1); ids_result(&p, 0, IDS_X, NULL);
    }
    ids_reset(); ids_add(0x1234, 1, 1); ids_add(0x2a0e, 1, 2);
    ids_case(0, "{\"devices\":[{\"vid\":10766,\"pid\":1,\"model\":\"DSLogic\",\"location\":\"loc-20121500\","
        "\"generation\":\"100003422\",\"serial\":\"X\",\"state\":\"unknown\"}]}\n", NULL);
    /* Unrelated modern entries do not block fallback; ID comes from the legacy entry. */
    ids_reset(); ids_add(0x2a0e, 1, 2); ids_case(0, IDS_X, NULL);
    for (size_t i = 0; i < G_N_ELEMENTS(ids_profiles); i++) {
        ids_reset(); ids_add(ids_profiles[i].vid, ids_profiles[i].pid, 1);
        char *expected = g_strdup_printf("{\"devices\":[{\"vid\":%u,\"pid\":%u,\"model\":\"%s\","
            "\"location\":\"loc-20121500\",\"generation\":\"100003421\",\"serial\":\"X\",\"state\":\"unknown\"}]}\n",
            ids_profiles[i].vid, ids_profiles[i].pid, ids_profiles[i].model);
        ids_case(0, expected, NULL); g_free(expected);
    }
    ids_reset(); ids_add(0x2a0e, 1, 1);
    struct ids_fixture *f = ids_add(0x2a0e, 2, 1);
    ids_set(f, ID_LOCATION, ids_new_number(538050560));
    ids_case(0, "{\"devices\":[{\"vid\":10766,\"pid\":1,\"model\":\"DSLogic\",\"location\":\"loc-20121500\",\"generation\":\"100003421\",\"serial\":\"X\",\"state\":\"unknown\"},"
        "{\"vid\":10766,\"pid\":2,\"model\":\"DSCope\",\"location\":\"loc-20120000\",\"generation\":\"100003422\",\"serial\":\"X\",\"state\":\"unknown\"}]}\n", NULL);
    ids_reset(); f = ids_add(0x2a0e, 1, 1);
    ids_set(f, ID_LOCATION, ids_new_number(UINT32_MAX));
    ids_set(f, ID_SERIAL, ids_new_text("\"\\\né:🚀"));
    ids_case(0, "{\"devices\":[{\"vid\":10766,\"pid\":1,\"model\":\"DSLogic\",\"location\":\"loc-ffffffff\",\"generation\":\"100003421\",\"serial\":\"\\\"\\\\\\u000aé:🚀\",\"state\":\"unknown\"}]}\n", NULL);
    /* Hard-coded canonical outputs exercise all uint64 bits, not a signed or
     * 32-bit intermediate. Zero is valid only with a successful API result. */
    const struct { uint64_t id; const char *text; } generations[] = {
        {0, "0"}, {1, "1"}, {15, "f"}, {UINT64_C(0x100003421), "100003421"},
        {UINT32_MAX, "ffffffff"}, {UINT64_C(0x100000000), "100000000"},
        {UINT64_C(0x8000000000000000), "8000000000000000"},
        {UINT64_MAX, "ffffffffffffffff"},
    };
    for (size_t i = 0; i < G_N_ELEMENTS(generations); i++) {
        ids_reset(); f = ids_add(0x2a0e, 1, 1); f->generation = generations[i].id;
        char *expected = g_strdup_printf("{\"devices\":[{\"vid\":10766,\"pid\":1,\"model\":\"DSLogic\","
            "\"location\":\"loc-20121500\",\"generation\":\"%s\",\"serial\":\"X\",\"state\":\"unknown\"}]}\n",
            generations[i].text);
        ids_case(0, expected, NULL); g_free(expected);
    }
    for (int absent_serial = 0; absent_serial < 2; absent_serial++) {
        ids_reset(); f = ids_add(0x2a0e, 1, 1);
        f->generation_error = 1; f->generation = absent_serial ? UINT64_MAX : 0;
        if (absent_serial) ids_set(f, ID_SERIAL, NULL);
        const char *expected = absent_serial ?
            "{\"devices\":[{\"vid\":10766,\"pid\":1,\"model\":\"DSLogic\",\"location\":\"loc-20121500\",\"generation\":null,\"serial\":null,\"state\":\"unknown\"}]}\n" :
            "{\"devices\":[{\"vid\":10766,\"pid\":1,\"model\":\"DSLogic\",\"location\":\"loc-20121500\",\"generation\":null,\"serial\":\"X\",\"state\":\"unknown\"}]}\n";
        ids_case(1, expected, "registry entry ID unavailable");
    }
    /* Owner's iSerialNumber==0 case: no serial property, complete location/ID. */
    ids_reset(); f = ids_add(0x2a0e, 0x20, 1); ids_set(f, ID_SERIAL, NULL);
    ids_case(0, "{\"devices\":[{\"vid\":10766,\"pid\":32,\"model\":\"DSLogic Plus\",\"location\":\"loc-20121500\",\"generation\":\"100003421\",\"serial\":null,\"state\":\"unknown\"}]}\n", NULL);
    for (int fault = 0; fault < 8; fault++) {
        ids_reset(); f = ids_add(0x2a0e, 1, 1);
        CFTypeRef value = NULL;
        const guint16 nul[] = {'A', 0, 'B'}, high[] = {0xd800}, low[] = {0xdc00};
        if (fault == 1) value = ids_new_wrong(0);
        if (fault == 2) value = ids_new_text("");
        if (fault == 3) value = ids_new_string(nul, 3);
        if (fault == 4) value = ids_new_string(high, 1);
        if (fault == 5) value = ids_new_string(low, 1);
        if (fault == 6) {
            guint16 long_text[4097]; for (size_t i = 0; i < G_N_ELEMENTS(long_text); i++) long_text[i] = 'Z';
            value = ids_new_string(long_text, G_N_ELEMENTS(long_text));
        }
        if (fault == 7) { value = ids_new_text("X"); ids_alloc_error = 1; }
        ids_set(f, ID_SERIAL, value);
        ids_case(fault ? 1 : 0, IDS_NULL, fault ? "cached serial present but malformed" : NULL);
    }
    for (int fault = 1; fault <= 4; fault++) {
        ids_reset(); ids_add(0x2a0e, 1, 1); ids_string_fault = fault;
        ids_case(1, IDS_NULL, "cached serial present but malformed");
    }
    ids_reset(); f = ids_add(0x2a0e, 1, 1);
    ids_set(f, ID_LOCATION, ids_new_number(1));
    ids_set(f, ID_SERIAL, ids_new_text("e\u0301:Case:A"));
    ids_case(0, "{\"devices\":[{\"vid\":10766,\"pid\":1,\"model\":\"DSLogic\",\"location\":\"loc-00000001\",\"generation\":\"100003421\",\"serial\":\"e\u0301:Case:A\",\"state\":\"unknown\"}]}\n", NULL);
    ids_reset(); f = ids_add(0x2a0e, 1, 1);
    char maximum[4097]; memset(maximum, 'Z', 4096); maximum[4096] = 0;
    ids_set(f, ID_SERIAL, ids_new_text(maximum));
    char *long_expected = g_strdup_printf("{\"devices\":[{\"vid\":10766,\"pid\":1,\"model\":\"DSLogic\","
        "\"location\":\"loc-20121500\",\"generation\":\"100003421\",\"serial\":\"%s\",\"state\":\"unknown\"}]}\n", maximum);
    ids_case(0, long_expected, NULL); g_free(long_expected);
    for (int fault = 0; fault < 7; fault++) {
        ids_reset(); f = ids_add(0x2a0e, 1, 1);
        CFTypeRef value = NULL;
        if (fault == 1) value = ids_new_wrong(1);
        if (fault == 2) value = ids_new_wrong(0);
        if (fault == 3) value = ids_new_number(-1);
        if (fault == 4) value = ids_new_number(0);
        if (fault == 5) value = ids_new_number((int64_t)UINT32_MAX + 1);
        if (fault == 6) value = ids_new_float();
        ids_set(f, ID_LOCATION, value); ids_case(1, IDS_NOLOC, "cached locationID");
    }
    for (int key = ID_VENDOR; key <= ID_PRODUCT; key++) for (int fault = 0; fault < 5; fault++) {
        ids_reset(); f = ids_add(0x2a0e, 1, 1);
        CFTypeRef value = fault == 0 ? NULL : fault == 1 ? ids_new_wrong(1) : fault == 2 ?
            ids_new_float() : ids_new_number(fault == 3 ? -1 : 65536);
        ids_set(f, key, value); ids_case(1, "{\"devices\":[]}\n", "cached VID/PID");
    }
#ifndef DSLCAP_TEST_REAL_CF
    ids_reset(); f = ids_add(0x2a0e, 1, 1); f->property[ID_LOCATION]->number_error = 1;
    ids_case(1, IDS_NOLOC, "cached locationID");
#endif
    ids_reset(); f = ids_add(0x2a0e, 1, 1);
    ids_set(f, ID_NAME, NULL); ids_set(f, ID_BCD, NULL); ids_case(0, IDS_X, NULL);
    ids_reset(); f = ids_add(0x2a0e, 1, 1);
    ids_set(f, ID_NAME, ids_new_wrong(1)); ids_set(f, ID_BCD, ids_new_float()); ids_case(0, IDS_X, NULL);
    ids_reset(); ids_add(0x2a0e, 1, 1); ids_invalid_iterator = 1;
    ids_case(1, IDS_X, "registry changed during enumeration");
    ids_reset(); ids_add(0x2a0e, 1, 1); ids_release_error = 1;
    ids_case(1, IDS_X, "cannot release a registry handle");
    for (int conflict = 1; conflict <= 12; conflict++) {
        ids_reset(); p = ids_spawn(0, 0, 0, conflict, 0);
        assert(parent_wait(&p) == 2);
        char out[1024]; ssize_t n = read(p.out, out, sizeof out - 1); assert(n > 0); out[n] = 0;
        assert(strstr(out, "\"error\":") && strchr(out, '\n') == out + strlen(out) - 1);
        close(p.writer); close(p.out); close(p.err); close(p.phase); close(p.gate);
    }
    ids_reset(); p = ids_spawn(1, 1, 0, 0, 0);
    parent_phase(&p, 'B'); parent_close(&p); parent_resume(&p); ids_result(&p, 1, NULL, NULL);
    for (int block = 1; block <= 3; block++) {
        ids_reset(); ids_add(0x2a0e, 1, 1); ids_block = block;
        p = ids_spawn(1, 0, 0, 0, 1); parent_phase(&p, block == 1 ? 'U' : block == 2 ? 'P' : 'G'); parent_close(&p);
        ids_result(&p, 1, NULL, NULL); /* Watcher interrupts fake registry operation. */
    }
    ids_reset(); p = ids_spawn(1, 0, 1, 0, 1);
    ids_result(&p, 1, NULL, "cannot write the result to stdout");
    /* Close the real stdout pipe before main, holding the parent-watch writer
     * open. This distinguishes SIGPIPE from EBADF and watcher-driven exit. */
    for (int watched = 0; watched < 2; watched++) {
        ids_reset(); p = ids_spawn(watched, 1, 0, 0, 1); parent_phase(&p, 'B');
        close(p.out); p.out = -1; parent_resume(&p);
        if (watched) ids_result(&p, 1, NULL, "cannot write the result to stdout");
        else {
            gint64 deadline = g_get_monotonic_time() + 2 * G_TIME_SPAN_SECOND;
            int status;
            for (;;) {
                pid_t got = waitpid(p.pid, &status, WNOHANG); assert(got >= 0);
                if (got) break;
                if (g_get_monotonic_time() >= deadline) {
                    kill(p.pid, SIGKILL); waitpid(p.pid, &status, 0);
                    assert(!"broken-pipe child did not exit promptly");
                }
                g_usleep(1000);
            }
            assert(WIFSIGNALED(status) && WTERMSIG(status) == SIGPIPE);
            char byte; assert(read(p.err, &byte, 1) == 0);
            close(p.writer); close(p.err); close(p.phase); close(p.gate);
        }
    }
    ids_reset();
    puts("list-ids tests passed: registry API/property allowlist, no USB/library calls, table coverage, "
         "CF types/ownership, raw locationID/entry generation, optional serial, Unicode/JSON, fallback, identity errors, parent, stdout and SIGPIPE/EPIPE");
}
