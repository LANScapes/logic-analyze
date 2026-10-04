/*
 * capcore: the capture core shared by dslcap and the GUI's MCP captures
 * (GPL-3.0, as DSView). See capcore.h.
 */
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#include <glib.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "libsigrok.h"
#include "capcore.h"

/* The spool and the output may exceed 2 GiB. */
G_STATIC_ASSERT(sizeof(off_t) >= 8);

#define POLL_US 2000

/*
 * Publication hooks: dslcap's parent watch removes a half-published file when
 * its parent dies. dslcap defines CAP_PUBLISH_HOOKS before including this file;
 * the GUI has none.
 */
#ifdef CAP_PUBLISH_HOOKS
static const struct cap_publish_hooks *g_hooks = &CAP_PUBLISH_HOOKS;
#else
static const struct cap_publish_hooks *g_hooks = NULL;
#endif

/*
 * Capture state. The library delivers data and events on its own threads,
 * so the capture fields below are guarded by g_lock (a pthread mutex, as the library
 * itself uses).
 */
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
static int g_done = 0;                   /* main thread should stop waiting */
static int g_task_end = 0;               /* terminal collect-task event seen */
static int g_err = 0;                    /* DS_EV_COLLECT_TASK_END_BY_ERROR/_DETACHED */
static int g_data_end = 0;               /* SR_DF_END seen */
static int g_pkt_error = 0;              /* packet status or logic data error */
static int g_overflow = 0;               /* SR_DF_OVERFLOW seen */
static FILE *g_raw = NULL;               /* disk spool of logic payload */
static uint64_t g_raw_bytes = 0;
static int g_io_error = 0;
static int g_format = -1;
static long long g_trig_pos = -1;
static int g_split_seen = 0;

static void on_event(int ev)
{
    pthread_mutex_lock(&g_lock);
    /* SR_DF_END and DS_EV_DEVICE_STOPPED come before the final status;
     * only the collect-task end events say how the capture finished. */
    if (ev == DS_EV_COLLECT_TASK_END ||
            ev == DS_EV_COLLECT_TASK_END_BY_ERROR ||
            ev == DS_EV_COLLECT_TASK_END_BY_DETACHED) {
        if (ev != DS_EV_COLLECT_TASK_END)
            g_err = ev;
        g_task_end = 1;
        g_done = 1;
    }
    pthread_mutex_unlock(&g_lock);
}

static void on_data(const struct sr_dev_inst *sdi, const struct sr_datafeed_packet *p)
{
    (void)sdi;
    pthread_mutex_lock(&g_lock);
    if (p->status != SR_PKT_OK) {
        g_pkt_error = 1;
        g_done = 1;
        if (p->type == SR_DF_END)
            g_data_end = 1;
    } else if (p->type == SR_DF_LOGIC) {
        const struct sr_datafeed_logic *l = p->payload;
        if (l->data_error) {
            g_pkt_error = 1;
            g_done = 1;
        } else {
            g_format = l->format;
            if (l->format == LA_SPLIT_DATA)
                g_split_seen = 1;
            if (g_raw && !g_io_error) {
                if (fwrite(l->data, 1, l->length, g_raw) != l->length) {
                    g_io_error = 1;
                    g_done = 1;
                } else {
                    g_raw_bytes += l->length;
                }
            }
        }
    } else if (p->type == SR_DF_TRIGGER && p->payload) {
        const struct ds_trigger_pos *t = p->payload;
        g_trig_pos = t->real_pos;
    } else if (p->type == SR_DF_OVERFLOW) {
        g_overflow = 1;
        g_done = 1;
    } else if (p->type == SR_DF_END) {
        g_data_end = 1;
    }
    pthread_mutex_unlock(&g_lock);
}

void cap_on_event(int ev) { on_event(ev); }
void cap_on_data(const struct sr_dev_inst *sdi, const struct sr_datafeed_packet *p) { on_data(sdi, p); }

/* Wait until a callback sets *flag or the monotonic deadline passes. */
static int wait_flag(const int *flag, gint64 deadline)
{
    for (;;) {
        pthread_mutex_lock(&g_lock);
        int set = *flag;
        pthread_mutex_unlock(&g_lock);
        gint64 now = g_get_monotonic_time();
        if (set || now >= deadline)
            return set;
        g_usleep(MIN(POLL_US, deadline - now));
    }
}

int cap_wait_done(gint64 deadline) { return wait_flag(&g_done, deadline); }
int cap_wait_task_end(gint64 deadline) { return wait_flag(&g_task_end, deadline); }

static void json_str(GString *out, const char *s)
{
    g_string_append_c(out, '"');
    for (; *s; s++) {
        if ((unsigned char)*s < 0x20)
            g_string_append_printf(out, "\\u%04x", (unsigned char)*s);
        else {
            if (*s == '"' || *s == '\\') g_string_append_c(out, '\\');
            g_string_append_c(out, *s);
        }
    }
    g_string_append_c(out, '"');
}

/* ---------------------------------------------------------------- settings */

static void set_error(struct cap_error *e, int rc, const char *message, const char *json_fmt, ...)
    G_GNUC_PRINTF(4, 5);

/* json_fmt continues the object after "error":"<message>" (or is NULL). */
static void set_error(struct cap_error *e, int rc, const char *message, const char *json_fmt, ...)
{
    e->rc = rc;
    g_strlcpy(e->message, message, sizeof e->message);
    GString *s = g_string_new("{\"error\":");
    json_str(s, message);
    if (json_fmt) {
        va_list ap;
        va_start(ap, json_fmt);
        g_string_append_vprintf(s, json_fmt, ap);
        va_end(ap);
    }
    g_string_append_c(s, '}');
    g_strlcpy(e->json, s->str, sizeof e->json);
    g_string_free(s, TRUE);
}

static void config_error(struct cap_error *e, const char *setting, int code)
{
    set_error(e, 1, "device configuration failed", ",\"setting\":\"%s\",\"code\":%d", setting, code);
}

static int set_u64(int key, uint64_t v) { return ds_set_actived_device_config(NULL, NULL, key, g_variant_new_uint64(v)); }

/* Read back a configuration value of the expected type. */
static GVariant *get_config(int key, const GVariantType *type)
{
    GVariant *gv = NULL;
    if (ds_get_actived_device_config(NULL, NULL, key, &gv) != SR_OK || !gv)
        return NULL;
    if (!g_variant_is_of_type(gv, type)) {
        g_variant_unref(gv);
        return NULL;
    }
    return gv;
}

/* Whether the active device lists key among its SR_CONF_DEVICE_OPTIONS. */
static int device_has_option(int key)
{
    GVariant *gv = NULL;
    int found = 0;
    if (ds_get_actived_device_config_list(NULL, SR_CONF_DEVICE_OPTIONS, &gv) != SR_OK || !gv)
        return 0;
    if (g_variant_is_of_type(gv, G_VARIANT_TYPE("ai"))) {
        gsize n = 0;
        const gint32 *keys = g_variant_get_fixed_array(gv, &n, sizeof(gint32));
        for (gsize i = 0; i < n; i++)
            if (keys[i] == key) found = 1;
    }
    g_variant_unref(gv);
    return found;
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

/* The depth asked of the driver: buffer delivery is aligned to 1024 samples. */
static uint64_t hw_samples(const struct cap_request *r)
{
    return r->stream ? r->samples : (r->samples + SAMPLES_ALIGN) & ~SAMPLES_ALIGN;
}

int cap_set_trigger(const struct cap_request *r, struct cap_error *e)
{
    int rc;
    /* The driver places the trigger at a percentage of the depth asked of it,
     * which can be more than the samples kept: scale it by samples/depth, and
     * keep it before the last sample kept. */
    uint64_t hw = hw_samples(r);
    uint64_t pos = ((uint64_t)r->trigpos * r->samples * 2 + hw) / (2 * hw);
    if (pos > 0 && pos * hw >= r->samples * 100)
        pos--;
    if ((rc = ds_trigger_reset()) != SR_OK) { config_error(e, "trigger", rc); return rc; }
    if ((rc = ds_trigger_set_mode(SIMPLE_TRIGGER)) != SR_OK) { config_error(e, "trigger_mode", rc); return rc; }
    if ((rc = ds_trigger_set_pos((uint16_t)pos)) != SR_OK) { config_error(e, "trigpos", rc); return rc; }
    if (r->trig_ch >= 0 && (rc = ds_trigger_probe_set((uint16_t)r->trig_ch,
            (unsigned char)r->trig_type, 'X')) != SR_OK) {
        config_error(e, "trigger", rc);
        return rc;
    }
    if ((rc = ds_trigger_set_en(r->trig_ch >= 0 ? TRUE : FALSE)) != SR_OK) { config_error(e, "trigger_enable", rc); return rc; }
    return SR_OK;
}

int cap_check(const struct cap_request *r, struct cap_error *e)
{
    /* The driver converts trigpos% of the (aligned) sample limit to a 32-bit
     * trigger position before applying its depth limit, so keep it in range. */
    if (r->trigpos > 0 &&
        r->samples + SAMPLES_ALIGN > (uint64_t)UINT32_MAX * 100 / (uint64_t)r->trigpos) {
        set_error(e, 2, "--trigpos too large for --samples", ",\"samples\":%llu,\"trigpos\":%d",
                  (unsigned long long)r->samples, r->trigpos);
        return 2;
    }
    return 0;
}

void cap_resolve_mode(struct cap_request *r)
{
    if (r->stream >= 0)
        return;
    /* Devices without an operation mode (the demo device) count as buffer. */
    GVariant *gv = device_has_option(SR_CONF_OPERATION_MODE) ?
        get_config(SR_CONF_OPERATION_MODE, G_VARIANT_TYPE_INT16) : NULL;
    r->stream = gv && g_variant_get_int16(gv) == LO_OP_STREAM;
    if (gv) g_variant_unref(gv);
}

int cap_apply(const struct cap_request *r, struct cap_setup *s, struct cap_error *e)
{
    int rc;
    memset(s, 0, sizeof *s);

    struct ds_device_full_info info;
    memset(&info, 0, sizeof info);
    ds_get_actived_device_info(&info);
    info.name[sizeof info.name - 1] = '\0';
    g_strlcpy(s->device, info.name, sizeof s->device);

    s->hw_samples = hw_samples(r);

    /* Mode first: it changes the channel mode and the allowed rates. Devices
     * without an operation mode (the demo device) have one channel mode. */
    if (device_has_option(SR_CONF_OPERATION_MODE) &&
            (ds_set_actived_device_config(NULL, NULL, SR_CONF_OPERATION_MODE,
                g_variant_new_int16(r->stream ? LO_OP_STREAM : LO_OP_BUFFER)) != SR_OK ||
             select_channel_mode(r->rate) != SR_OK)) {
        set_error(e, 2, "samplerate unavailable in operation mode", ",\"samplerate\":%llu",
                  (unsigned long long)r->rate);
        return 2;
    }
    GVariant *gv = get_config(SR_CONF_VLD_CH_NUM, G_VARIANT_TYPE_INT16);
    if (!gv) {
        set_error(e, 1, "cannot read channel limit", NULL);
        return 1;
    }
    int max_channels = g_variant_get_int16(gv);
    g_variant_unref(gv);

    for (GSList *l = ds_get_actived_device_channels(); l; l = l->next)
        ds_enable_device_channel(l->data, FALSE);
    for (int i = 0; i < r->nch; i++) {
        /* The selected channel mode limits which channels exist at this rate. */
        if (i >= max_channels || ds_enable_device_channel_index(r->channels[i], TRUE) != SR_OK) {
            set_error(e, 2, "channel/rate combination unavailable",
                      ",\"channel\":%d,\"samplerate\":%llu,\"max_channels\":%d",
                      r->channels[i], (unsigned long long)r->rate, max_channels);
            return 2;
        }
    }

    /* Buffer mode holds the capture in the device memory: refuse more samples
     * than it holds for this channel count (the app's own depth limit). */
    gv = r->stream ? NULL : get_config(SR_CONF_HW_DEPTH, G_VARIANT_TYPE_UINT64);
    if (gv) {
        uint64_t depth = g_variant_get_uint64(gv);
        g_variant_unref(gv);
        if (s->hw_samples > depth) {
            set_error(e, 2, "samples exceed the device memory for this channel count in buffer mode",
                      ",\"samples\":%llu,\"max_samples\":%llu,\"channels\":%d",
                      (unsigned long long)r->samples, (unsigned long long)depth, r->nch);
            return 2;
        }
    }

    /* A plain capture: the internal clock, no run-length compression, no
     * input filter, whatever the GUI's device options held before. */
    if (device_has_option(SR_CONF_CLOCK_TYPE) &&
            (rc = ds_set_actived_device_config(NULL, NULL, SR_CONF_CLOCK_TYPE, g_variant_new_boolean(FALSE))) != SR_OK) {
        config_error(e, "clock", rc);
        return 1;
    }
    if (device_has_option(SR_CONF_RLE_SUPPORT) &&
            (rc = ds_set_actived_device_config(NULL, NULL, SR_CONF_RLE, g_variant_new_boolean(FALSE))) != SR_OK) {
        config_error(e, "rle", rc);
        return 1;
    }
    if (device_has_option(SR_CONF_FILTER) &&
            (rc = ds_set_actived_device_config(NULL, NULL, SR_CONF_FILTER, g_variant_new_int16(SR_FILTER_NONE))) != SR_OK) {
        config_error(e, "filter", rc);
        return 1;
    }

    /* Devices with a threshold voltage list SR_CONF_VTH; older ones only have a
     * 3.3 V / 5 V threshold setting, which is left as it is, and the requested
     * threshold is ignored (the record's vth is then null). */
    int has_vth = device_has_option(SR_CONF_VTH);
    if (has_vth && (rc = ds_set_actived_device_config(NULL, NULL, SR_CONF_VTH, g_variant_new_double(r->vth))) != SR_OK) {
        config_error(e, "vth", rc);
        return 1;
    }
    int rate_rc = set_samplerate(r->rate);
    if (rate_rc != SR_OK) {
        set_error(e, rate_rc == SR_ERR_ARG ? 2 : 1,
                  rate_rc == SR_ERR_ARG ? "unsupported samplerate" : "samplerate configuration failed",
                  ",\"samplerate\":%llu", (unsigned long long)r->rate);
        return e->rc;
    }
    if ((rc = set_u64(SR_CONF_LIMIT_SAMPLES, s->hw_samples)) != SR_OK) {
        config_error(e, "samples", rc);
        return 1;
    }

    for (GSList *l = ds_get_actived_device_channels(); l; l = l->next) {
        const struct sr_channel *ch = l->data;
        if (!ch->enabled) continue;
        if (s->nch == CAP_MAX_CHANNELS) {
            set_error(e, 1, "too many enabled channels", NULL);
            return 1;
        }
        s->enabled[s->nch++] = ch->index;
    }
    /* ascending order, as the device sends them */
    for (int i = 0; i < s->nch; i++) for (int j = i + 1; j < s->nch; j++)
        if (s->enabled[j] < s->enabled[i]) { int t = s->enabled[i]; s->enabled[i] = s->enabled[j]; s->enabled[j] = t; }

    if (r->trig_ch >= 0) {
        int trigger_enabled = 0;
        for (int i = 0; i < s->nch; i++)
            if (s->enabled[i] == r->trig_ch) trigger_enabled = 1;
        if (!trigger_enabled) {
            set_error(e, 2, "trigger channel must be an enabled device channel", NULL);
            return 2;
        }
    }
    if (cap_set_trigger(r, e) != SR_OK)
        return 1;

    /* Report what the device holds, not what was requested. */
    GVariant *rate_gv = get_config(SR_CONF_SAMPLERATE, G_VARIANT_TYPE_UINT64);
    GVariant *limit_gv = get_config(SR_CONF_LIMIT_SAMPLES, G_VARIANT_TYPE_UINT64);
    GVariant *vth_gv = has_vth ? get_config(SR_CONF_VTH, G_VARIANT_TYPE_DOUBLE) : NULL;
    s->rate = rate_gv ? g_variant_get_uint64(rate_gv) : 0;
    s->limit = limit_gv ? g_variant_get_uint64(limit_gv) : 0;
    s->vth = vth_gv ? g_variant_get_double(vth_gv) : NAN;
    const char *unread = !rate_gv ? "samplerate" : !limit_gv ? "samples" : has_vth && !vth_gv ? "vth" : NULL;
    if (rate_gv) g_variant_unref(rate_gv);
    if (limit_gv) g_variant_unref(limit_gv);
    if (vth_gv) g_variant_unref(vth_gv);
    if (unread || s->limit != s->hw_samples || (has_vth && !isfinite(s->vth))) {
        set_error(e, 1, "cannot verify device configuration", ",\"setting\":\"%s\"",
                  unread ? unread : s->limit != s->hw_samples ? "samples" : "vth");
        return 1;
    }
    return 0;
}

/* ---------------------------------------------------------------- recording */

int cap_record_begin(const char *out_base)
{
    pthread_mutex_lock(&g_lock);
    g_done = g_task_end = g_err = g_data_end = g_pkt_error = g_overflow = 0;
    g_raw_bytes = 0;
    g_io_error = 0;
    g_format = -1;
    g_trig_pos = -1;
    g_split_seen = 0;
    pthread_mutex_unlock(&g_lock);

    char *spool = g_strdup_printf("%s.raw-XXXXXX", out_base);
    if (g_hooks && g_hooks->active())
        g_hooks->lock_checked();
    int fd = g_mkstemp(spool);
    /* Anonymous before fdopen or collection, including the failure path. */
    if (fd >= 0) unlink(spool);
    if (g_hooks && g_hooks->active()) g_hooks->unlock();
    FILE *raw = NULL;
    if (fd >= 0) {
        raw = fdopen(fd, "w+b");
        if (!raw) close(fd);
    }
    g_free(spool);
    pthread_mutex_lock(&g_lock);
    g_raw = raw;
    pthread_mutex_unlock(&g_lock);
    return raw ? 0 : -1;
}

/* Keep the channel-major file layout, using fixed-size conversion buffers. */
int cap_write_bin(const char *path, int nch, uint64_t per_ch, uint64_t got, cap_read_fn read_words, void *ctx)
{
    uint64_t raw[4096], channel[4096];
    if (nch <= 0 || nch > (int)G_N_ELEMENTS(raw)) return -1;
    /* Every channel's last byte must be addressable as an off_t. */
    if (per_ch > (uint64_t)G_MAXINT64 / 8 / (uint64_t)nch) return -1;
    int hooked = g_hooks && g_hooks->active();
    /* Write to a temporary file and publish it only on success. */
    char *tmp = g_strdup_printf("%s.XXXXXX", path);
    if (hooked) {
        g_hooks->lock_checked();
        *g_hooks->tmp = tmp;
    }
    int fd = g_mkstemp(tmp);
    if (hooked) {
        if (fd < 0) *g_hooks->tmp = NULL;
        g_hooks->unlock();
    }
    if (fd < 0) { g_free(tmp); return -1; }
    FILE *f = fdopen(fd, "wb");
    if (!f) close(fd);
    int failed = !f;
    for (uint64_t k = 0; k < per_ch && !failed;) {
        size_t count = MIN(per_ch - k, G_N_ELEMENTS(raw) / nch);
        if (read_words(ctx, k, count, nch, raw)) {
            failed = 1;
            break;
        }
        for (int c = 0; c < nch; c++) {
            for (size_t i = 0; i < count; i++) channel[i] = raw[i * nch + c];
            /* Clear samples past the requested count in the last word. */
            if (k + count == per_ch && got % 64)
                channel[count - 1] &= (1ULL << (got % 64)) - 1;
            off_t offset = (off_t)(((uint64_t)c * per_ch + k) * 8);
            if (fseeko(f, offset, SEEK_SET) || fwrite(channel, 8, count, f) != count) {
                failed = 1;
                break;
            }
        }
        k += count;
    }
    if (f && fclose(f)) failed = 1;
    char *published = hooked ? g_strdup(path) : NULL;
    if (hooked)
        g_hooks->lock_checked();
    /* link() publishes atomically and, unlike rename(), refuses to replace an existing capture. */
    if (!failed && link(tmp, path)) failed = 1;
    if (!failed && hooked) {
        *g_hooks->bin = published;
        published = NULL;
    }
    if (hooked) g_hooks->check();
    unlink(tmp);  /* after link() the published name keeps the data; drop the temporary one either way */
    if (hooked) {
        *g_hooks->tmp = NULL;
        g_hooks->unlock();
    }
    g_free(published);
    g_free(tmp);
    return failed ? -1 : 0;
}

/* The spool, read in order: LA_CROSS_DATA's 64-sample words rotate through channels. */
static int spool_read(void *ctx, uint64_t k, size_t count, int nch, uint64_t *raw)
{
    (void)ctx; (void)k;
    return fread(raw, sizeof(uint64_t) * nch, count, g_raw) != count ? -1 : 0;
}

static int write_output(const char *path, int nch, uint64_t per_ch, uint64_t got)
{
    if (fflush(g_raw) || fseeko(g_raw, 0, SEEK_SET)) return -1;
    return cap_write_bin(path, nch, per_ch, got, spool_read, NULL);
}

/* Why a finished acquisition must not be published, or NULL if it may. */
static const char *capture_failure(int timed_out, int task_end, int err, int pkt_error,
                                   int overflow, uint64_t got, uint64_t samples)
{
    if (timed_out) return "capture timed out";
    if (!task_end) return "capture did not finish";
    if (err == DS_EV_COLLECT_TASK_END_BY_DETACHED) return "device detached during capture";
    if (err) return "capture ended with a device error";
    if (pkt_error) return "device reported a data error";
    if (overflow) return "device buffer overflow";
    if (got == 0) return "no samples captured";
    if (got < samples) return "capture ended before the requested sample count";
    return NULL;
}

/* One JSON object; error, when set, makes it a failure report without a file. */
char *cap_format_record(const char *error, const struct cap_record *r)
{
    GString *s = g_string_new("{");
    if (error) {
        g_string_append(s, "\"error\":");
        json_str(s, error);
        g_string_append(s, ",");
    }
    g_string_append(s, "\"device\":");
    json_str(s, r->device);
    g_string_append_printf(s, ",\"samplerate\":%llu,\"samples_requested\":%llu,\"samples\":%llu,\"words_per_channel\":%llu,"
           "\"channels\":[", (unsigned long long)r->rate, (unsigned long long)r->samples,
           (unsigned long long)r->got, (unsigned long long)r->per_ch);
    for (int i = 0; i < r->nch; i++) g_string_append_printf(s, "%s%d", i ? "," : "", r->channels[i]);
    /* vth is null on devices without a threshold voltage. */
    if (isfinite(r->vth)) g_string_append_printf(s, "],\"vth\":%.3f,\"mode\":", r->vth);
    else g_string_append(s, "],\"vth\":null,\"mode\":");
    json_str(s, r->mode);
    g_string_append_printf(s, ",\"format\":\"%s\",\"trigger\":", r->format == LA_SPLIT_DATA ? "split" : "cross");
    if (r->trig) json_str(s, r->trig); else g_string_append(s, "null");
    g_string_append_printf(s, ",\"trigger_pos\":%lld,\"timed_out\":%s,\"lib_error_event\":%d,\"elapsed_s\":%.3f,\"bin\":",
           r->trig_pos, r->timed_out ? "true" : "false", r->err, r->secs);
    if (r->bin) json_str(s, r->bin); else g_string_append(s, "null");
    g_string_append_printf(s, ",\"limit_samples\":%llu,\"packet_error\":%s,\"overflow\":%s",
           (unsigned long long)r->limit, r->pkt_error ? "true" : "false",
           r->overflow ? "true" : "false");
    if (r->stopped_by_user)
        g_string_append(s, ",\"stopped_by_user\":true");
    g_string_append(s, "}\n");
    return g_string_free(s, FALSE);
}

int cap_finish(const struct cap_request *req, const struct cap_setup *setup, const char *out_base,
               const char *bin_name, const struct cap_end *end, char **report, const char **failure_out)
{
    pthread_mutex_lock(&g_lock);
    int task_end = g_task_end, err = g_err, pkt_error = g_pkt_error, overflow = g_overflow;
    int io_error = g_io_error, format = g_split_seen ? LA_SPLIT_DATA : g_format;
    uint64_t raw_bytes = g_raw_bytes;
    long long trig_pos = g_trig_pos;
    pthread_mutex_unlock(&g_lock);

    /* The trigger as dslcap's --trigger takes it. */
    char trig[16];
    if (req->trig_ch >= 0) g_snprintf(trig, sizeof trig, "%d:%c", req->trig_ch, req->trig_type);

    /* De-interleave LA_CROSS_DATA: 64-sample words rotate through channels. */
    int nch = setup->nch;
    uint64_t words = raw_bytes / 8;
    uint64_t per_ch = nch ? words / nch : 0;
    uint64_t got = per_ch > req->samples / 64 ? req->samples : per_ch * 64;
    per_ch = got / 64 + (got % 64 != 0);

    struct cap_record r = {
        .device = setup->device, .rate = setup->rate, .samples = req->samples, .got = got,
        .per_ch = per_ch, .limit = setup->limit, .channels = setup->enabled, .nch = nch,
        .vth = setup->vth, .mode = req->stream ? "stream" : "buffer",
        .trig = req->trig_ch >= 0 ? trig : NULL, .format = format,
        .trig_pos = trig_pos, .timed_out = end->timed_out, .err = err,
        .pkt_error = pkt_error, .overflow = overflow,
        .secs = end->secs, .bin = NULL,
    };
    const char *failure = io_error ? NULL : capture_failure(end->timed_out, task_end, err,
            pkt_error, overflow, got, req->samples);
    /* A user's Stop keeps what arrived. */
    if (end->stopped_by_user && failure && got > 0 &&
            !strcmp(failure, "capture ended before the requested sample count")) {
        failure = NULL;
        r.stopped_by_user = 1;
    }
    char *path = g_strdup_printf("%s.bin", out_base);
    int exit_code = 0;
    if (io_error || (!failure && write_output(path, nch, per_ch, got))) {
        *report = g_strdup("{\"error\":\"cannot write capture data\"}\n");
        failure = "cannot write capture data";
        exit_code = 1;
    } else if (failure) {
        *report = cap_format_record(failure, &r);
        exit_code = 3;
    } else {
        if (g_hooks && g_hooks->active()) {
            g_hooks->lock_checked();
            g_hooks->unlock();
        }
        r.bin = bin_name;
        *report = cap_format_record(NULL, &r);
    }
    g_free(path);
    if (failure_out) *failure_out = failure;
    cap_record_end();
    return exit_code;
}

void cap_record_end(void)
{
    /* Late callbacks, if the task never reported its end, must not see a closed spool. */
    pthread_mutex_lock(&g_lock);
    FILE *raw = g_raw;
    g_raw = NULL;
    pthread_mutex_unlock(&g_lock);
    if (raw) fclose(raw);
}

int cap_publish_file(const char *path, const char *data, size_t len)
{
    char *tmp = g_strdup_printf("%s.XXXXXX", path);
    int fd = g_mkstemp(tmp);
    if (fd < 0) { g_free(tmp); return -1; }
    int failed = 0;
    for (size_t off = 0; off < len && !failed;) {
        ssize_t n = write(fd, data + off, len - off);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) failed = 1;
        else off += (size_t)n;
    }
    if (close(fd)) failed = 1;
    if (!failed && link(tmp, path)) failed = 1;
    unlink(tmp);
    g_free(tmp);
    return failed ? -1 : 0;
}
