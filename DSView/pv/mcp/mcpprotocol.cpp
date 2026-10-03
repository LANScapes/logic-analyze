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
#include <QRandomGenerator>
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

QJsonObject evidence_message(const QString &jws, const QString &device_verification_id)
{
    QJsonObject m = base("evidence");
    m["jws"] = jws;
    m["device_verification_id"] = device_verification_id;
    return m;
}

QJsonObject lease_message(const QString &op, const QString &nonce,
                          const QString &boot_epoch, qint64 device_generation)
{
    QJsonObject m = base("lease");
    m["op"] = op;
    m["nonce"] = nonce;
    m["boot_epoch"] = boot_epoch;
    m["device"] = QStringLiteral("default");
    m["device_generation"] = device_generation;
    return m;
}

bool is_hex32(const QString &s)
{
    if (s.size() != 32)
        return false;
    for (QChar c : s) {
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')))
            return false;
    }
    return true;
}

QString new_nonce()
{
    QString s;
    for (int i = 0; i < 4; i++)
        s += QString("%1").arg(QRandomGenerator::system()->generate(), 8, 16, QChar('0'));
    return s;
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
        a.boot_epoch = m.value("boot_epoch").toString();
        if (is_hex32(a.boot_epoch) && integer(m.value("build"), a.build)
                && integer(m.value("min_build"), a.min_build))
            a.type = AgentMessage::Ok;
    }
    else if (type == "gui_error") {
        a.code = m.value("code").toString();
        a.type = AgentMessage::Error;
    }
    else if (type == "evidence_result") {
        a.result = m.value("result").toString();
        QJsonValue e = m.value("expires_at");
        bool ok = e.isNull() || integer(e, a.expires_at);
        if (e.isNull())
            a.expires_at = -1;
        if (ok && (a.result == "granted" || a.result == "not_newer"
                   || a.result == "revoked" || a.result == "rejected"))
            a.type = AgentMessage::EvidenceResult;
    }
    else if (type == "lease_request" || type == "lease_returned") {
        a.nonce = m.value("nonce").toString();
        a.boot_epoch = m.value("boot_epoch").toString();
        if (is_hex32(a.nonce) && is_hex32(a.boot_epoch) && m.value("device").toString() == "default"
                && integer(m.value("device_generation"), a.device_generation) && a.device_generation >= 0)
            a.type = type == "lease_request" ? AgentMessage::LeaseRequest : AgentMessage::LeaseReturned;
    }
    return a;
}

static GuiLease::Reply reply(const QString &op, const QString &nonce, qint64 gen)
{
    GuiLease::Reply r;
    r.send = true;
    r.op = op;
    r.nonce = nonce;
    r.device_generation = gen;
    return r;
}

GuiLease::Step GuiLease::connected(const QString &boot_epoch)
{
    Step s = disconnected();
    _boot_epoch = boot_epoch;
    _state = GuiOwned;
    return s;
}

GuiLease::Step GuiLease::disconnected()
{
    Step s;
    s.unpark = lent();
    _state = NoAgent;
    _boot_epoch.clear();
    _nonce.clear();
    _gen = 0;
    return s;
}

GuiLease::Step GuiLease::lease_request(const QString &nonce, const QString &boot_epoch, qint64 gen)
{
    Step s;
    if (_state == NoAgent || boot_epoch != _boot_epoch)
        return s;

    switch (_state) {
    case GuiOwned:
        _state = ReleasePending;
        _nonce = nonce;
        _gen = gen;
        s.decide = true;
        break;
    case ReleasePending:
        // A newer request while the GUI still decides: answer that one.
        _nonce = nonce;
        _gen = gen;
        break;
    case McpOwned:
        // The agent lost our earlier answer; the analyzer is already released.
        _nonce = nonce;
        _gen = gen;
        s.reply = reply("released", nonce, gen);
        break;
    case ReclaimPending:
        // The agent gave the analyzer back before answering our reclaim; keep it.
        _state = GuiOwned;
        _nonce.clear();
        s.reply = reply("busy", nonce, gen);
        s.unpark = true;
        break;
    case NoAgent:
        break;
    }
    return s;
}

GuiLease::Reply GuiLease::decide(bool release)
{
    if (_state != ReleasePending)
        return Reply();
    _state = release ? McpOwned : GuiOwned;
    return reply(release ? "released" : "busy", _nonce, _gen);
}

GuiLease::Reply GuiLease::reclaim(const QString &nonce)
{
    if (_state != McpOwned)
        return Reply();
    _state = ReclaimPending;
    _nonce = nonce;
    return reply("reclaim", nonce, _gen);
}

GuiLease::Step GuiLease::lease_returned(const QString &nonce, const QString &boot_epoch, qint64 gen)
{
    Step s;
    if (_state != ReclaimPending || nonce != _nonce || boot_epoch != _boot_epoch || gen != _gen)
        return s;
    _state = GuiOwned;
    _nonce.clear();
    s.unpark = true;
    return s;
}

} // namespace mcp
} // namespace pv
