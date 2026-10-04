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

// An MCP capture, run by the GUI in its normal session as if the user pressed
// Start (doc/mcp-gui-protocol.md, "capture"). The request is applied, the data
// recorded and the files written by tools/dslcap/capcore.c, the code dslcap
// uses, so both give the same result. Mac App Store edition only.

#pragma once

#include <QElapsedTimer>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>

#include "../interface/icallbacks.h"
#include "../../../tools/dslcap/capcore.h"
#include "mcpprotocol.h"

namespace pv {

class SigSession;

namespace toolbars {
class SamplingBar;
}

namespace mcp {

class McpCapture : public QObject, public IMessageListener
{
    Q_OBJECT

public:
    McpCapture(SigSession *session, toolbars::SamplingBar *bar, QObject *parent);

    bool running() const { return _id >= 0; }
    // Inside start(): the app's Start runs for an MCP capture, which sets its
    // own trigger (no trigger dialog).
    bool starting() const { return _starting; }

    // Runs one capture into <staging>/<name>.bin and .json. Answers through
    // started() and then done() or failed(), or failed() at once.
    void start(qint64 id, const QString &name, const CaptureRequest &req, const QString &staging);
    void cancel(qint64 id);
    // The agent went away: the capture goes on as the user's own.
    void abandon();
    // The pane's Stop: as the app's Stop button.
    void user_stop();

    // Writes the logic capture on screen (the user's or an MCP one) as
    // <staging>/<name>.bin and .json; answers current_done() or failed().
    void write_current(qint64 id, const QString &name, const QString &staging);

    // While running: the channels, the rate and the depth, for the pane.
    QStringList activity() const;

    // The device to use when the selected one is not an analyzer: the first
    // whose name contains this ("DSLogic", as dslcap; the parity test uses "Demo").
    void set_device_name(const QString &want) { _want = want; }

    void OnMessage(int msg) override;

signals:
    void started(qint64 id);
    void done(qint64 id, const QString &name, const QJsonObject &meta);
    void failed(qint64 id, const QString &code, const QString &message);
    void current_done(qint64 id, const QString &name, const QJsonObject &meta);
    void active_changed(bool on);   // an MCP capture started or ended

private:
    void fail(qint64 id, const QString &code, const QString &message);
    bool choose_device(QString &code, QString &message);
    void stop(bool timed_out);
    void finish();
    void save_settings();
    void restore_settings();

    SigSession *_session;
    toolbars::SamplingBar *_bar;
    QString _want = "DSLogic";

    qint64 _id = -1;
    QString _name;
    QString _out_base;
    struct cap_request _req;
    struct cap_setup _setup;
    bool _starting = false;
    bool _own_stop = false;          // timeout or cancel
    bool _timed_out = false;
    bool _cancelled = false;
    bool _stopped_by_user = false;
    QElapsedTimer _elapsed;
    QTimer _timeout;
    bool _mcp_on_screen = false;     // the data on screen is from an MCP capture
    bool _stopped_on_screen = false; // and the user stopped it early
    struct {
        bool valid = false;
        int collect_mode = 0;
        bool has_clock = false, clock = false;
        bool has_rle = false, rle = false;
        bool has_filter = false;
        int filter = 0;
    } _saved;                        // restored when the capture ends
};

} // namespace mcp
} // namespace pv
