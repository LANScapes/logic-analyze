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
// purchase evidence and the device lease, plus the state the MCP pane shows.
// MCP is available only while this app runs: when MCP is enabled in the app's
// settings, the GUI starts the agent at launch, and the agent exits when the
// GUI disconnects or quits. Mac App Store edition only.

#pragma once

#include <QDateTime>
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
    bool connected() const { return _fd >= 0 && !_boot_epoch.isEmpty(); }
    bool enabled() const { return _enabled; }
    QString connection_text() const;
    QString purchase_text() const;
    bool analyzer_lent() const { return _lease.lent(); }
    bool reclaim_pending() const { return _lease.state() == GuiLease::ReclaimPending; }
    bool purchase_busy() const { return _fetching; }
    QString last_error() const { return _error; }

    void set_enabled(bool on);   // the app setting; starts or stops the agent
    void confirm_purchase();     // AppTransaction.refresh(): explicit user action only
    void take_back();            // reclaim the analyzer from the MCP client
    void start_agent();
    void show_pane();

    // Hooks for SigSession (see sigsession.cpp).
    bool is_virtual_device(ds_device_handle h);
    bool may_activate(ds_device_handle h);
    void note_file_device(ds_device_handle h) { _file_devices.insert(h); }

    // From mcpstorekit.swift, on the main thread.
    void evidence_fetched(bool refresh, const QString &jws, const QString &dvid, const QString &error);

signals:
    void changed();

private slots:
    void try_connect();
    void on_readable();
    void fetch_evidence();

private:
    void drop(const QString &why);
    bool send(const QJsonObject &msg);
    void send_reply(const GuiLease::Reply &r);
    void apply(const GuiLease::Step &s);
    void handle(const AgentMessage &m);
    void send_evidence();
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
    QString _boot_epoch;
    GuiLease _lease;
    QTimer _connect_timer;
    QTimer _evidence_timer;
    bool _enabled = false;
    QElapsedTimer _agent_started;

    QString _jws;                // the latest AppTransaction evidence
    QString _dvid;
    bool _fetching = false;
    QString _fetch_error;
    QElapsedTimer _evidence_sent;
    bool _evidence_queued = false;
    QString _evidence_result;
    qint64 _expires_at = -1;

    ds_device_handle _released = NULL_HANDLE;  // the hardware the GUI gave to MCP
    ds_device_handle _wanted = NULL_HANDLE;    // the device the user asked for meanwhile
    QSet<ds_device_handle> _file_devices;
    dialogs::DSMessageBox *_prompt = nullptr;
    int _prompt_seq = 0;

    QString _error;
};

} // namespace mcp
} // namespace pv
