/* Descriptor-only identity listing, GPL-3.0-or-later (as dslcap).
 * Keep this independent of libsigrok: its initialization scans DSL hardware. */
#include <glib.h>
#include <stdint.h>
#include <stdio.h>
#include "list_ids.h"
#ifdef DSLCAP_LIST_IDS_TEST
#include "test_list_ids_usb.h"
#else
#include <libusb.h>
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

/* UTF-8 comes only from a strict UTF-16LE decode; null is never an identity. */
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

static int ids_string_descriptor(libusb_device_handle *handle, uint8_t index,
                                uint16_t lang, unsigned char data[256])
{
    /* The only control request in this module: device-recipient standard IN
     * GET_DESCRIPTOR(STRING). No ASCII helper (it substitutes '?' for Unicode). */
    int n = libusb_control_transfer(handle, LIBUSB_ENDPOINT_IN |
            LIBUSB_REQUEST_TYPE_STANDARD | LIBUSB_RECIPIENT_DEVICE,
            LIBUSB_REQUEST_GET_DESCRIPTOR, (LIBUSB_DT_STRING << 8) | index,
            lang, data, 256, 1000);
    if (n < 2 || n > 255 || data[0] != n || (n & 1) ||
            data[1] != LIBUSB_DT_STRING)
        return -1;
    return n;
}

static char *ids_serial(libusb_device *dev, uint8_t index)
{
    libusb_device_handle *handle = NULL;
    unsigned char data[256];
    char *serial = NULL;
    if (libusb_open(dev, &handle) != 0) return NULL;
    int n = ids_string_descriptor(handle, 0, 0, data);
    /* Select the first advertised language; do not invent an English fallback. */
    if (n < 4) goto done;
    uint16_t lang = (uint16_t)(data[2] | (data[3] << 8));
    if (!lang) goto done;
    n = ids_string_descriptor(handle, index, lang, data);
    if (n < 4) goto done;
    gunichar2 units[126];
    int count = (n - 2) / 2;
    for (int i = 0; i < count; i++) {
        units[i] = (gunichar2)(data[2 + 2*i] | (data[3 + 2*i] << 8));
        /* Embedded NUL would truncate a selector. Reject it, never substitute. */
        if (!units[i]) goto done;
    }
    glong read_units = 0;
    serial = g_utf16_to_utf8(units, count, &read_units, NULL, NULL);
    if (read_units != count) { g_free(serial); serial = NULL; }
done:
    libusb_close(handle);
    return serial;
}

static int ids_production_backend_supported(void)
{
    /* No audited backend currently meets the full no-side-effect contract.
     * See the pinned Darwin/Linux source audit in README.md. Refuse BEFORE init:
     * even enumeration can issue requests other than GET_DESCRIPTOR. */
    return 0;
}

static int ids_backend_supported(void)
{
#ifdef DSLCAP_LIST_IDS_TEST
    /* Compile-time fake USB harness only; no production setting can bypass it. */
    return ids_test_backend_supported;
#else
    return ids_production_backend_supported();
#endif
}

int dslcap_list_ids(void)
{
    if (!ids_backend_supported()) {
        fprintf(stderr, "dslcap: --list-ids unavailable: libusb backend has no "
                "audited descriptor-only enumeration path; USB was not initialized\n");
        printf("{\"devices\":[]}\n");
        return 1;
    }

    libusb_context *ctx = NULL;
    libusb_device **list = NULL;
    int rc = 0, first = 1;
    if (libusb_init(&ctx) != 0) {
        fprintf(stderr, "dslcap: --list-ids: cannot initialize libusb\n");
        printf("{\"devices\":[]}\n");
        return 1;
    }
    ssize_t count = libusb_get_device_list(ctx, &list);
    printf("{\"devices\":[");
    if (count < 0) {
        fprintf(stderr, "dslcap: --list-ids: cannot enumerate USB devices\n");
        rc = 1;
        goto done;
    }
    for (ssize_t i = 0; i < count; i++) {
        struct libusb_device_descriptor desc;
        if (libusb_get_device_descriptor(list[i], &desc) != 0) {
            fprintf(stderr, "dslcap: --list-ids: unreadable device descriptor at "
                    "inventory index %zd; inventory is incomplete\n", i);
            rc = 1;
            continue;
        }
        const char *model = ids_model(desc.idVendor, desc.idProduct);
        if (!model) continue;
        uint8_t bus = libusb_get_bus_number(list[i]), ports[7];
        int depth = libusb_get_port_numbers(list[i], ports, sizeof ports);
        char path[40], *location = NULL;
        if (bus && depth > 0 && depth <= (int)sizeof ports) {
            size_t pos = (size_t)snprintf(path, sizeof path, "usb-%u-", bus);
            location = path;
            for (int j = 0; j < depth; j++) {
                if (!ports[j]) { location = NULL; break; }
                pos += (size_t)snprintf(path + pos, sizeof path - pos,
                                       "%s%u", j ? "." : "", ports[j]);
            }
        }
        if (!location) {
            fprintf(stderr, "dslcap: --list-ids: location unavailable for "
                    "%04x:%04x; identity is incomplete\n", desc.idVendor, desc.idProduct);
            rc = 1;
        }
        char *serial = desc.iSerialNumber ? ids_serial(list[i], desc.iSerialNumber) : NULL;
        if (desc.iSerialNumber && !serial) {
            fprintf(stderr, "dslcap: --list-ids: serial descriptor index %u unreadable "
                    "for %04x:%04x at %s (access, detach or malformed descriptor); "
                    "identity is incomplete\n", desc.iSerialNumber, desc.idVendor,
                    desc.idProduct, location ? location : "unknown location");
            rc = 1;
        }
        printf("%s{\"vid\":%u,\"pid\":%u,\"model\":", first ? "" : ",",
               desc.idVendor, desc.idProduct);
        first = 0;
        ids_json_string(model);
        printf(",\"location\":"); ids_json_string(location);
        printf(",\"serial\":"); ids_json_string(serial);
        /* These VID/PIDs serve both pre-firmware and runtime devices. No
         * requested descriptor here proves firmware/FPGA/claim readiness. */
        printf(",\"state\":\"unknown\"}");
        g_free(serial);
    }
done:
    printf("]}\n");
    if (list) libusb_free_device_list(list, 1);
    libusb_exit(ctx);
    return rc;
}
