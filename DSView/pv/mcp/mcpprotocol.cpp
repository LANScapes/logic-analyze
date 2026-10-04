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

#include "mcpprotocol.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QRegularExpression>
#include <algorithm>

namespace pv {
namespace mcp {

QByteArray encode_frame(const QJsonObject &msg)
{
    QByteArray body = QJsonDocument(msg).toJson(QJsonDocument::Compact);
    if (body.isEmpty() || body.size() > kMaxFrameBody)
        return QByteArray();
    QByteArray frame;
    quint32 n = (quint32)body.size();
    frame.append(char((n >> 24) & 0xff));
    frame.append(char((n >> 16) & 0xff));
    frame.append(char((n >> 8) & 0xff));
    frame.append(char(n & 0xff));
    frame.append(body);
    return frame;
}

bool take_frame(QByteArray &buf, QJsonObject &msg, bool &error)
{
    error = false;
    if (buf.size() < 4)
        return false;
    const unsigned char *p = (const unsigned char *)buf.constData();
    quint32 n = (quint32(p[0]) << 24) | (quint32(p[1]) << 16) | (quint32(p[2]) << 8) | quint32(p[3]);
    if (n < 1 || n > (quint32)kMaxFrameBody) {
        error = true;
        return false;
    }
    if ((quint32)buf.size() < 4 + n)
        return false;
    QJsonParseError pe;
    QJsonDocument doc = QJsonDocument::fromJson(buf.mid(4, n), &pe);
    buf.remove(0, 4 + n);
    if (pe.error != QJsonParseError::NoError || !doc.isObject()) {
        error = true;
        return false;
    }
    msg = doc.object();
    return true;
}

static QJsonObject base(const char *type)
{
    QJsonObject m;
    m["v"] = kProtocolVersion;
    m["type"] = QString::fromLatin1(type);
    return m;
}

QJsonObject hello_message()
{
    QJsonObject m = base("gui_hello");
    m["proto"] = kProtocolVersion;
    m["epoch"] = kSecurityEpoch;
    m["build"] = kGuiBuild;
    m["min_build"] = kGuiMinAgentBuild;
    return m;
}

// A whole JSON number as an integer; false for fractions and other types.
static bool integer(const QJsonValue &v, qint64 &out)
{
    if (!v.isDouble())
        return false;
    double d = v.toDouble();
    if (d != (double)(qint64)d)
        return false;
    out = (qint64)d;
    return true;
}

static bool number(const QJsonValue &v, double &out)
{
    if (!v.isDouble())
        return false;
    out = v.toDouble();
    return true;
}

// The capture request's fields; an empty result means the request is usable.
static QString parse_request(const QJsonObject &q, CaptureRequest &r)
{
    qint64 n = 0;
    double d = 0;

    QJsonValue ch = q.value("channels");
    if (!ch.isArray() || ch.toArray().isEmpty() || ch.toArray().size() > 16)
        return "channels must be a list of 1 to 16 channel numbers";
    for (const QJsonValue &v : ch.toArray()) {
        if (!integer(v, n) || n < 0 || n > 15)
            return "channels must be numbers 0-15";
        if (std::find(r.channels.begin(), r.channels.end(), (int)n) != r.channels.end())
            return "channels must not repeat";
        r.channels.push_back((int)n);
    }

    if (!integer(q.value("samplerate_hz"), r.samplerate_hz) || r.samplerate_hz <= 0)
        return "samplerate_hz must be a positive integer";

    if (q.contains("samples")) {
        if (!integer(q.value("samples"), r.samples) || r.samples < 1)
            return "samples must be a positive integer";
    }
    else if (q.contains("duration_s")) {
        if (!number(q.value("duration_s"), d) || !(d > 0) || d * (double)r.samplerate_hz > 9.0e18)
            return "duration_s must be a positive number";
        r.samples = qMax<qint64>(1, (qint64)(d * (double)r.samplerate_hz));
    }
    else
        r.samples = 1000000;

    if (q.contains("threshold_v")) {
        if (!number(q.value("threshold_v"), r.threshold_v) || !(r.threshold_v >= 0 && r.threshold_v <= 5))
            return "threshold_v must be 0-5";
    }

    if (q.contains("mode")) {
        QString mode = q.value("mode").toString();
        if (mode != "buffer" && mode != "stream")
            return "mode must be buffer or stream";
        r.stream = mode == "stream" ? 1 : 0;
    }

    QJsonValue tc = q.value("trigger_channel");
    if (!tc.isUndefined() && !tc.isNull()) {
        if (!integer(tc, n) || std::find(r.channels.begin(), r.channels.end(), (int)n) == r.channels.end())
            return "trigger_channel must be one of channels";
        r.trigger_channel = (int)n;
    }
    if (q.contains("trigger_edge")) {
        QString e = q.value("trigger_edge").toString();
        if (e.size() != 1 || !QString("RFC10").contains(e))
            return "trigger_edge must be R, F, C, 1 or 0";
        r.trigger_edge = e.at(0).toLatin1();
    }
    if (q.contains("trigger_position_percent")) {
        if (!integer(q.value("trigger_position_percent"), n) || n < 0 || n > 100)
            return "trigger_position_percent must be 0-100";
        r.trigger_position_percent = (int)n;
    }
    if (q.contains("timeout_ms")) {
        if (!integer(q.value("timeout_ms"), r.timeout_ms) || r.timeout_ms < 1)
            return "timeout_ms must be a positive integer";
    }
    return QString();
}

static bool valid_name(const QString &name)
{
    static const QRegularExpression re("^[A-Za-z0-9_][A-Za-z0-9_-]{0,127}$");
    return re.match(name).hasMatch();
}

AgentMessage parse_agent_message(const QJsonObject &m)
{
    AgentMessage a;
    qint64 v = 0;
    if (!integer(m.value("v"), v) || v != kProtocolVersion)
        return a;
    QString type = m.value("type").toString();

    if (type == "gui_ok") {
        if (integer(m.value("build"), a.build) && integer(m.value("min_build"), a.min_build))
            a.type = AgentMessage::Ok;
        return a;
    }
    if (type == "gui_error") {
        a.type = AgentMessage::Error;
        return a;
    }

    if (!integer(m.value("id"), a.id) || a.id < 0) {
        a.id = -1;
        return a;
    }
    if (type == "devices")
        a.type = AgentMessage::Devices;
    else if (type == "capture_cancel")
        a.type = AgentMessage::CaptureCancel;
    else if (type == "current") {
        a.type = AgentMessage::Current;
        a.name = m.value("name").toString();
        if (!valid_name(a.name))
            a.req_error = "name must be 1-128 characters of A-Z a-z 0-9 _ -, not starting with -";
    }
    else if (type == "capture") {
        a.type = AgentMessage::Capture;
        a.name = m.value("name").toString();
        if (!valid_name(a.name))
            a.req_error = "name must be 1-128 characters of A-Z a-z 0-9 _ -, not starting with -";
        else if (!m.value("req").isObject())
            a.req_error = "req must be an object";
        else
            a.req_error = parse_request(m.value("req").toObject(), a.req);
    }
    return a;
}

QJsonObject devices_ok_message(qint64 id, const QStringList &devices, const QString &selected)
{
    QJsonObject m = base("devices_ok");
    m["id"] = id;
    m["devices"] = QJsonArray::fromStringList(devices);
    m["selected"] = selected;
    return m;
}

QJsonObject capture_started_message(qint64 id)
{
    QJsonObject m = base("capture_started");
    m["id"] = id;
    return m;
}

QJsonObject capture_done_message(qint64 id, const QString &name, const QJsonObject &meta)
{
    QJsonObject m = base("capture_done");
    m["id"] = id;
    m["name"] = name;
    m["meta"] = meta;
    return m;
}

QJsonObject current_ok_message(qint64 id, const QString &name, const QJsonObject &meta)
{
    QJsonObject m = base("current_ok");
    m["id"] = id;
    m["name"] = name;
    m["meta"] = meta;
    return m;
}

QJsonObject capture_error_message(qint64 id, const QString &code, const QString &message)
{
    QJsonObject m = base("capture_error");
    m["id"] = id;
    m["code"] = code;
    m["message"] = message;
    return m;
}

} // namespace mcp
} // namespace pv
