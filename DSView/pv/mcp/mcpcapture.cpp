/*
 * This file is part of the Logic Analyze project (Mac App Store edition).
 *
 * Copyright (C) 2026 Lanscapes
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mcpcapture.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <glib.h>
#include <string.h>

#include "../deviceagent.h"
#include "../log.h"
#include "../sigsession.h"
#include "../toolbars/samplingbar.h"

namespace pv {
namespace mcp {

static const int kIndicatorHoldMs = 3000;      // MCP ● stays this long after a capture
static const qint64 kTaskEndGraceUs = 5 * G_TIME_SPAN_SECOND;

McpCapture::McpCapture(SigSession *session, toolbars::SamplingBar *bar, QObject *parent)
    : QObject(parent), _session(session), _bar(bar)
{
    _timeout.setSingleShot(true);
    connect(&_timeout, &QTimer::timeout, this, [this]() { stop(true); });
    _indicator_off.setSingleShot(true);
    connect(&_indicator_off, &QTimer::timeout, this, [this]() {
        if (!running())
            emit active_changed(false);
    });
    session->add_msg_listener(this);
}

void McpCapture::fail(qint64 id, const QString &code, const QString &message)
{
    dsv_info("MCP capture %lld: %s: %s", (long long)id, code.toUtf8().constData(), message.toUtf8().constData());
    emit failed(id, code, message);
}

// The selected analyzer, or the first one in the device list (dslcap's rule).
bool McpCapture::choose_device(QString &code, QString &message)
{
    DeviceAgent *dev = _session->get_device();
    if (dev->have_instance() && dev->is_hardware())
        return true;

    struct ds_device_base_info *array = NULL;
    int count = 0;
    ds_device_handle want = NULL_HANDLE;
    if (ds_get_device_list(&array, &count) == SR_OK && array != NULL) {
        for (int i = 0; i < count && want == NULL_HANDLE; i++)
            if (strstr(array[i].name, _want.toUtf8().constData()))
                want = array[i].handle;
    }
    g_free(array);

    if (want == NULL_HANDLE) {
        code = "no_device";
        message = "no analyzer is connected";
        return false;
    }
    if ((!dev->have_instance() || dev->handle() != want) && !_session->set_device(want)) {
        code = "no_device";
        message = "the analyzer could not be opened";
        return false;
    }
    return true;
}

void McpCapture::start(qint64 id, const QString &name, const CaptureRequest &req, const QString &staging)
{
    if (running() || _session->is_working() || _session->is_saving() || _session->is_doing_action()) {
        fail(id, "busy", "Logic Analyze is capturing or saving; try again when it is done");
        return;
    }
    QString code, message;
    if (!choose_device(code, message)) {
        fail(id, code, message);
        return;
    }
    DeviceAgent *dev = _session->get_device();
    if (dev->get_work_mode() != LOGIC) {
        fail(id, "unsupported", "the selected device is not in logic analyzer mode");
        return;
    }
    if (!QDir(staging).exists()) {
        fail(id, "failed", "the staging directory does not exist");
        return;
    }

    struct cap_request r;
    memset(&r, 0, sizeof r);
    for (int c : req.channels)
        r.channels[r.nch++] = c;
    r.rate = (uint64_t)req.samplerate_hz;
    r.samples = (uint64_t)req.samples;
    r.vth = req.threshold_v;
    r.stream = req.stream;
    r.trig_ch = req.trigger_channel;
    r.trig_type = req.trigger_edge;
    r.trigpos = req.trigger_position_percent;

    struct cap_error e;
    if (cap_check(&r, &e) || cap_apply(&r, &_setup, &e)) {
        fail(id, e.rc == 2 ? "unsupported" : "failed", e.message);
        return;
    }
    // Show the new settings as the device options dialog does, then apply the
    // request once more so the device holds exactly what dslcap would set.
    _session->broadcast_msg(DSV_MSG_DEVICE_OPTIONS_UPDATED);
    _bar->update_sample_rate_list();
    _bar->reload();
    if (cap_apply(&r, &_setup, &e)) {
        fail(id, e.rc == 2 ? "unsupported" : "failed", e.message);
        return;
    }
    _session->set_collect_mode(COLLECT_SINGLE);
    _session->set_capture_work_time(r.samples * 1000000000ULL / r.rate);

    _out_base = QDir(staging).filePath(name);
    if (cap_record_begin(_out_base.toUtf8().constData())) {
        fail(id, "failed", "cannot create capture spool");
        return;
    }
    _id = id;
    _name = name;
    _req = r;
    _own_stop = _timed_out = _cancelled = _stopped_by_user = false;
    _elapsed.start();

    _starting = true;
    bool ok = _session->start_capture(false);
    _starting = false;
    if (!ok) {
        cap_record_end();
        _id = -1;
        fail(id, "failed", "start failed");
        return;
    }
    _timeout.start((int)qMin<qint64>(req.timeout_ms, INT_MAX));
    _indicator_off.stop();
    emit active_changed(true);
    emit started(id);
}

void McpCapture::OnMessage(int msg)
{
    switch (msg) {
    case DSV_MSG_START_COLLECT_WORK_PREV:
        // The trigger panel has just committed the user's trigger; an MCP
        // capture uses the request's, set as dslcap sets it.
        if (_starting) {
            struct cap_error e;
            if (cap_set_trigger(&_req, &e) != SR_OK)
                dsv_err("MCP capture: %s", e.json);
        }
        break;
    case DSV_MSG_END_COLLECT_WORK_PREV:
        if (running() && !_own_stop)
            _stopped_by_user = true;
        break;
    case DSV_MSG_END_COLLECT_WORK:
        if (running())
            finish();
        break;
    default:
        break;
    }
}

void McpCapture::stop(bool timed_out)
{
    if (!running())
        return;
    _own_stop = true;
    _timed_out = _timed_out || timed_out;
    _session->stop_capture();
}

void McpCapture::cancel(qint64 id)
{
    if (id != _id)
        return;
    _cancelled = true;
    stop(false);
}

void McpCapture::abandon()
{
    if (!running())
        return;
    _timeout.stop();
    cap_record_end();
    _id = -1;
    _indicator_off.start(kIndicatorHoldMs);
}

void McpCapture::finish()
{
    qint64 id = _id;
    _id = -1;
    _timeout.stop();
    _indicator_off.start(kIndicatorHoldMs);

    if (_cancelled) {
        cap_record_end();
        fail(id, "stopped", "cancelled by the agent");
        return;
    }

    // The collect task's end event normally arrived before this message.
    cap_wait_task_end(g_get_monotonic_time() + kTaskEndGraceUs);
    struct cap_end end;
    end.timed_out = _timed_out;
    end.stopped_by_user = _stopped_by_user;
    end.secs = _elapsed.nsecsElapsed() / 1e9;

    QByteArray base = _out_base.toUtf8();
    QByteArray bin = (_name + ".bin").toUtf8();
    char *report = NULL;
    const char *failure = NULL;
    int rc = cap_finish(&_req, &_setup, base.constData(), bin.constData(), &end, &report, &failure);
    QByteArray text(report);
    g_free(report);

    if (rc != 0) {
        QString why = failure ? QString::fromUtf8(failure) : QString("capture failed");
        if (_stopped_by_user && why == "no samples captured")
            fail(id, "stopped", "stopped by the user before any data arrived");
        else
            fail(id, "failed", why);
        return;
    }
    // <name>.json last: its presence means both files are complete.
    if (cap_publish_file((base + ".json").constData(), text.constData(), (size_t)text.size())) {
        QFile::remove(_out_base + ".bin");
        fail(id, "failed", "cannot write capture data");
        return;
    }
    emit done(id, _name, QJsonDocument::fromJson(text).object());
}

} // namespace mcp
} // namespace pv
