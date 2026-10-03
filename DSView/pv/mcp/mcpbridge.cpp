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

#include "mcpbridge.h"
#include "mcphooks.h"
#include "mcpplatform.h"

#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPointer>
#include <QPushButton>
#include <QSettings>
#include <QSocketNotifier>
#include <QVBoxLayout>
#include <errno.h>
#include <glib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../deviceagent.h"
#include "../dialogs/dsdialog.h"
#include "../dialogs/dsmessagebox.h"
#include "../log.h"
#include "../sigsession.h"
#include "../ui/langresource.h"
#include "../ui/msgbox.h"

namespace pv {
namespace mcp {

static const int kReconnectMs = 3 * 1000;
static const qint64 kRestartAgentMs = 30 * 1000;    // start the agent again at most this often
static const char *kEnabledKey = "MCP/enabled";
static const int kReleasePromptMs = 25 * 1000;      // the agent waits 30 s for an answer

McpBridge *McpBridge::_instance = nullptr;

McpBridge::McpBridge(SigSession *session, QWidget *window)
    : QObject(window), _session(session), _window(window)
{
    _instance = this;

    _connect_timer.setSingleShot(true);
    connect(&_connect_timer, &QTimer::timeout, this, &McpBridge::try_connect);

    // MCP runs only with this app: at launch, if it is enabled, start the agent.
    _enabled = QSettings().value(kEnabledKey, false).toBool();
    if (_enabled)
        QTimer::singleShot(0, this, [this]() { set_enabled(true); });
}

McpBridge::~McpBridge()
{
    if (_fd >= 0)
        close(_fd);
    _instance = nullptr;
}

// ---------------------------------------------------------------- connection

void McpBridge::start_agent()
{
    QString err;
    _agent_started.start();
    if (!platform::open_agent(&err))
        _error = err;
    _connect_timer.start(kReconnectMs);
    emit changed();
}

void McpBridge::try_connect()
{
    if (_fd >= 0 || !_enabled)
        return;
    QString why;
    int fd = platform::connect_agent(&why);
    if (fd < 0) {
        // The agent exits whenever the GUI disconnects; start it again if needed.
        if (!_agent_started.isValid() || _agent_started.elapsed() >= kRestartAgentMs)
            start_agent();
        else
            _connect_timer.start(kReconnectMs);
        return;
    }
    _fd = fd;
    _inbuf.clear();
    _ok = false;
    _notifier = new QSocketNotifier(_fd, QSocketNotifier::Read, this);
    connect(_notifier, &QSocketNotifier::activated, this, &McpBridge::on_readable);
    // The agent wants gui_hello within 2 s of the connection.
    if (!send(hello_message()))
        drop("cannot send gui_hello");
}

void McpBridge::drop(const QString &why)
{
    if (_fd < 0)
        return;
    dsv_info("MCP agent connection closed: %s", why.toUtf8().constData());
    delete _notifier;
    _notifier = nullptr;
    close(_fd);
    _fd = -1;
    _ok = false;
    if (_prompt)
        _prompt->close();
    apply(_lease.disconnected());
    if (_enabled && why != "version")
        _connect_timer.start(kReconnectMs);
    emit changed();
}

bool McpBridge::send(const QJsonObject &msg)
{
    if (_fd < 0)
        return false;
    QByteArray frame = encode_frame(msg);
    if (frame.isEmpty())
        return false;
    const char *p = frame.constData();
    qint64 left = frame.size();
    while (left > 0) {
        ssize_t n = ::send(_fd, p, (size_t)left, 0);
        if (n < 0 && errno == EINTR)
            continue;
        if (n <= 0) {
            QMetaObject::invokeMethod(this, [this]() { drop("write failed"); }, Qt::QueuedConnection);
            return false;
        }
        p += n;
        left -= n;
    }
    return true;
}

void McpBridge::on_readable()
{
    char buf[8192];
    while (_fd >= 0) {
        ssize_t n = recv(_fd, buf, sizeof(buf), MSG_DONTWAIT);
        if (n > 0) {
            _inbuf.append(buf, (int)n);
            continue;
        }
        if (n < 0 && errno == EINTR)
            continue;
        if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
            break;
        drop(n == 0 ? "closed by the agent" : "read failed");
        return;
    }

    QJsonObject msg;
    bool error = false;
    while (_fd >= 0 && take_frame(_inbuf, msg, error))
        handle(parse_agent_message(msg));
    if (error)
        drop("malformed frame");
}

void McpBridge::handle(const AgentMessage &m)
{
    switch (m.type) {
    case AgentMessage::Ok:
        if (_ok || m.build < kGuiMinAgentBuild || m.min_build > kGuiBuild) {
            _error = L_S(STR_PAGE_MSG, S_ID(IDS_MSG_MCP_AGENT_VERSION),
                      "The Logic Analyze Agent is a different version. Update Logic Analyze.");
            drop("version");
            return;
        }
        _ok = true;
        _error.clear();
        _lease.connected();
        emit changed();
        break;

    case AgentMessage::Error:
        _error = L_S(STR_PAGE_MSG, S_ID(IDS_MSG_MCP_AGENT_VERSION),
                  "The Logic Analyze Agent is a different version. Update Logic Analyze.");
        drop("version");
        break;

    case AgentMessage::LeaseRequest:
    case AgentMessage::LeaseReturned:
        if (!_ok) {
            drop("no gui_ok");
            return;
        }
        apply(m.type == AgentMessage::LeaseRequest ? _lease.lease_request() : _lease.lease_returned());
        break;

    case AgentMessage::Invalid:
        drop("unexpected message");
        break;
    }
}

// ---------------------------------------------------------------- setting

void McpBridge::set_enabled(bool on)
{
    _enabled = on;
    QSettings().setValue(kEnabledKey, on);
    _error.clear();
    if (on) {
        start_agent();
    }
    else {
        // The agent exits when the GUI disconnects.
        _connect_timer.stop();
        drop("MCP turned off");
    }
    emit changed();
}

// ---------------------------------------------------------------- device lease

void McpBridge::send_lease(const QString &op)
{
    if (!op.isEmpty())
        send(lease_message(op));
}

void McpBridge::apply(const GuiLease::Step &s)
{
    send_lease(s.reply);
    if (s.unpark)
        unpark();
    if (s.decide)
        decide_release();
    emit changed();
}

void McpBridge::decide_release()
{
    DeviceAgent *dev = _session->get_device();

    // Only a selected analyzer is held open; anything else releases at once.
    if (!dev->is_hardware()) {
        _released = NULL_HANDLE;
        send_lease(_lease.decide(true));
        notice(L_S(STR_PAGE_MSG, S_ID(IDS_MSG_MCP_CLIENT_USING), "An MCP client is using the analyzer."));
        return;
    }
    // A save cannot be interrupted: the client gets "busy" and may try again.
    if (_session->is_saving()) {
        send_lease(_lease.decide(false));
        return;
    }
    if (_session->is_working() || _session->have_hardware_data()) {
        QString info = _session->is_working()
            ? QString(L_S(STR_PAGE_MSG, S_ID(IDS_MSG_MCP_CAPTURE_STOPS), "The running capture stops."))
            : QString(L_S(STR_PAGE_MSG, S_ID(IDS_MSG_MCP_DATA_CLEARED),
                          "The captured data on screen is cleared; save it first if you need it."));
        QPointer<McpBridge> self(this);
        int seq = ++_prompt_seq;
        QTimer::singleShot(kReleasePromptMs, this, [this, seq]() {
            if (_prompt && seq == _prompt_seq)
                _prompt->close();     // no answer in time: keep the analyzer
        });
        bool yes = MsgBox::Confirm(L_S(STR_PAGE_MSG, S_ID(IDS_MSG_MCP_HAND_OVER),
                                       "An MCP client wants to use the analyzer. Hand it over?"),
                                   info, &_prompt, _window);
        if (!self)
            return;
        _prompt = nullptr;
        // The connection or the request may have gone while the question was open.
        if (_lease.state() != GuiLease::ReleasePending)
            return;
        if (!yes || _session->is_saving()) {
            send_lease(_lease.decide(false));
            emit changed();
            return;
        }
    }
    release_device();
    send_lease(_lease.decide(true));
    notice(L_S(STR_PAGE_MSG, S_ID(IDS_MSG_MCP_CLIENT_USING_TAKE_BACK),
               "An MCP client is using the analyzer. Select it in the device list to take it back."));
    emit changed();
}

void McpBridge::release_device()
{
    _released = NULL_HANDLE;
    DeviceAgent *dev = _session->get_device();
    if (!dev->is_hardware())
        return;
    _released = dev->handle();
    if (_session->is_working())
        _session->stop_capture();
    ds_device_handle demo = demo_handle();
    if (demo == NULL_HANDLE || !_session->set_device(demo))
        dev->release();      // close the analyzer even if the demo device fails
}

void McpBridge::take_back()
{
    send_lease(_lease.reclaim());
    emit changed();
}

void McpBridge::unpark()
{
    ds_device_handle h = _wanted != NULL_HANDLE ? _wanted : _released;
    _wanted = NULL_HANDLE;
    _released = NULL_HANDLE;
    // Go back to the analyzer only if the GUI still sits idle on the demo device.
    if (h == NULL_HANDLE || !in_device_list(h) || !_session->get_device()->is_demo()
            || _session->is_working() || _session->is_saving())
        return;
    _session->set_device(h);
}

ds_device_handle McpBridge::demo_handle()
{
    // libsigrok creates the demo device first; it stays first in the list.
    struct ds_device_base_info *array = NULL;
    int count = 0;
    ds_device_handle h = NULL_HANDLE;
    if (ds_get_device_list(&array, &count) == SR_OK && array != NULL && count > 0)
        h = array[0].handle;
    g_free(array);
    return h;
}

bool McpBridge::in_device_list(ds_device_handle h)
{
    struct ds_device_base_info *array = NULL;
    int count = 0;
    bool found = false;
    if (ds_get_device_list(&array, &count) == SR_OK && array != NULL) {
        for (int i = 0; i < count; i++)
            found = found || array[i].handle == h;
    }
    g_free(array);
    return found;
}

bool McpBridge::is_virtual_device(ds_device_handle h)
{
    return h == demo_handle() || _file_devices.contains(h);
}

bool McpBridge::may_activate(ds_device_handle h)
{
    if (!_lease.lent() || is_virtual_device(h))
        return true;
    // The user picked the analyzer: ask for it back and switch when it returns.
    _wanted = h;
    if (_lease.state() == GuiLease::McpOwned)
        take_back();
    notice(L_S(STR_PAGE_MSG, S_ID(IDS_MSG_MCP_CLIENT_USING_SWITCH),
               "An MCP client is using the analyzer. Logic Analyze switches to it when the client is done."));
    return false;
}

void McpBridge::notice(const QString &text)
{
    QMessageBox *box = new QMessageBox(QMessageBox::Information, QApplication::applicationName(), text,
                                       QMessageBox::Ok, _window);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->setWindowModality(Qt::NonModal);
    box->show();
}

// ---------------------------------------------------------------- pane

void McpBridge::show_pane()
{
    dialogs::DSDialog dlg(_window, true, false);
    dlg.setTitle(L_S(STR_PAGE_DLG, S_ID(IDS_DLG_MCP_TITLE), "MCP"));
    dlg.setMinimumSize(480, 220);

    QWidget *panel = new QWidget(&dlg);
    QVBoxLayout *lay = new QVBoxLayout(panel);
    lay->setContentsMargins(10, 10, 10, 10);
    lay->setSpacing(12);

    QLabel *intro = new QLabel(L_S(STR_PAGE_DLG, S_ID(IDS_DLG_MCP_INTRO),
        "MCP lets AI assistants on this Mac (Claude Desktop, Claude Code, Cursor) capture, "
        "measure and decode signals with the analyzer, through the Logic Analyze Agent. "
        "It is available only while Logic Analyze is open."));
    intro->setWordWrap(true);
    lay->addWidget(intro);

    QGridLayout *grid = new QGridLayout();
    grid->setHorizontalSpacing(12);
    grid->setVerticalSpacing(8);
    QLabel *agent = new QLabel();
    agent->setWordWrap(true);
    QLabel *analyzer = new QLabel();
    analyzer->setWordWrap(true);
    QPushButton *toggle = new QPushButton();
    QPushButton *back = new QPushButton(L_S(STR_PAGE_DLG, S_ID(IDS_DLG_MCP_TAKE_BACK), "Take Back"));
    grid->addWidget(new QLabel(L_S(STR_PAGE_DLG, S_ID(IDS_DLG_MCP_STATUS), "MCP:")), 0, 0, Qt::AlignLeft | Qt::AlignTop);
    grid->addWidget(agent, 0, 1);
    grid->addWidget(toggle, 0, 2);
    grid->addWidget(new QLabel(L_S(STR_PAGE_DLG, S_ID(IDS_DLG_MCP_ANALYZER), "Analyzer:")), 1, 0, Qt::AlignLeft | Qt::AlignTop);
    grid->addWidget(analyzer, 1, 1);
    grid->addWidget(back, 1, 2);
    grid->setColumnStretch(1, 1);
    lay->addLayout(grid);

    QLabel *err = new QLabel();
    err->setWordWrap(true);
    lay->addWidget(err);
    lay->addStretch(1);

    auto refresh = [=]() {
        toggle->setText(enabled() ? L_S(STR_PAGE_DLG, S_ID(IDS_DLG_MCP_TURN_OFF), "Turn Off")
                                  : L_S(STR_PAGE_DLG, S_ID(IDS_DLG_MCP_TURN_ON), "Turn On"));
        if (!enabled())
            agent->setText(L_S(STR_PAGE_DLG, S_ID(IDS_DLG_MCP_OFF), "Off"));
        else if (connected())
            agent->setText(L_S(STR_PAGE_DLG, S_ID(IDS_DLG_MCP_AGENT_RUNNING), "On. The agent is running."));
        else
            agent->setText(L_S(STR_PAGE_DLG, S_ID(IDS_DLG_MCP_AGENT_STARTING), "On. The agent is not running yet."));
        if (reclaim_pending())
            analyzer->setText(L_S(STR_PAGE_DLG, S_ID(IDS_DLG_MCP_LENT_WAITING),
                                  "Lent to an MCP client. Waiting for it to finish."));
        else if (analyzer_lent())
            analyzer->setText(L_S(STR_PAGE_DLG, S_ID(IDS_DLG_MCP_LENT), "Lent to an MCP client."));
        else
            analyzer->setText(L_S(STR_PAGE_DLG, S_ID(IDS_DLG_MCP_HELD),
                                  "Held by Logic Analyze. MCP clients ask for it when they need it."));
        back->setVisible(analyzer_lent());
        back->setEnabled(!reclaim_pending());
        err->setText(last_error());
        err->setVisible(!last_error().isEmpty());
    };
    refresh();

    connect(this, &McpBridge::changed, panel, refresh);
    connect(toggle, &QPushButton::clicked, panel, [this]() { set_enabled(!enabled()); });
    connect(back, &QPushButton::clicked, panel, [this]() { take_back(); });

    dlg.layout()->addWidget(panel);
    dlg.exec();
}

// ---------------------------------------------------------------- hooks

bool device_lent()
{
    return McpBridge::instance() && McpBridge::instance()->analyzer_lent();
}

bool is_virtual_device(ds_device_handle h)
{
    return !McpBridge::instance() || McpBridge::instance()->is_virtual_device(h);
}

bool may_activate(ds_device_handle h)
{
    return !McpBridge::instance() || McpBridge::instance()->may_activate(h);
}

void note_file_device(ds_device_handle h)
{
    if (McpBridge::instance())
        McpBridge::instance()->note_file_device(h);
}

} // namespace mcp
} // namespace pv
