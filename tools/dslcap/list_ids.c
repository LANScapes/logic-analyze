/* Read-only IORegistry identity listing, GPL-3.0-or-later (as dslcap).
 * No USB handle, device request, libusb initialization or libsigrok scan. */
#include <glib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "list_ids.h"
#if defined(__APPLE__) || defined(DSLCAP_LIST_IDS_TEST)
#ifdef DSLCAP_LIST_IDS_TEST
#include "test_list_ids_registry.h"
#else
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#endif

struct ids_profile { uint16_t vid, pid; const char *model; };
/* VID/PID/model projection of BOTH tables in hardware/DSL/dsl.h. Duplicate
 * speed profiles have the same model. test_list_ids checks both directions
 * against that source, so an upstream table change cannot silently drift. */
static const struct ids_profile ids_profiles[] = {
    {0x2a0e, 0x0001, "DSLogic"},
    {0x2a0e, 0x0003, "DSLogic Pro"},
    {0x2a0e, 0x0020, "DSLogic Plus"},
    {0x2a0e, 0x0021, "DSLogic Basic"},
    {0x2a0e, 0x0029, "DSLogic U2Basic"},
    {0x2a0e, 0x002a, "DSLogic U3Pro16"},
    {0x2a0e, 0x002c, "DSLogic U3Pro32"},
    {0x2a0e, 0x002d, "DSLogic U2Pro16"},
    {0x2a0e, 0x0030, "DSLogic Plus"},
    {0x2a0e, 0x0031, "DSLogic U2Basic"},
    {0x2a0e, 0x0034, "DSLogic Plus"},
    {0x2a0e, 0x0035, "DSLogic U2Basic"},
    {0x2a0e, 0x0002, "DSCope"},
    {0x2a0e, 0x0004, "DSCope20"},
    {0x2a0e, 0x0022, "DSCope B20"},
    {0x2a0e, 0x0023, "DSCope C20"},
    {0x2a0e, 0x0024, "DSCope C20P"},
    {0x2a0e, 0x0025, "DSCope C20"},
    {0x2a0e, 0x0026, "DSCope U2B20"},
    {0x2a0e, 0x0027, "DSCope U2P20"},
    {0x2a0e, 0x0028, "DSCope U2B100"},
    {0x2a0e, 0x002b, "DSCope U3P100"},
};

static const char *ids_model(uint16_t vid, uint16_t pid)
{
    for (size_t i = 0; i < G_N_ELEMENTS(ids_profiles); i++)
        if (ids_profiles[i].vid == vid && ids_profiles[i].pid == pid)
            return ids_profiles[i].model;
    return NULL;
}

/* UTF-8 is decoded without substitution, truncation or normalization. */
static void ids_json_string(const char *s)
{
    if (!s) { printf("null"); return; }
    putchar('"');
    for (; *s; s++) {
        unsigned char c = (unsigned char)*s;
        if (c < 0x20) printf("\\u%04x", c);
        else {
            if (c == '"' || c == '\\') putchar('\\');
            putchar(c);
        }
    }
    putchar('"');
}

/* Accept only losslessly represented nonnegative integer CFNumbers. Boolean,
 * data, floating-point, signed-negative and out-of-range values are unknown. */
static int ids_number(io_registry_entry_t entry, CFStringRef key,
                      uint64_t maximum, uint32_t *value)
{
    CFTypeRef property = IORegistryEntryCreateCFProperty(entry, key, kCFAllocatorDefault, 0);
    if (!property) return 0;
    int64_t n = -1;
    int valid = CFGetTypeID(property) == CFNumberGetTypeID() &&
        !CFNumberIsFloatType((CFNumberRef)property) &&
        CFNumberGetValue((CFNumberRef)property, kCFNumberSInt64Type, &n) &&
        n >= 0 && (uint64_t)n <= maximum;
    CFRelease(property);
    if (valid) *value = (uint32_t)n;
    return valid;
}

static char *ids_string(io_registry_entry_t entry, CFStringRef key)
{
    CFTypeRef property = IORegistryEntryCreateCFProperty(entry, key, kCFAllocatorDefault, 0);
    char *text = NULL;
    if (!property) return NULL;
    if (CFGetTypeID(property) != CFStringGetTypeID()) goto done;
    CFStringRef string = (CFStringRef)property;
    CFIndex units = CFStringGetLength(string), bytes = 0, written = 0;
    /* Bound allocations; reject rather than truncate an oversized identity. */
    if (units <= 0 || units > 4096) goto done;
    CFRange range = CFRangeMake(0, units);
    if (CFStringGetBytes(string, range, kCFStringEncodingUTF8, 0, false,
                         NULL, 0, &bytes) != units || bytes <= 0 || bytes > 16384)
        goto done;
    text = g_try_malloc((gsize)bytes + 1);
    if (!text) goto done;
    if (CFStringGetBytes(string, range, kCFStringEncodingUTF8, 0, false,
                         (UInt8 *)text, bytes, &written) != units || written != bytes ||
            memchr(text, 0, (size_t)bytes) || !g_utf8_validate(text, bytes, NULL)) {
        g_free(text); text = NULL;
        goto done;
    }
    text[bytes] = 0;
done:
    CFRelease(property);
    return text;
}

static int ids_release(io_object_t object)
{
    if (IOObjectRelease(object) == KERN_SUCCESS) return 0;
    fprintf(stderr, "dslcap: --list-ids: cannot release a registry handle; inventory is incomplete\n");
    return 1;
}

/* Enumerate one class only. The legacy class is consulted only when the modern
 * class produced no DSL rows and no errors, preventing cross-class duplicates.
 * Each property is a separate cached snapshot, not an atomic device identity. */
static int ids_registry_class(const char *name, int *first, int *matches)
{
    int rc = 0;
    io_iterator_t iterator = 0;
    CFMutableDictionaryRef matching = IOServiceMatching(name);
    if (!matching) {
        fprintf(stderr, "dslcap: --list-ids: cannot create registry matching dictionary\n");
        return 1;
    }
    /* The function consumes matching on BOTH success and failure. */
    kern_return_t result = IOServiceGetMatchingServices(kIOMainPortDefault, matching, &iterator);
    if (result != KERN_SUCCESS) {
        fprintf(stderr, "dslcap: --list-ids: registry enumeration failed for %s (%d); "
                "inventory is incomplete\n", name, result);
        if (iterator) ids_release(iterator);
        return 1; /* Do not conceal access/security failures with fallback. */
    }
    if (!iterator) return 0; /* Documented successful empty result. */
    io_registry_entry_t entry;
    while ((entry = IOIteratorNext(iterator))) {
        uint32_t vid = 0, pid = 0, location_id = 0, bcd_device = 0;
        int vendor_ok = ids_number(entry, CFSTR("idVendor"), UINT16_MAX, &vid);
        int product_ok = ids_number(entry, CFSTR("idProduct"), UINT16_MAX, &pid);
        if (!vendor_ok || !product_ok) {
            fprintf(stderr, "dslcap: --list-ids: missing or malformed cached VID/PID; "
                    "inventory is incomplete\n");
            rc = 1;
            rc |= ids_release(entry);
            continue;
        }
        const char *model = ids_model((uint16_t)vid, (uint16_t)pid);
        if (!model) { rc |= ids_release(entry); continue; }
        (*matches)++;
        int location_ok = ids_number(entry, CFSTR("locationID"), UINT32_MAX, &location_id) && location_id;
        char location[13];
        if (location_ok) snprintf(location, sizeof location, "loc-%08" PRIx32, location_id);
        else {
            fprintf(stderr, "dslcap: --list-ids: missing or malformed cached locationID for "
                    "%04x:%04x; identity is incomplete\n", vid, pid);
            rc = 1;
        }
        char *serial = ids_string(entry, CFSTR("USB Serial Number"));
        if (!serial) {
            fprintf(stderr, "dslcap: --list-ids: cached serial unavailable or malformed for "
                    "%04x:%04x at %s; identity is incomplete\n", vid, pid,
                    location_ok ? location : "unknown location");
            rc = 1;
        }
        /* Read optional cached descriptors, release their snapshots, and do not
         * infer firmware/FPGA state from product labels or a revision number. */
        char *product_name = ids_string(entry, CFSTR("USB Product Name"));
        int bcd_known = ids_number(entry, CFSTR("bcdDevice"), UINT16_MAX, &bcd_device);
        (void)bcd_known;
        g_free(product_name);
        printf("%s{\"vid\":%u,\"pid\":%u,\"model\":", *first ? "" : ",", vid, pid);
        *first = 0;
        ids_json_string(model);
        printf(",\"location\":"); ids_json_string(location_ok ? location : NULL);
        printf(",\"serial\":"); ids_json_string(serial);
        printf(",\"state\":\"unknown\"}");
        g_free(serial);
        rc |= ids_release(entry);
    }
    if (!IOIteratorIsValid(iterator)) {
        fprintf(stderr, "dslcap: --list-ids: registry changed during enumeration; "
                "inventory is incomplete\n");
        rc = 1;
    }
    rc |= ids_release(iterator);
    return rc;
}
#endif

#if !defined(__APPLE__) || defined(DSLCAP_LIST_IDS_TEST)
static int ids_unavailable(void)
{
    fprintf(stderr, "dslcap: --list-ids unavailable: a read-only IORegistry backend "
            "is supported only on macOS; USB was not initialized\n");
    printf("{\"devices\":[]}\n");
    return 1;
}
#endif

int dslcap_list_ids(void)
{
#ifdef DSLCAP_LIST_IDS_TEST
    /* A compile-time fake backend only, never a production override. */
    if (!ids_test_use_registry) return ids_unavailable();
#endif
#if defined(__APPLE__) || defined(DSLCAP_LIST_IDS_TEST)
    int first = 1, matches = 0;
    printf("{\"devices\":[");
    int rc = ids_registry_class("IOUSBHostDevice", &first, &matches);
    if (!rc && !matches) rc = ids_registry_class("IOUSBDevice", &first, &matches);
    printf("]}\n");
    return rc;
#else
    return ids_unavailable();
#endif
}
