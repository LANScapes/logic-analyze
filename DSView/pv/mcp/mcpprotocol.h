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

// One frame: 4-byte big-endian length, then that many bytes of one JSON object.
// Returns an empty array if the body would be longer than kMaxFrameBody.
QByteArray encode_frame(const QJsonObject &msg);

// Takes one complete frame off the front of buf into msg and returns true.
// Returns false if more bytes are needed, or with error set on a bad frame.
bool take_frame(QByteArray &buf, QJsonObject &msg, bool &error);

// GUI -> agent.
QJsonObject hello_message();
QJsonObject lease_message(const QString &op);    // released, busy or reclaim

// Agent -> GUI.
struct AgentMessage
{
    enum Type { Invalid, Ok, Error, LeaseRequest, LeaseReturned };

    Type type = Invalid;
    qint64 build = 0;            // gui_ok
    qint64 min_build = 0;        // gui_ok
};

AgentMessage parse_agent_message(const QJsonObject &m);

// The GUI's view of the device lease on the one open connection.
class GuiLease
{
public:
    enum State { NoAgent, GuiOwned, ReleasePending, McpOwned, ReclaimPending };

    struct Step
    {
        QString reply;           // lease op to send now, or empty
        bool decide = false;     // ask whether to release (then call decide())
        bool unpark = false;     // the GUI may use the analyzer again
    };

    State state() const { return _state; }

    // True while an MCP client holds the analyzer or the GUI waits to get it back.
    bool lent() const { return _state == McpOwned || _state == ReclaimPending; }

    void connected() { _state = GuiOwned; }
    Step disconnected();
    Step lease_request();
    QString decide(bool release);
    QString reclaim();
    Step lease_returned();

private:
    State _state = NoAgent;
};

} // namespace mcp
} // namespace pv
