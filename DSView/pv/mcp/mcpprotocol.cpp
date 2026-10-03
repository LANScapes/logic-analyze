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

#include <QJsonDocument>
#include <QJsonValue>
#include <QVariant>

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

QJsonObject lease_message(const QString &op)
{
    QJsonObject m = base("lease");
    m["op"] = op;
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
    }
    else if (type == "gui_error")
        a.type = AgentMessage::Error;
    else if (type == "lease_request")
        a.type = AgentMessage::LeaseRequest;
    else if (type == "lease_returned")
        a.type = AgentMessage::LeaseReturned;
    return a;
}

GuiLease::Step GuiLease::disconnected()
{
    Step s;
    s.unpark = lent();
    _state = NoAgent;
    return s;
}

GuiLease::Step GuiLease::lease_request()
{
    Step s;
    switch (_state) {
    case GuiOwned:
        _state = ReleasePending;
        s.decide = true;
        break;
    case McpOwned:
        // The agent asks again; the analyzer is already released.
        s.reply = "released";
        break;
    case ReclaimPending:
        // The agent wants it again before it answered our reclaim; keep it.
        _state = GuiOwned;
        s.reply = "busy";
        s.unpark = true;
        break;
    case ReleasePending:   // still deciding; one answer covers both
    case NoAgent:
        break;
    }
    return s;
}

QString GuiLease::decide(bool release)
{
    if (_state != ReleasePending)
        return QString();
    _state = release ? McpOwned : GuiOwned;
    return release ? "released" : "busy";
}

QString GuiLease::reclaim()
{
    if (_state != McpOwned)
        return QString();
    _state = ReclaimPending;
    return "reclaim";
}

GuiLease::Step GuiLease::lease_returned()
{
    Step s;
    if (_state != ReclaimPending)
        return s;
    _state = GuiOwned;
    s.unpark = true;
    return s;
}

} // namespace mcp
} // namespace pv
