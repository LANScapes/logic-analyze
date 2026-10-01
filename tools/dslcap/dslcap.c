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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "libsigrok.h"

static volatile int g_done = 0, g_err = 0;
static GByteArray *g_raw = NULL;          /* concatenated logic payload */
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
        g_byte_array_append(g_raw, l->data, (guint)l->length);
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
        if (*s == '"' || *s == '\\') putchar('\\');
        if ((unsigned char)*s >= 0x20) putchar(*s);
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
    ds_set_actived_device_config(NULL, NULL, SR_CONF_OPERATION_MODE,
        g_variant_new_int16(strcmp(mode, "stream") ? LO_OP_BUFFER : LO_OP_STREAM));

    for (GSList *l = ds_get_actived_device_channels(); l; l = l->next)
        ds_enable_device_channel(l->data, FALSE);
    for (int i = 0; i < nch; i++)
        ds_enable_device_channel_index(enabled[i], TRUE);

    ds_set_actived_device_config(NULL, NULL, SR_CONF_VTH, g_variant_new_double(vth));
    set_u64(SR_CONF_SAMPLERATE, rate);
    set_u64(SR_CONF_LIMIT_SAMPLES, samples);

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
        int tch = atoi(trig);
        const char *colon = strchr(trig, ':');
        char t0 = colon ? colon[1] : 'R';
        ds_trigger_probe_set((uint16_t)tch, (unsigned char)t0, 'X');
        ds_trigger_set_en(TRUE);
    } else {
        ds_trigger_set_en(FALSE);
    }

    GVariant *gv = NULL;
    uint64_t act_rate = rate, act_samples = samples;
    if (ds_get_actived_device_config(NULL, NULL, SR_CONF_SAMPLERATE, &gv) == SR_OK && gv) { act_rate = g_variant_get_uint64(gv); g_variant_unref(gv); gv = NULL; }
    if (ds_get_actived_device_config(NULL, NULL, SR_CONF_LIMIT_SAMPLES, &gv) == SR_OK && gv) { act_samples = g_variant_get_uint64(gv); g_variant_unref(gv); gv = NULL; }

    g_raw = g_byte_array_new();
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    int rc = ds_start_collect();
    if (rc != SR_OK) {
        printf("{\"error\":\"start failed\",\"code\":%d}\n", rc);
        ds_lib_exit();
        return 1;
    }
    double waited = 0;
    while (!g_done && waited < timeout) { usleep(20000); waited += 0.02; }
    int timed_out = !g_done;
    if (ds_is_collecting()) ds_stop_collect();
    clock_gettime(CLOCK_MONOTONIC, &t1);

    /* De-interleave LA_CROSS_DATA: 64-sample words rotate through channels. */
    uint64_t words = g_raw->len / 8;
    uint64_t per_ch = nch ? words / nch : 0;
    char path[1024];
    snprintf(path, sizeof path, "%s.bin", out);
    FILE *f = fopen(path, "wb");
    if (!f) { printf("{\"error\":\"cannot write output\"}\n"); ds_lib_exit(); return 1; }
    const uint64_t *w = (const uint64_t *)g_raw->data;
    for (int c = 0; c < nch; c++)
        for (uint64_t k = 0; k < per_ch; k++)
            fwrite(&w[k * nch + c], 8, 1, f);
    fclose(f);

    uint64_t got = per_ch * 64;
    if (got > act_samples) got = act_samples;
    double secs = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
    printf("{\"device\":");
    json_str(info.name);
    printf(",\"samplerate\":%llu,\"samples_requested\":%llu,\"samples\":%llu,\"words_per_channel\":%llu,"
           "\"channels\":[", (unsigned long long)act_rate, (unsigned long long)act_samples,
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

    g_byte_array_free(g_raw, TRUE);
    ds_release_actived_device();
    ds_lib_exit();
    return (timed_out || g_err) ? 3 : 0;
}
