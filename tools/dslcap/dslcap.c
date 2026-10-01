/*
 * dslcap: headless DSLogic capture on libsigrok4DSL (GPL-3.0, as DSView).
 *
 *   dslcap --list
 *   dslcap --channels 0,1 --samplerate 10000000 --samples 1000000
 *          [--vth 1.6] [--mode buffer|stream] [--trigger CH:R|F|C|1|0]
 *          [--trigpos PERCENT] [--timeout SEC] --out /path/base
 *
 * Writes <base>.bin: for each enabled channel in ascending order, the
 * channel's samples packed LSB-first, ceil(samples/64)*8 bytes per channel.
 * Prints one JSON object describing the capture on stdout.
 */
#include <glib.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "libsigrok.h"

static volatile int g_done = 0, g_err = 0;
static FILE *g_raw = NULL;               /* disk spool of logic payload */
static uint64_t g_raw_bytes = 0;
static int g_io_error = 0;
static int g_format = -1;
static long long g_trig_pos = -1;
static int g_split_seen = 0;

static void on_event(int ev)
{
    if (ev == DS_EV_COLLECT_TASK_END || ev == DS_EV_DEVICE_STOPPED)
        g_done = 1;
    if (ev == DS_EV_COLLECT_TASK_END_BY_ERROR || ev == DS_EV_COLLECT_TASK_END_BY_DETACHED) {
        g_err = ev;
        g_done = 1;
    }
}

static void on_data(const struct sr_dev_inst *sdi, const struct sr_datafeed_packet *p)
{
    (void)sdi;
    if (p->type == SR_DF_LOGIC) {
        const struct sr_datafeed_logic *l = p->payload;
        g_format = l->format;
        if (l->format == LA_SPLIT_DATA)
            g_split_seen = 1;
        if (!g_io_error) {
            if (fwrite(l->data, 1, l->length, g_raw) != l->length) {
                g_io_error = 1;
                g_done = 1;
            } else {
                g_raw_bytes += l->length;
            }
        }
    } else if (p->type == SR_DF_TRIGGER && p->payload) {
        const struct ds_trigger_pos *t = p->payload;
        g_trig_pos = t->real_pos;
    } else if (p->type == SR_DF_END) {
        g_done = 1;
    }
}

static void json_str(const char *s)
{
    putchar('"');
    for (; *s; s++) {
        if ((unsigned char)*s < 0x20)
            printf("\\u%04x", (unsigned char)*s);
        else {
            if (*s == '"' || *s == '\\') putchar('\\');
            putchar(*s);
        }
    }
    putchar('"');
}

static int pick_device(int list_only)
{
    struct ds_device_base_info *list = NULL;
    int count = 0, pick = -1;
    ds_get_device_list(&list, &count);
    if (list_only) printf("{\"devices\":[");
    for (int i = 0; i < count; i++) {
        if (list_only) {
            printf("%s", i ? "," : "");
            json_str(list[i].name);
        }
        if (pick < 0 && strstr(list[i].name, "DSLogic"))
            pick = i;
    }
    if (list_only) printf("]}\n");
    if (pick >= 0 && !list_only) {
        if (ds_active_device(list[pick].handle) != SR_OK)
            pick = -2;
    }
    g_free(list);
    return pick;
}

static int set_u64(int key, uint64_t v) { return ds_set_actived_device_config(NULL, NULL, key, g_variant_new_uint64(v)); }

/* Keep the channel-major file layout, using fixed-size conversion buffers. */
static int write_output(const char *path, int nch, uint64_t per_ch, uint64_t got)
{
    uint64_t raw[4096], channel[4096];
    if (fflush(g_raw) || fseeko(g_raw, 0, SEEK_SET)) return -1;
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    int failed = 0;
    for (uint64_t k = 0; k < per_ch && !failed;) {
        size_t count = MIN(per_ch - k, G_N_ELEMENTS(raw) / nch);
        if (fread(raw, sizeof(uint64_t) * nch, count, g_raw) != count) {
            failed = 1;
            break;
        }
        for (int c = 0; c < nch; c++) {
            for (size_t i = 0; i < count; i++) channel[i] = raw[i * nch + c];
            /* Clear samples past the requested count in the last word. */
            if (k + count == per_ch && got % 64)
                channel[count - 1] &= (1ULL << (got % 64)) - 1;
            if (fseeko(f, (off_t)(((uint64_t)c * per_ch + k) * 8), SEEK_SET) ||
                fwrite(channel, 8, count, f) != count) {
                failed = 1;
                break;
            }
        }
        k += count;
    }
    if (fclose(f)) failed = 1;
    return failed ? -1 : 0;
}

static int set_samplerate(uint64_t rate)
{
    GVariant *dict = NULL;
    if (ds_get_actived_device_config_list(NULL, SR_CONF_SAMPLERATE, &dict) != SR_OK || !dict)
        return SR_ERR;
    GVariant *rates = g_variant_lookup_value(dict, "samplerates", G_VARIANT_TYPE("at"));
    g_variant_unref(dict);
    if (!rates)
        return SR_ERR;

    gsize count;
    const uint64_t *values = g_variant_get_fixed_array(rates, &count, sizeof(uint64_t));
    int supported = 0;
    for (gsize i = 0; i < count; i++)
        if (values[i] == rate) { supported = 1; break; }
    g_variant_unref(rates);

    /* The driver stores arbitrary rates but rounds the hardware divider. */
    return supported ? set_u64(SR_CONF_SAMPLERATE, rate) : SR_ERR_ARG;
}

static int select_channel_mode(uint64_t rate)
{
    GVariant *gv = NULL;
    if (ds_get_actived_device_config_list(NULL, SR_CONF_CHANNEL_MODE, &gv) != SR_OK || !gv)
        return SR_ERR;
    const struct sr_list_item *modes = (const struct sr_list_item *)g_variant_get_uint64(gv);
    g_variant_unref(gv);

    /* The driver lists modes from most channels to fewest. */
    for (; modes && modes->id >= 0; modes++) {
        if (ds_set_actived_device_config(NULL, NULL, SR_CONF_CHANNEL_MODE,
                g_variant_new_int16(modes->id)) != SR_OK)
            return SR_ERR;
        gv = NULL;
        if (ds_get_actived_device_config_list(NULL, SR_CONF_SAMPLERATE, &gv) != SR_OK || !gv)
            return SR_ERR;
        GVariant *rates = g_variant_lookup_value(gv, "samplerates", G_VARIANT_TYPE("at"));
        g_variant_unref(gv);
        if (!rates) return SR_ERR;
        gsize count = 0;
        const uint64_t *values = g_variant_get_fixed_array(rates, &count, sizeof(uint64_t));
        int found = 0;
        for (gsize i = 0; i < count; i++)
            if (values[i] == rate) found = 1;
        g_variant_unref(rates);
        if (found) return SR_OK;
    }
    return SR_ERR;
}

int main(int argc, char **argv)
{
    const char *res = getenv("DSLCAP_RES");
    const char *out = NULL, *chans = "0", *mode = "buffer", *trig = NULL;
    uint64_t rate = 10000000, samples = 1000000;
    double vth = 1.6, timeout = 30;
    int trigpos = 10, list_only = 0;

    for (int i = 1; i < argc; i++) {
        const char *a = argv[i], *v = (i + 1 < argc) ? argv[i + 1] : NULL;
        if (!strcmp(a, "--list")) list_only = 1;
        else if (!strcmp(a, "--res") && v) { res = v; i++; }
        else if (!strcmp(a, "--out") && v) { out = v; i++; }
        else if (!strcmp(a, "--channels") && v) { chans = v; i++; }
        else if (!strcmp(a, "--samplerate") && v) { rate = strtoull(v, NULL, 10); i++; }
        else if (!strcmp(a, "--samples") && v) { samples = strtoull(v, NULL, 10); i++; }
        else if (!strcmp(a, "--vth") && v) { vth = atof(v); i++; }
        else if (!strcmp(a, "--mode") && v) { mode = v; i++; }
        else if (!strcmp(a, "--trigger") && v) { trig = v; i++; }
        else if (!strcmp(a, "--trigpos") && v) { trigpos = atoi(v); i++; }
        else if (!strcmp(a, "--timeout") && v) { timeout = atof(v); i++; }
        else { fprintf(stderr, "unknown argument: %s\n", a); return 2; }
    }
    if (!res) res = "/Applications/DSView Native.app/Contents/MacOS/res";
    if (!list_only && !out) { fprintf(stderr, "--out is required\n"); return 2; }

    int enabled[64], nch = 0;
    if (!list_only) {
        char *dup = g_strdup(chans), *tok, *save = NULL;
        for (tok = strtok_r(dup, ",", &save); tok; tok = strtok_r(NULL, ",", &save)) {
            int c = atoi(tok);
            for (int i = 0; i < nch; i++) {
                if (enabled[i] == c) {
                    printf("{\"error\":\"duplicate channel\",\"channel\":%d}\n", c);
                    g_free(dup);
                    return 2;
                }
            }
            if (nch == 64) {
                printf("{\"error\":\"too many channels\"}\n");
                g_free(dup);
                return 2;
            }
            enabled[nch++] = c;
        }
        g_free(dup);
    }

    uint64_t hw_samples = samples;
    if (strcmp(mode, "stream")) {
        /* Buffer delivery is aligned to 1024 samples in the driver. */
        if (samples > UINT64_MAX - SAMPLES_ALIGN) { fprintf(stderr, "sample limit too large\n"); return 2; }
        hw_samples = (samples + SAMPLES_ALIGN) & ~SAMPLES_ALIGN;
    }

    ds_log_level(1);
    ds_set_firmware_resource_dir(res);
    ds_set_event_callback(on_event);
    ds_set_datafeed_callback(on_data);
    if (ds_lib_init() != SR_OK) { printf("{\"error\":\"lib init failed\"}\n"); return 1; }

    int dev = pick_device(list_only);
    if (list_only) { ds_lib_exit(); return 0; }
    if (dev < 0) {
        printf("{\"error\":\"%s\"}\n", dev == -1 ? "no DSLogic found" : "device activation failed");
        ds_lib_exit();
        return 1;
    }

    struct ds_device_full_info info;
    memset(&info, 0, sizeof info);
    ds_get_actived_device_info(&info);

    /* Mode first: it changes the channel mode and the allowed rates. */
    if (ds_set_actived_device_config(NULL, NULL, SR_CONF_OPERATION_MODE,
            g_variant_new_int16(strcmp(mode, "stream") ? LO_OP_BUFFER : LO_OP_STREAM)) != SR_OK ||
            select_channel_mode(rate) != SR_OK) {
        printf("{\"error\":\"samplerate unavailable in operation mode\",\"samplerate\":%llu}\n",
               (unsigned long long)rate);
        ds_lib_exit();
        return 2;
    }
    GVariant *gv = NULL;
    if (ds_get_actived_device_config(NULL, NULL, SR_CONF_VLD_CH_NUM, &gv) != SR_OK || !gv) {
        printf("{\"error\":\"cannot read channel limit\"}\n");
        ds_lib_exit();
        return 1;
    }
    int max_channels = g_variant_get_int16(gv);
    g_variant_unref(gv);
    gv = NULL;

    for (GSList *l = ds_get_actived_device_channels(); l; l = l->next)
        ds_enable_device_channel(l->data, FALSE);
    for (int i = 0; i < nch; i++) {
        /* The selected channel mode limits which channels exist at this rate. */
        if (i >= max_channels || ds_enable_device_channel_index(enabled[i], TRUE) != SR_OK) {
            printf("{\"error\":\"channel/rate combination unavailable\",\"channel\":%d,\"samplerate\":%llu,\"max_channels\":%d}\n",
                   enabled[i], (unsigned long long)rate, max_channels);
            ds_lib_exit();
            return 2;
        }
    }

    ds_set_actived_device_config(NULL, NULL, SR_CONF_VTH, g_variant_new_double(vth));
    int rate_rc = set_samplerate(rate);
    if (rate_rc != SR_OK) {
        printf("{\"error\":\"%s\",\"samplerate\":%llu}\n",
            rate_rc == SR_ERR_ARG ? "unsupported samplerate" : "samplerate configuration failed",
            (unsigned long long)rate);
        ds_lib_exit();
        return 1;
    }
    set_u64(SR_CONF_LIMIT_SAMPLES, hw_samples);

    nch = 0;
    for (GSList *l = ds_get_actived_device_channels(); l; l = l->next) {
        const struct sr_channel *ch = l->data;
        if (!ch->enabled) continue;
        if (nch == 64) {
            printf("{\"error\":\"too many enabled channels\"}\n");
            ds_lib_exit();
            return 1;
        }
        enabled[nch++] = ch->index;
    }
    /* ascending order, as the device sends them */
    for (int i = 0; i < nch; i++) for (int j = i + 1; j < nch; j++)
        if (enabled[j] < enabled[i]) { int t = enabled[i]; enabled[i] = enabled[j]; enabled[j] = t; }

    ds_trigger_reset();
    ds_trigger_set_mode(SIMPLE_TRIGGER);
    ds_trigger_set_pos((uint16_t)trigpos);
    if (trig) {
        char *end;
        errno = 0;
        long tch = strtol(trig, &end, 10);
        int trigger_enabled = 0;
        if (!errno && end != trig && (*end == ':' || *end == '\0') &&
            tch >= 0 && tch < MaxTriggerProbes) {
            for (int i = 0; i < nch; i++)
                if (enabled[i] == tch) trigger_enabled = 1;
        }
        if (!trigger_enabled) {
            printf("{\"error\":\"trigger channel must be an enabled device channel\"}\n");
            ds_lib_exit();
            return 2;
        }
        const char *colon = strchr(trig, ':');
        char t0 = colon ? colon[1] : 'R';
        ds_trigger_probe_set((uint16_t)tch, (unsigned char)t0, 'X');
        ds_trigger_set_en(TRUE);
    } else {
        ds_trigger_set_en(FALSE);
    }

    uint64_t act_rate = rate;
    if (ds_get_actived_device_config(NULL, NULL, SR_CONF_SAMPLERATE, &gv) == SR_OK && gv) { act_rate = g_variant_get_uint64(gv); g_variant_unref(gv); gv = NULL; }

    char *spool = g_strdup_printf("%s.raw-XXXXXX", out);
    int fd = g_mkstemp(spool);
    if (fd >= 0) {
        g_raw = fdopen(fd, "w+b");
        if (!g_raw) close(fd);
        unlink(spool);
    }
    g_free(spool);
    if (!g_raw) { printf("{\"error\":\"cannot create capture spool\"}\n"); ds_lib_exit(); return 1; }
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    int rc = ds_start_collect();
    if (rc != SR_OK) {
        printf("{\"error\":\"start failed\",\"code\":%d}\n", rc);
        fclose(g_raw);
        ds_lib_exit();
        return 1;
    }
    double waited = 0;
    while (!g_done && waited < timeout) { usleep(20000); waited += 0.02; }
    int timed_out = !g_done;
    if (ds_is_collecting()) ds_stop_collect();
    clock_gettime(CLOCK_MONOTONIC, &t1);

    /* De-interleave LA_CROSS_DATA: 64-sample words rotate through channels. */
    uint64_t words = g_raw_bytes / 8;
    uint64_t per_ch = nch ? words / nch : 0;
    uint64_t got = per_ch * 64;
    if (got > samples) got = samples;
    per_ch = (got + 63) / 64;
    char path[1024];
    snprintf(path, sizeof path, "%s.bin", out);
    if (g_io_error || write_output(path, nch, per_ch, got)) {
        printf("{\"error\":\"cannot write capture data\"}\n");
        fclose(g_raw);
        ds_lib_exit();
        return 1;
    }

    double secs = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
    printf("{\"device\":");
    json_str(info.name);
    printf(",\"samplerate\":%llu,\"samples_requested\":%llu,\"samples\":%llu,\"words_per_channel\":%llu,"
           "\"channels\":[", (unsigned long long)act_rate, (unsigned long long)samples,
           (unsigned long long)got, (unsigned long long)per_ch);
    for (int i = 0; i < nch; i++) printf("%s%d", i ? "," : "", enabled[i]);
    printf("],\"vth\":%.3f,\"mode\":", vth);
    json_str(mode);
    printf(",\"format\":\"%s\",\"trigger\":", g_format == LA_SPLIT_DATA || g_split_seen ? "split" : "cross");
    if (trig) json_str(trig); else printf("null");
    printf(",\"trigger_pos\":%lld,\"timed_out\":%s,\"lib_error_event\":%d,\"elapsed_s\":%.3f,\"bin\":",
           g_trig_pos, timed_out ? "true" : "false", g_err, secs);
    json_str(path);
    printf("}\n");

    fclose(g_raw);
    ds_release_actived_device();
    ds_lib_exit();
    return (timed_out || g_err) ? 3 : 0;
}
