/* Hardware-free regression of the actual lib_main.c collection lifecycle.
 * Only GLib and pthreads are linked; session/driver work is mocked, and neither
 * ds_lib_init() nor USB discovery, hotplug, or real acquisition is called.
 * Build/run commands are in README-collect.md. */
#undef NDEBUG
#include <assert.h>
#include <signal.h>
#include "../libsigrok-internal.h"
#include "../log.h"

/* Keep unrelated library entry points removable by the linker. */
#undef sr_info
#undef sr_err
#undef sr_detail
#define sr_info(...) ((void)0)
#define sr_err(...) ((void)0)
#define sr_detail(...) ((void)0)

static GThread *collect_thread_new(const char *, GThreadFunc, gpointer);
static gpointer collect_thread_join(GThread *);
#define g_thread_new collect_thread_new
#define g_thread_join collect_thread_join
#include "../lib_main.c"
#undef g_thread_new
#undef g_thread_join

static GMutex mutex;
static GCond changed;
static int sessions, starts, ends, joins, threads, error_event;
static int allow_acquisition, fail_acquisition, block_run, stopped;
static int restart_in_callback, hold_callback, callback_entered, release_callback;
static int callback_active, delay_publication;
static struct sr_session fake_session;

static void wait_for(int *value, int target)
{
    gint64 deadline = g_get_monotonic_time() + 3 * G_TIME_SPAN_SECOND;
    /* Caller holds mutex. Each wait, as well as the whole test, is bounded. */
    while (*value < target)
        assert(g_cond_wait_until(&changed, &mutex, deadline));
}

static GThread *collect_thread_new(const char *name, GThreadFunc fn, gpointer arg)
{
    GThread *thread = g_thread_new(name, fn, arg);
    g_mutex_lock(&mutex);
    threads++;
    /* Exercise completion before the creator publishes collect_thread. */
    if (delay_publication)
        wait_for(&ends, 1);
    g_mutex_unlock(&mutex);
    return thread;
}

static gpointer collect_thread_join(GThread *thread)
{
    /* Fail promptly on the old bug, rather than depending on the platform's
     * fatal diagnostic or hanging on a pthread self-join. */
    assert(thread != g_thread_self());
    g_mutex_lock(&mutex);
    joins++;
    g_cond_broadcast(&changed);
    g_mutex_unlock(&mutex);
    /* All valid joins still use the real GLib implementation. */
    return g_thread_join(thread);
}

struct sr_session *sr_session_new(void)
{
    g_mutex_lock(&mutex);
    sessions++;
    g_cond_broadcast(&changed);
    g_mutex_unlock(&mutex);
    return &fake_session;
}

int sr_session_run(void)
{
    g_mutex_lock(&mutex);
    while (block_run && !stopped)
        wait_for(&stopped, 1);
    g_mutex_unlock(&mutex);
    return SR_OK;
}

int sr_session_stop(void)
{
    g_mutex_lock(&mutex);
    stopped = 1;
    g_cond_broadcast(&changed);
    g_mutex_unlock(&mutex);
    return SR_OK;
}

static int acquisition_start(struct sr_dev_inst *di, void *data)
{
    assert(di == lib_ctx.actived_device_instance && data == di);
    g_mutex_lock(&mutex);
    wait_for(&allow_acquisition, 1);
    assert(!callback_active);
    starts++;
    g_cond_broadcast(&changed);
    int ret = fail_acquisition ? SR_ERR : SR_OK;
    g_mutex_unlock(&mutex);
    return ret;
}

static void datafeed(const struct sr_dev_inst *di,
                     const struct sr_datafeed_packet *packet)
{
    (void)di;
    (void)packet;
    assert(!"mock acquisition must not feed real data");
}

static void event_callback(int event)
{
    if (event != DS_EV_COLLECT_TASK_END && event != DS_EV_COLLECT_TASK_END_BY_ERROR)
        return;
    g_mutex_lock(&mutex);
    callback_active = 1;
    error_event = event == DS_EV_COLLECT_TASK_END_BY_ERROR;
    int generation = sessions;
    g_mutex_unlock(&mutex);
    assert(!ds_is_collecting());
    if (restart_in_callback) {
        /* This used to join itself (or overlap if its handle wasn't published). */
        assert(ds_start_collect() == SR_ERR_CALL_STATUS);
        g_mutex_lock(&mutex);
        assert(sessions == generation);
        g_mutex_unlock(&mutex);
    }
    g_mutex_lock(&mutex);
    callback_entered++;
    g_cond_broadcast(&changed);
    if (hold_callback)
        wait_for(&release_callback, 1);
    callback_active = 0;
    ends++;
    g_cond_broadcast(&changed);
    g_mutex_unlock(&mutex);
}

static struct sr_dev_driver driver = { .dev_acquisition_start = acquisition_start };
static struct sr_channel channel = { .enabled = TRUE };
static struct sr_dev_inst device = { .driver = &driver, .status = SR_ST_ACTIVE };

static void setup(void)
{
    sessions = starts = ends = joins = threads = error_event = 0;
    allow_acquisition = fail_acquisition = block_run = stopped = 0;
    restart_in_callback = hold_callback = callback_entered = release_callback = 0;
    callback_active = delay_publication = 0;
    lib_ctx.collect_thread = NULL;
    lib_ctx.is_collecting = lib_ctx.is_stop_by_detached = 0;
    lib_ctx.actived_device_instance = &device;
    lib_ctx.event_callback = event_callback;
    lib_ctx.data_forward_callback = datafeed;
    device.channels = g_slist_append(NULL, &channel);
    assert(pthread_mutex_init(&lib_ctx.mutext, NULL) == 0);
}

static void allow_worker(void)
{
    g_mutex_lock(&mutex);
    allow_acquisition = 1;
    g_cond_broadcast(&changed);
    g_mutex_unlock(&mutex);
}

static void teardown(void)
{
    /* Reap the last natural completion without initializing the library. */
    if (lib_ctx.collect_thread) {
        g_mutex_lock(&mutex);
        wait_for(&ends, sessions);
        g_mutex_unlock(&mutex);
        collect_thread_join(lib_ctx.collect_thread);
        lib_ctx.collect_thread = NULL;
    }
    assert(joins == threads);
    assert(!ds_is_collecting());
    g_slist_free(device.channels);
    device.channels = NULL;
    assert(pthread_mutex_destroy(&lib_ctx.mutext) == 0);
}

static void callback_restart(void)
{
    setup();
    restart_in_callback = 1;
    assert(ds_start_collect() == SR_OK);
    allow_worker();
    teardown();
    assert(sessions == 1 && starts == 1 && ends == 1 && !error_event);
}

static void error_callback_restart(void)
{
    setup();
    restart_in_callback = fail_acquisition = 1;
    assert(ds_start_collect() == SR_OK);
    allow_worker();
    teardown();
    assert(sessions == 1 && starts == 1 && ends == 1 && error_event);
}

static void fast_callback_restart(void)
{
    setup();
    restart_in_callback = delay_publication = allow_acquisition = 1;
    assert(ds_start_collect() == SR_OK);
    teardown();
    assert(sessions == 1 && starts == 1 && ends == 1);
}

static gpointer queued_restart(gpointer unused)
{
    (void)unused;
    assert(ds_start_collect() == SR_OK);
    return NULL;
}

static void queued_restart_waits(void)
{
    setup();
    hold_callback = 1;
    assert(ds_start_collect() == SR_OK);
    allow_worker();
    g_mutex_lock(&mutex);
    wait_for(&callback_entered, 1);
    g_mutex_unlock(&mutex);
    GThread *caller = g_thread_new("queued_restart", queued_restart, NULL);
    g_mutex_lock(&mutex);
    wait_for(&joins, 1);
    /* Joining must precede session destruction/replacement, not just launch. */
    assert(callback_active && sessions == 1 && starts == 1);
    release_callback = 1;
    g_cond_broadcast(&changed);
    g_mutex_unlock(&mutex);
    g_thread_join(caller);
    teardown();
    assert(sessions == 2 && starts == 2 && ends == 2);
}

static void repeat_and_stop(void)
{
    setup();
    allow_worker();
    for (int run = 1; run <= 50; run++) {
        assert(ds_start_collect() == SR_OK);
        g_mutex_lock(&mutex);
        wait_for(&ends, run);
        g_mutex_unlock(&mutex);
    }
    block_run = 1;
    assert(ds_start_collect() == SR_OK);
    assert(ds_is_collecting());
    assert(ds_start_collect() == SR_ERR_CALL_STATUS);
    assert(ds_stop_collect() == SR_OK);
    assert(lib_ctx.collect_thread == NULL && !ds_is_collecting());
    assert(ds_stop_collect() == SR_ERR_CALL_STATUS);
    block_run = 0;
    assert(ds_start_collect() == SR_OK);
    teardown();
    assert(sessions == 52 && starts == 52 && ends == 52);
}

static void immediate_stop(void)
{
    setup();
    block_run = 1;
    allow_worker();
    assert(ds_start_collect() == SR_OK);
    /* May stop before the mocked acquisition/session loop has started. */
    assert(ds_stop_collect() == SR_OK);
    teardown();
    assert(stopped && sessions == 1 && starts == 1 && ends == 1);
}

int main(int argc, char **argv)
{
    alarm(15); /* Bound joins as well as the explicit condition waits. */
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/collect/callback-restart", callback_restart);
    g_test_add_func("/collect/error-callback-restart", error_callback_restart);
    g_test_add_func("/collect/fast-callback-restart", fast_callback_restart);
    g_test_add_func("/collect/queued-restart-waits", queued_restart_waits);
    g_test_add_func("/collect/repeat-and-stop", repeat_and_stop);
    g_test_add_func("/collect/immediate-stop", immediate_stop);
    return g_test_run();
}
