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

// The GUI's connection to the Logic Analyze Agent (doc/mcp-gui-protocol.md):
// it answers the agent's device listing, runs its captures (McpCapture) and
// hands it the capture on screen. It owns the toolbar's MCP button and the
// MCP pane the button opens. Nothing on the MCP path asks the user anything.
// MCP is available only while this app runs: when MCP is enabled in the app's
// settings, the GUI starts the agent at launch, and the agent exits when the
// GUI disconnects or quits. Mac App Store edition only.

#pragma once

#include <QElapsedTimer>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QTimer>

#include "mcpprotocol.h"

class QSocketNotifier;
class QToolButton;
class QWidget;

namespace pv {

class SigSession;

namespace toolbars {
class SamplingBar;
}

namespace mcp {

class McpCapture;

class McpBridge : public QObject
{
    Q_OBJECT

public:
    McpBridge(SigSession *session, toolbars::SamplingBar *bar, QWidget *window);
    ~McpBridge();

    static McpBridge *instance() { return _instance; }

    bool connected() const { return _fd >= 0 && _ok; }
    bool enabled() const { return _enabled; }
    QString last_error() const { return _error; }

    void set_enabled(bool on);   // the app setting; starts or stops the agent
    void start_agent();

    // The MCP pane: a tool window, never modal. Returns it (for the layout check).
    QWidget *show_pane();
    void retranslate();          // the button's text and tooltip

    McpCapture *capture() { return _capture; }

signals:
    void changed();

private slots:
    void try_connect();
    void on_readable();

private:
    void drop(const QString &why);
    bool send(const QJsonObject &msg);
    void handle(const AgentMessage &m);
    void answer_devices(qint64 id);
    void update_button();

    static McpBridge *_instance;

    SigSession *_session;
    QWidget *_window;
    McpCapture *_capture;
    QToolButton *_button;
    QPointer<QWidget> _pane;

    int _fd = -1;
    QSocketNotifier *_notifier = nullptr;
    QByteArray _inbuf;
    bool _ok = false;            // gui_ok received
    QTimer _connect_timer;
    bool _enabled = false;
    bool _busy = false;          // an MCP capture is running (orange dot)
    QElapsedTimer _agent_started;

    QString _error;
};

} // namespace mcp
} // namespace pv
