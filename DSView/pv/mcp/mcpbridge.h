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
// the device lease, plus the state the MCP pane shows.
// MCP is available only while this app runs: when MCP is enabled in the app's
// settings, the GUI starts the agent at launch, and the agent exits when the
// GUI disconnects or quits. Mac App Store edition only.

#pragma once

#include <QElapsedTimer>
#include <QObject>
#include <QSet>
#include <QString>
#include <QTimer>
#include <libsigrok.h>

#include "mcpprotocol.h"

class QSocketNotifier;
class QWidget;

namespace pv {

class SigSession;

namespace dialogs {
class DSMessageBox;
}

namespace mcp {

class McpBridge : public QObject
{
    Q_OBJECT

public:
    McpBridge(SigSession *session, QWidget *window);
    ~McpBridge();

    static McpBridge *instance() { return _instance; }

    // For the MCP pane.
    bool connected() const { return _fd >= 0 && _ok; }
    bool enabled() const { return _enabled; }
    bool analyzer_lent() const { return _lease.lent(); }
    bool reclaim_pending() const { return _lease.state() == GuiLease::ReclaimPending; }
    QString last_error() const { return _error; }

    void set_enabled(bool on);   // the app setting; starts or stops the agent
    void take_back();            // reclaim the analyzer from the MCP client
    void start_agent();
    void show_pane();

    // Hooks for SigSession (see sigsession.cpp).
    bool is_virtual_device(ds_device_handle h);
    bool may_activate(ds_device_handle h);
    void note_file_device(ds_device_handle h) { _file_devices.insert(h); }

signals:
    void changed();

private slots:
    void try_connect();
    void on_readable();

private:
    void drop(const QString &why);
    bool send(const QJsonObject &msg);
    void send_lease(const QString &op);
    void apply(const GuiLease::Step &s);
    void handle(const AgentMessage &m);
    void decide_release();
    void release_device();
    void unpark();
    ds_device_handle demo_handle();
    bool in_device_list(ds_device_handle h);
    void notice(const QString &text);

    static McpBridge *_instance;

    SigSession *_session;
    QWidget *_window;

    int _fd = -1;
    QSocketNotifier *_notifier = nullptr;
    QByteArray _inbuf;
    bool _ok = false;            // gui_ok received
    GuiLease _lease;
    QTimer _connect_timer;
    bool _enabled = false;
    QElapsedTimer _agent_started;

    ds_device_handle _released = NULL_HANDLE;  // the hardware the GUI gave to MCP
    ds_device_handle _wanted = NULL_HANDLE;    // the device the user asked for meanwhile
    QSet<ds_device_handle> _file_devices;
    dialogs::DSMessageBox *_prompt = nullptr;
    int _prompt_seq = 0;

    QString _error;
};

} // namespace mcp
} // namespace pv
