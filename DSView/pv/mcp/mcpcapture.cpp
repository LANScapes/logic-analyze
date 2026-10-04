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

#include <libsigrokdecode.h>
#include "../data/decode/decoder.h"
#include "../data/decoderstack.h"
#include "../data/logicsnapshot.h"
#include "../deviceagent.h"
#include "../log.h"
#include "../sigsession.h"
#include "../toolbars/samplingbar.h"
#include "../view/decodetrace.h"
#include "../view/signal.h"
#include <QJsonArray>
#include <algorithm>
#include <cmath>

namespace pv {
namespace mcp {

static const qint64 kTaskEndGraceUs = 5 * G_TIME_SPAN_SECOND;

McpCapture::McpCapture(SigSession *session, toolbars::SamplingBar *bar, QObject *parent)
    : QObject(parent), _session(session), _bar(bar)
{
    _timeout.setSingleShot(true);
    connect(&_timeout, &QTimer::timeout, this, [this]() { stop(true); });
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
    if ((!dev->have_instance() || dev->handle() != want) && !_session->set_device(want, true)) {
        if (ds_get_last_error() == SR_ERR_DEVICE_IS_EXCLUSIVE) {
            code = "busy";
            message = "another program is using the analyzer";
        } else {
            code = "no_device";
            message = "the analyzer could not be opened";
        }
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
    cap_resolve_mode(&r);   // no mode: the user's buffer or stream setting
    r.trig_ch = req.trigger_channel;
    r.trig_type = req.trigger_edge;
    r.trigpos = req.trigger_position_percent;

    struct cap_error e;
    if (cap_check(&r, &e)) {
        fail(id, "unsupported", e.message);
        return;
    }
    save_settings();
    // Show the new settings as the device options dialog does, then apply the
    // request once more so the device holds exactly what dslcap would set. A
    // refused request can have changed some settings already: show them too.
    bool applied = cap_apply(&r, &_setup, &e) == 0;
    _session->broadcast_msg(DSV_MSG_DEVICE_OPTIONS_UPDATED);
    _bar->update_sample_rate_list();
    _bar->reload();
    if (!applied || cap_apply(&r, &_setup, &e)) {
        restore_settings();
        fail(id, e.rc == 2 ? "unsupported" : "failed", e.message);
        return;
    }
    _session->set_collect_mode(COLLECT_SINGLE);
    _session->set_capture_work_time(r.samples * 1000000000ULL / r.rate);

    _out_base = QDir(staging).filePath(name);
    if (cap_record_begin(_out_base.toUtf8().constData())) {
        restore_settings();
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
        restore_settings();
        fail(id, "failed", "start failed");
        return;
    }
    if (!running())   // it has already ended, and finish() has answered
        return;
    _timeout.start((int)qMin<qint64>(req.timeout_ms, INT_MAX));
    emit active_changed(true);
    emit started(id);
}

// The settings that an MCP capture changes and gives back afterwards: the
// capture mode, and the clock, RLE and input filter that capcore turns off.
void McpCapture::save_settings()
{
    DeviceAgent *dev = _session->get_device();
    _saved.valid = true;
    _saved.collect_mode = _session->get_collect_mode();
    _saved.has_clock = dev->get_config_bool(SR_CONF_CLOCK_TYPE, _saved.clock);
    _saved.has_rle = dev->get_config_bool(SR_CONF_RLE, _saved.rle);
    _saved.has_filter = dev->get_config_int16(SR_CONF_FILTER, _saved.filter);
}

void McpCapture::restore_settings()
{
    if (!_saved.valid)
        return;
    _saved.valid = false;
    DeviceAgent *dev = _session->get_device();
    if (_saved.has_clock)
        dev->set_config_bool(SR_CONF_CLOCK_TYPE, _saved.clock);
    if (_saved.has_rle)
        dev->set_config_bool(SR_CONF_RLE, _saved.rle);
    if (_saved.has_filter)
        dev->set_config_int16(SR_CONF_FILTER, _saved.filter);
    // Loop mode needs stream mode on an analyzer (as SamplingBar::reload).
    if (_saved.collect_mode != COLLECT_LOOP || dev->is_stream_mode() || !dev->is_hardware())
        _session->set_collect_mode((DEVICE_COLLECT_MODE)_saved.collect_mode);
    _bar->update_view_status();   // the Mode button's icon
}

void McpCapture::OnMessage(int msg)
{
    switch (msg) {
    case DSV_MSG_CURRENT_DEVICE_CHANGED:
        _mcp_on_screen = _stopped_on_screen = false;
        _saved.valid = false;   // they belong to the other device
        break;
    case DSV_MSG_START_COLLECT_WORK_PREV:
        _mcp_on_screen = _starting;
        _stopped_on_screen = false;
        // The trigger panel has just committed the user's trigger; an MCP
        // capture uses the request's, set as dslcap sets it.
        if (_starting) {
            struct cap_error e;
            if (cap_set_trigger(&_req, &e) != SR_OK)
                dsv_err("MCP capture: %s", e.json);
        }
        break;
    case DSV_MSG_END_COLLECT_WORK_PREV:
        // An explicit Stop (not this class's timeout or cancel).
        if (!running() || !_own_stop)
            _stopped_on_screen = true;
        if (running() && !_own_stop)
            _stopped_by_user = true;
        break;
    case DSV_MSG_END_COLLECT_WORK:
        if (running())
            finish();
        else
            restore_settings();   // after abandon(), when the capture ends
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
    emit active_changed(false);
}

void McpCapture::finish()
{
    qint64 id = _id;
    _id = -1;
    _timeout.stop();
    restore_settings();
    // The agent has the data: switching device or quitting does not ask to save it.
    _session->is_first_store_confirm();
    emit active_changed(false);

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

void McpCapture::user_stop()
{
    // As the app's Stop: the agent gets what arrived, with stopped_by_user.
    if (running())
        _session->stop_capture();
}

// "ch 0-3, 7, 25 MHz, 1M samples" (the template is translated by the caller).
static QString channel_list(const int *ch, int n)
{
    QStringList parts;
    for (int i = 0; i < n;) {
        int j = i;
        while (j + 1 < n && ch[j + 1] == ch[j] + 1)
            j++;
        parts << (j > i ? QString("%1–%2").arg(ch[i]).arg(ch[j]) : QString::number(ch[i]));
        i = j + 1;
    }
    return parts.join(", ");
}

static QString si(double v, const char *unit)
{
    const char *p[] = {"", "k", "M", "G"};
    int i = 0;
    while (v >= 1000 && i < 3) {
        v /= 1000;
        i++;
    }
    return QString("%1%2%3%4").arg(v, 0, 'g', 4).arg(*unit ? " " : "").arg(p[i]).arg(unit);
}

QStringList McpCapture::activity() const
{
    if (!running())
        return QStringList();
    int sorted[CAP_MAX_CHANNELS];
    int n = _req.nch;
    std::copy(_req.channels, _req.channels + n, sorted);
    std::sort(sorted, sorted + n);
    return {channel_list(sorted, n), si((double)_req.rate, "Hz"), si((double)_req.samples, "")};
}

// ---------------------------------------------------------------- current

namespace {

// The capture on screen, read through the LogicSnapshot's blocks.
struct OnScreen
{
    data::LogicSnapshot *snap;
    std::vector<int> chans;
    std::vector<uint64_t> starts;   // each block's first byte; one more entry at the end
};

uint8_t byte_at(OnScreen *d, int ch, uint64_t b, int &block, uint8_t *&p, bool &level)
{
    if (block < 0 || b < d->starts[block] || b >= d->starts[block + 1]) {
        block = (int)(std::upper_bound(d->starts.begin(), d->starts.end(), b) - d->starts.begin()) - 1;
        p = d->snap->get_block_buf(block, ch, level);
    }
    if (p)
        return p[b - d->starts[block]];
    return level ? 0xff : 0x00;
}

int read_on_screen(void *ctx, uint64_t k, size_t count, int nch, uint64_t *raw)
{
    OnScreen *d = (OnScreen *)ctx;
    for (int c = 0; c < nch; c++) {
        int block = -1;
        uint8_t *p = nullptr;
        bool level = false;
        for (size_t i = 0; i < count; i++) {
            uint64_t w = 0;
            uint64_t b = (k + i) * 8;
            for (int j = 0; j < 8; j++) {
                if (b + j < d->starts.back())
                    w |= (uint64_t)byte_at(d, d->chans[c], b + j, block, p, level) << (8 * j);
            }
            raw[i * nch + c] = w;
        }
    }
    return 0;
}

} // namespace

void McpCapture::write_current(qint64 id, const QString &name, const QString &staging)
{
    if (running() || _session->is_working() || _session->is_saving()) {
        fail(id, "busy", "a capture or a save is running; try again when it is done");
        return;
    }
    DeviceAgent *dev = _session->get_device();
    data::LogicSnapshot *snap = dynamic_cast<data::LogicSnapshot *>(_session->get_snapshot(SR_CHANNEL_LOGIC));
    uint64_t samples = snap ? snap->get_ring_sample_count() : 0;
    if (!dev->have_instance() || dev->get_work_mode() != LOGIC || !_session->have_view_data() || !snap
            || snap->empty() || samples == 0) {
        fail(id, "no_data", "no logic capture is on screen");
        return;
    }

    OnScreen d;
    d.snap = snap;
    for (view::Signal *s : _session->get_signals())
        if (s->signal_type() == SR_CHANNEL_LOGIC && s->enabled() && snap->has_data(s->get_index()))
            d.chans.push_back(s->get_index());
    std::sort(d.chans.begin(), d.chans.end());
    if (d.chans.empty() || (int)d.chans.size() > CAP_MAX_CHANNELS) {
        fail(id, "no_data", "no logic capture is on screen");
        return;
    }
    d.starts.push_back(0);
    for (int i = 0; i < snap->get_block_num(); i++)
        d.starts.push_back(d.starts.back() + snap->get_block_size(i));
    samples = std::min<uint64_t>(samples, d.starts.back() * 8);

    QString base = QDir(staging).filePath(name);
    QByteArray bin_path = (base + ".bin").toUtf8();
    uint64_t per_ch = samples / 64 + (samples % 64 != 0);
    if (cap_write_bin(bin_path.constData(), (int)d.chans.size(), per_ch, samples, read_on_screen, &d)) {
        fail(id, "failed", "cannot write capture data");
        return;
    }

    double vth = NAN;
    if (!dev->get_config_double(SR_CONF_VTH, vth))
        vth = NAN;
    QByteArray device = dev->name().toUtf8();
    QByteArray bin = (name + ".bin").toUtf8();
    struct cap_record r;
    memset(&r, 0, sizeof r);
    r.device = device.constData();
    r.rate = _session->cur_snap_samplerate();
    r.samples = r.got = samples;
    r.per_ch = per_ch;
    r.limit = dev->get_sample_limit();
    r.channels = d.chans.data();
    r.nch = (int)d.chans.size();
    r.vth = vth;
    r.mode = dev->is_hardware() && dev->is_stream_mode() ? "stream" : "buffer";
    r.format = LA_CROSS_DATA;
    r.trig_pos = _session->is_triged() ? (long long)_session->get_trigger_pos() : -1;
    r.stopped_by_user = _stopped_on_screen;
    r.bin = bin.constData();
    char *text = cap_format_record(NULL, &r);
    QJsonObject meta = QJsonDocument::fromJson(QByteArray(text)).object();
    g_free(text);

    // Also what is on screen besides the samples: who captured it, and the decoders.
    meta["source"] = _mcp_on_screen ? "mcp" : "user";
    QJsonArray decoders;
    for (view::DecodeTrace *t : _session->get_decode_signals()) {
        for (data::decode::Decoder *dec : t->decoder()->stack()) {
            const srd_decoder *sd = dec->decoder();
            QJsonObject channels;
            for (GSList *l : {sd->channels, sd->opt_channels})
                for (; l; l = l->next) {
                    const srd_channel *pdch = (const srd_channel *)l->data;
                    int ch = dec->binded_probe_index(pdch);
                    if (ch >= 0)
                        channels[pdch->id] = ch;
                }
            decoders.append(QJsonObject{{"id", sd->id}, {"name", sd->name}, {"channels", channels}});
        }
    }
    meta["decoders"] = decoders;

    QByteArray json = QJsonDocument(meta).toJson(QJsonDocument::Compact) + "\n";
    if (cap_publish_file((base + ".json").toUtf8().constData(), json.constData(), (size_t)json.size())) {
        QFile::remove(base + ".bin");
        fail(id, "failed", "cannot write capture data");
        return;
    }
    emit current_done(id, name, meta);
}

} // namespace mcp
} // namespace pv
