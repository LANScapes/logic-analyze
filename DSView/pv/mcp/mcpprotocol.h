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

// The GUI side of the agent channel g<epoch>, version 1, build 2: framing and
// messages, as documented in doc/mcp-gui-protocol.md. Pure logic (QtCore only),
// so it is unit-tested without the app (DSView/pv/mcp/test_mcpprotocol.cpp).

#pragma once

#include <QByteArray>
#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <vector>

namespace pv {
namespace mcp {

const int kProtocolVersion = 1;
const int kSecurityEpoch = 1;           // the socket is g<kSecurityEpoch>
const int kGuiBuild = 2;                // this GUI's protocol build (2: GUI captures)
const int kGuiMinAgentBuild = 2;        // the oldest agent build this GUI talks to
const int kMaxFrameBody = 32 * 1024 - 4;

// One frame: 4-byte big-endian length, then that many bytes of one JSON object.
// Returns an empty array if the body would be longer than kMaxFrameBody.
QByteArray encode_frame(const QJsonObject &msg);

// Takes one complete frame off the front of buf into msg and returns true.
// Returns false if more bytes are needed, or with error set on a bad frame.
bool take_frame(QByteArray &buf, QJsonObject &msg, bool &error);

// A capture request's fields (doc/mcp-gui-protocol.md, "capture").
struct CaptureRequest
{
    std::vector<int> channels;
    qint64 samplerate_hz = 0;
    qint64 samples = 0;           // resolved from duration_s if needed
    double threshold_v = 1.6;
    bool stream = false;
    int trigger_channel = -1;     // -1: none
    char trigger_edge = 'R';
    int trigger_position_percent = 10;
    qint64 timeout_ms = 30000;
};

// Agent -> GUI.
struct AgentMessage
{
    enum Type { Invalid, Ok, Error, Devices, Capture, CaptureCancel, Current };

    Type type = Invalid;
    qint64 build = 0;            // gui_ok
    qint64 min_build = 0;        // gui_ok
    qint64 id = -1;              // devices, capture, capture_cancel
    QString name;                // capture, current: the file base name
    CaptureRequest req;          // capture
    QString req_error;           // capture, current: why the request is unusable (then "unsupported")
};

AgentMessage parse_agent_message(const QJsonObject &m);

// GUI -> agent.
QJsonObject hello_message();
QJsonObject devices_ok_message(qint64 id, const QStringList &devices, const QString &selected);
QJsonObject capture_started_message(qint64 id);
QJsonObject capture_done_message(qint64 id, const QString &name, const QJsonObject &meta);
QJsonObject capture_error_message(qint64 id, const QString &code, const QString &message);
QJsonObject current_ok_message(qint64 id, const QString &name, const QJsonObject &meta);

} // namespace mcp
} // namespace pv
