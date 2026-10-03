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

// The GUI side of the agent channel g<epoch>, version 1: framing, messages and
// the device lease, as documented in doc/mcp-gui-protocol.md. Pure logic (QtCore
// only), so it is unit-tested without the app (DSView/pv/mcp/test_mcpprotocol.cpp).

#pragma once

#include <QByteArray>
#include <QJsonObject>
#include <QString>

namespace pv {
namespace mcp {

const int kProtocolVersion = 1;
const int kSecurityEpoch = 1;           // the socket is g<kSecurityEpoch>
const int kGuiBuild = 1;                // this GUI's protocol build
const int kGuiMinAgentBuild = 1;        // the oldest agent build this GUI talks to
const int kMaxFrameBody = 32 * 1024 - 4;
const int kMaxJwsBytes = 16 * 1024;

// One frame: 4-byte big-endian length, then that many bytes of one JSON object.
// Returns an empty array if the body would be longer than kMaxFrameBody.
QByteArray encode_frame(const QJsonObject &msg);

// Takes one complete frame off the front of buf into msg and returns true.
// Returns false if more bytes are needed, or with error set on a bad frame.
bool take_frame(QByteArray &buf, QJsonObject &msg, bool &error);

// GUI -> agent.
QJsonObject hello_message();
QJsonObject evidence_message(const QString &jws, const QString &device_verification_id);
QJsonObject lease_message(const QString &op, const QString &nonce,
                          const QString &boot_epoch, qint64 device_generation);

// Agent -> GUI.
struct AgentMessage
{
    enum Type { Invalid, Ok, Error, EvidenceResult, LeaseRequest, LeaseReturned };

    Type type = Invalid;
    QString boot_epoch;          // gui_ok, lease_*
    qint64 build = 0;            // gui_ok
    qint64 min_build = 0;        // gui_ok
    QString code;                // gui_error
    QString result;              // evidence_result
    qint64 expires_at = -1;      // evidence_result (epoch ms), -1 for null
    QString nonce;               // lease_*
    qint64 device_generation = 0;// lease_*
};

AgentMessage parse_agent_message(const QJsonObject &m);

bool is_hex32(const QString &s);
QString new_nonce();             // 32 lowercase hex digits from the system RNG

// The GUI's view of the device lease (design section 7). The agent keeps the
// authoritative state; this mirrors it from the messages exchanged.
class GuiLease
{
public:
    enum State { NoAgent, GuiOwned, ReleasePending, McpOwned, ReclaimPending };

    struct Reply
    {
        bool send = false;
        QString op;              // released, busy or reclaim
        QString nonce;
        qint64 device_generation = 0;
    };

    struct Step
    {
        Reply reply;             // send this lease message now
        bool decide = false;     // ask whether to release (then call decide())
        bool unpark = false;     // the GUI may use the analyzer again
    };

    State state() const { return _state; }
    const QString &boot_epoch() const { return _boot_epoch; }

    // True while an MCP client holds the analyzer or the GUI waits to get it back.
    bool lent() const { return _state == McpOwned || _state == ReclaimPending; }

    Step connected(const QString &boot_epoch);
    Step disconnected();
    Step lease_request(const QString &nonce, const QString &boot_epoch, qint64 gen);
    Reply decide(bool release);
    Reply reclaim(const QString &nonce);
    Step lease_returned(const QString &nonce, const QString &boot_epoch, qint64 gen);

private:
    State _state = NoAgent;
    QString _boot_epoch;
    QString _nonce;              // the request being answered, or our reclaim
    qint64 _gen = 0;
};

} // namespace mcp
} // namespace pv
