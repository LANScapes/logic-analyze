/*
 * capcore: the capture core shared by dslcap and the GUI's MCP captures
 * (GPL-3.0, as DSView). One request turns into the same device settings, the
 * same recording of the data feed, the same <base>.bin layout and the same
 * JSON record, whichever program runs it (doc/mcp-gui-protocol.md).
 *
 * dslcap.c includes capcore.c into its own translation unit (so its tests can
 * inject faults); the GUI compiles capcore.c on its own.
 */
#ifndef DSLCAP_CAPCORE_H
#define DSLCAP_CAPCORE_H

#include <stdint.h>
#include <stdio.h>
#include <glib.h>
#include "libsigrok.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CAP_MAX_CHANNELS 64

/* One capture request: dslcap's options, or the MCP request's fields. */
struct cap_request {
    int channels[CAP_MAX_CHANNELS];  /* as given; distinct */
    int nch;
    uint64_t rate, samples;
    double vth;                      /* ignored on devices without a threshold */
    int stream;                      /* 1 stream, 0 buffer, -1 the device's current mode */
    int trig_ch;                     /* -1: no trigger */
    char trig_type;                  /* R F C 1 0 */
    int trigpos;                     /* 0..100 */
};

/* What cap_apply() left on the active device. */
struct cap_setup {
    char device[150];
    uint64_t hw_samples;             /* the depth asked of the driver */
    uint64_t rate, limit;            /* read back */
    double vth;                      /* read back; NAN without a threshold */
    int enabled[CAP_MAX_CHANNELS];   /* ascending */
    int nch;
};

/* A refusal: dslcap's exit status (2 unavailable setting, 1 device error),
 * its one-line JSON result, and the bare message. */
struct cap_error {
    int rc;
    char json[512];
    char message[128];
};

/* Publication hooks (dslcap's parent watch); see capcore.c. */
struct cap_publish_hooks {
    int (*active)(void);
    void (*lock_checked)(void);      /* lock, then check the parent */
    void (*check)(void);             /* check the parent, lock held */
    void (*unlock)(void);
    char **tmp;                      /* the temporary name, while it exists */
    char **bin;                      /* the published name, until the result is delivered */
};

/* Range checks that need more than one field (trigger position vs depth).
 * Returns 0, or rc with e filled. */
int cap_check(const struct cap_request *r, struct cap_error *e);

/* A request without a mode (stream -1) takes the active device's operation
 * mode; call before cap_apply(). */
void cap_resolve_mode(struct cap_request *r);

/* Applies r to the active device: operation mode and channel mode, enabled
 * channels, internal clock without RLE or filter, threshold, rate, depth and
 * the trigger; reads back what the device holds. Returns 0, or rc with e. */
int cap_apply(const struct cap_request *r, struct cap_setup *s, struct cap_error *e);

/* The trigger part of cap_apply() alone (the GUI sets it again right before
 * the start, after its own trigger panel committed). Returns SR_OK or an error. */
int cap_set_trigger(const struct cap_request *r, struct cap_error *e);

/* Library callbacks while recording. Safe to call at any time. */
void cap_on_event(int ev);
void cap_on_data(const struct sr_dev_inst *sdi, const struct sr_datafeed_packet *p);

/* Starts recording the data feed into an anonymous spool next to out_base.
 * Returns 0, or -1 if the spool cannot be created. */
int cap_record_begin(const char *out_base);

/* Waits until the capture is done (data error or collect-task end) or the
 * monotonic deadline passes; returns nonzero if done. */
int cap_wait_done(gint64 deadline);

/* Waits for the collect task's own end event. */
int cap_wait_task_end(gint64 deadline);

/* How the run ended, as seen by the caller. */
struct cap_end {
    int timed_out;
    int stopped_by_user;             /* keep partial data (GUI Stop) */
    double secs;                     /* elapsed time */
};

/* Ends recording: writes <out_base>.bin (bin_name in the record) for a
 * complete capture, and the one-line JSON record to *report (a g_malloc'd
 * string the caller frees). Returns 0 (published), 3 (the capture failed;
 * the record has "error"), or 1 (the data could not be written). With
 * stopped_by_user, a short capture is published with "stopped_by_user":true;
 * *failure then names why nothing was published (or NULL). */
int cap_finish(const struct cap_request *r, const struct cap_setup *s, const char *out_base,
               const char *bin_name, const struct cap_end *end, char **report, const char **failure);

/* Closes the spool without writing anything (cap_finish does this itself). */
void cap_record_end(void);

/* Reads words k..k+count-1 of every channel into raw, interleaved: word i of
 * channel c at raw[i * nch + c]. Returns 0, or -1 on a read error. */
typedef int (*cap_read_fn)(void *ctx, uint64_t k, size_t count, int nch, uint64_t *raw);

/* Writes path in the .bin layout: each channel's per_ch words in turn, bits
 * past got cleared; through a temporary file and link(). Returns 0 or -1. */
int cap_write_bin(const char *path, int nch, uint64_t per_ch, uint64_t got, cap_read_fn read_words, void *ctx);

/* The JSON record's fields. */
struct cap_record {
    const char *device;
    uint64_t rate, samples, got, per_ch, limit;
    const int *channels;
    int nch;
    double vth;                      /* NAN: null */
    const char *mode, *trig;
    int format;
    long long trig_pos;
    int timed_out, err, pkt_error, overflow, stopped_by_user;
    double secs;
    const char *bin;
};

/* The one-line JSON record (with a newline), g_malloc'd; error, when set,
 * makes it a failure record. */
char *cap_format_record(const char *error, const struct cap_record *r);

/* Writes len bytes to path through a temporary file and link(), never
 * replacing an existing file. Returns 0 or -1. */
int cap_publish_file(const char *path, const char *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif
