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
#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
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
#include "../ui/msgbox.h"

namespace pv {
namespace mcp {

static const int kReconnectMs = 3 * 1000;
static const qint64 kRestartAgentMs = 30 * 1000;    // start the agent again at most this often
static const char *kEnabledKey = "MCP/enabled";
static const int kEvidenceEveryMs = 24 * 60 * 60 * 1000;
static const qint64 kEvidenceGapMs = 5500;          // the agent takes one evidence per 5 s
static const int kReleasePromptMs = 25 * 1000;      // the agent waits 30 s for an answer

McpBridge *McpBridge::_instance = nullptr;

McpBridge::McpBridge(SigSession *session, QWidget *window)
    : QObject(window), _session(session), _window(window)
{
    _instance = this;

    _connect_timer.setSingleShot(true);
    connect(&_connect_timer, &QTimer::timeout, this, &McpBridge::try_connect);
    connect(&_evidence_timer, &QTimer::timeout, this, &McpBridge::fetch_evidence);

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
    _boot_epoch.clear();
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
    _boot_epoch.clear();
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
    if (_boot_epoch.isEmpty() && m.type != AgentMessage::Ok && m.type != AgentMessage::Error) {
        drop("no gui_ok");
        return;
    }

    switch (m.type) {
    case AgentMessage::Ok:
        if (!_boot_epoch.isEmpty() || m.build < kGuiMinAgentBuild || m.min_build > kGuiBuild) {
            _error = "The Logic Analyze Agent is a different version. Update Logic Analyze.";
            drop("version");
            return;
        }
        _boot_epoch = m.boot_epoch;
        _error.clear();
        apply(_lease.connected(_boot_epoch));
        if (!_jws.isEmpty())
            send_evidence();
        emit changed();
        break;

    case AgentMessage::Error:
        _error = "The Logic Analyze Agent is a different version. Update Logic Analyze.";
        drop("version");
        break;

    case AgentMessage::EvidenceResult:
        _evidence_result = m.result;
        if (m.result == "granted")
            _expires_at = m.expires_at;
        else if (m.result == "revoked")
            _expires_at = -1;
        emit changed();
        break;

    case AgentMessage::LeaseRequest:
        apply(_lease.lease_request(m.nonce, m.boot_epoch, m.device_generation));
        break;

    case AgentMessage::LeaseReturned:
        apply(_lease.lease_returned(m.nonce, m.boot_epoch, m.device_generation));
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
        if (_jws.isEmpty())
            fetch_evidence();
        _evidence_timer.start(kEvidenceEveryMs);
        start_agent();
    }
    else {
        // The agent exits when the GUI disconnects.
        _evidence_timer.stop();
        _connect_timer.stop();
        drop("MCP turned off");
    }
    emit changed();
}

// ---------------------------------------------------------------- evidence

static void evidence_cb(void *ctx, const char *jws, const char *dvid, const char *error)
{
    bool refresh = ctx != nullptr;
    QString j = jws ? QString::fromUtf8(jws) : QString();
    QString d = dvid ? QString::fromUtf8(dvid) : QString();
    QString e = error ? QString::fromUtf8(error) : QString();
    QMetaObject::invokeMethod(qApp, [refresh, j, d, e]() {
        if (McpBridge::instance())
            McpBridge::instance()->evidence_fetched(refresh, j, d, e);
    }, Qt::QueuedConnection);
}

void McpBridge::fetch_evidence()
{
    if (_fetching)
        return;
    _fetching = true;
    la_mcp_fetch_app_transaction(0, nullptr, evidence_cb);
}

void McpBridge::confirm_purchase()
{
    if (_fetching)
        return;
    _fetching = true;
    emit changed();
    la_mcp_fetch_app_transaction(1, (void *)this, evidence_cb);
}

void McpBridge::evidence_fetched(bool refresh, const QString &jws, const QString &dvid, const QString &error)
{
    _fetching = false;
    if (jws.isEmpty() || jws.toUtf8().size() > kMaxJwsBytes) {
        _fetch_error = error.isEmpty() ? QString("The App Store returned no usable purchase record.") : error;
        dsv_info("MCP purchase evidence unavailable (%s): %s", refresh ? "refresh" : "shared",
                 _fetch_error.toUtf8().constData());
    }
    else {
        _fetch_error.clear();
        _jws = jws;
        _dvid = dvid;
        send_evidence();
    }
    emit changed();
}

void McpBridge::send_evidence()
{
    if (!connected() || _jws.isEmpty() || _evidence_queued)
        return;
    if (_evidence_sent.isValid() && _evidence_sent.elapsed() < kEvidenceGapMs) {
        _evidence_queued = true;
        QTimer::singleShot(int(kEvidenceGapMs - _evidence_sent.elapsed()), this, [this]() {
            _evidence_queued = false;
            send_evidence();
        });
        return;
    }
    if (send(evidence_message(_jws, _dvid)))
        _evidence_sent.start();
}

// ---------------------------------------------------------------- device lease

void McpBridge::send_reply(const GuiLease::Reply &r)
{
    if (r.send)
        send(lease_message(r.op, r.nonce, _lease.boot_epoch(), r.device_generation));
}

void McpBridge::apply(const GuiLease::Step &s)
{
    send_reply(s.reply);
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
        send_reply(_lease.decide(true));
        notice("An MCP client is using the analyzer.");
        return;
    }
    // A save cannot be interrupted: the client gets "busy" and may try again.
    if (_session->is_saving()) {
        send_reply(_lease.decide(false));
        return;
    }
    if (_session->is_working() || _session->have_hardware_data()) {
        QString info = _session->is_working()
            ? QString("The running capture stops.")
            : QString("The captured data on screen is cleared; save it first if you need it.");
        QPointer<McpBridge> self(this);
        int seq = ++_prompt_seq;
        QTimer::singleShot(kReleasePromptMs, this, [this, seq]() {
            if (_prompt && seq == _prompt_seq)
                _prompt->close();     // no answer in time: keep the analyzer
        });
        bool yes = MsgBox::Confirm("An MCP client wants to use the analyzer. Hand it over?", info, &_prompt, _window);
        if (!self)
            return;
        _prompt = nullptr;
        // The connection or the request may have gone while the question was open.
        if (_lease.state() != GuiLease::ReleasePending)
            return;
        if (!yes || _session->is_saving()) {
            send_reply(_lease.decide(false));
            emit changed();
            return;
        }
    }
    release_device();
    send_reply(_lease.decide(true));
    notice("An MCP client is using the analyzer. Select it in the device list to take it back.");
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
    send_reply(_lease.reclaim(new_nonce()));
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
    notice("An MCP client is using the analyzer. Logic Analyze switches to it when the client is done.");
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

QString McpBridge::connection_text() const
{
    if (!_enabled)
        return "Off";
    return connected() ? "Running" : "Starting...";
}

QString McpBridge::purchase_text() const
{
    if (_fetching)
        return "Checking with the App Store...";
    if (_evidence_result == "granted" && _expires_at > 0)
        return QString("Confirmed. MCP is available until %1.")
            .arg(QLocale().toString(QDateTime::fromMSecsSinceEpoch(_expires_at), QLocale::ShortFormat));
    if (_evidence_result == "not_newer")
        return "Confirmed earlier (the agent already has this purchase record).";
    if (_evidence_result == "revoked")
        return "The App Store reports this purchase as refunded or revoked.";
    if (_evidence_result == "rejected")
        return "The agent did not accept the purchase record. Click Confirm Purchase.";
    if (!_fetch_error.isEmpty())
        return "Not confirmed yet. Click Confirm Purchase.";
    if (!_jws.isEmpty())
        return "Read from the App Store; sent when the agent runs.";
    return "Not confirmed yet.";
}

void McpBridge::show_pane()
{
    dialogs::DSDialog dlg(_window, true, false);
    dlg.setTitle("MCP");
    dlg.setMinimumSize(520, 300);

    QWidget *panel = new QWidget(&dlg);
    QVBoxLayout *lay = new QVBoxLayout(panel);
    lay->setContentsMargins(10, 10, 10, 10);
    lay->setSpacing(12);

    QLabel *intro = new QLabel(
        "MCP lets AI assistants on this Mac (Claude Desktop, Claude Code, Cursor) capture, "
        "measure and decode signals with the analyzer, through the Logic Analyze Agent. "
        "It is available only while Logic Analyze is open.");
    intro->setWordWrap(true);
    lay->addWidget(intro);

    QGridLayout *grid = new QGridLayout();
    grid->setHorizontalSpacing(12);
    grid->setVerticalSpacing(8);
    QLabel *conn = new QLabel();
    QLabel *purchase = new QLabel();
    QLabel *analyzer = new QLabel();
    purchase->setWordWrap(true);
    analyzer->setWordWrap(true);
    QPushButton *toggle = new QPushButton();
    QPushButton *confirm = new QPushButton("Confirm Purchase");
    QPushButton *back = new QPushButton("Take Back");
    grid->addWidget(new QLabel("MCP:"), 0, 0, Qt::AlignLeft | Qt::AlignTop);
    grid->addWidget(conn, 0, 1);
    grid->addWidget(toggle, 0, 2);
    grid->addWidget(new QLabel("Purchase:"), 2, 0, Qt::AlignLeft | Qt::AlignTop);
    grid->addWidget(purchase, 2, 1);
    grid->addWidget(confirm, 2, 2);
    grid->addWidget(new QLabel("Analyzer:"), 3, 0, Qt::AlignLeft | Qt::AlignTop);
    grid->addWidget(analyzer, 3, 1);
    grid->addWidget(back, 3, 2);
    grid->setColumnStretch(1, 1);
    lay->addLayout(grid);

    QLabel *clients = new QLabel(
        "AI clients are added, listed and revoked in the Logic Analyze Agent's menu-bar item. "
        "Adding a client gives it a pairing token: anything that can read that client's "
        "configuration can use the analyzer within the access you grant there.");
    clients->setWordWrap(true);
    lay->addWidget(clients);
    QHBoxLayout *row = new QHBoxLayout();
    QPushButton *open = new QPushButton("Show Logic Analyze Agent");
    row->addWidget(open);
    row->addStretch(1);
    lay->addLayout(row);

    QLabel *err = new QLabel();
    err->setWordWrap(true);
    lay->addWidget(err);
    lay->addStretch(1);

    auto refresh = [=]() {
        toggle->setText(enabled() ? "Turn Off" : "Turn On");
        conn->setText(connection_text());
        open->setEnabled(connected());
        purchase->setText(purchase_text());
        confirm->setEnabled(!purchase_busy());
        if (reclaim_pending())
            analyzer->setText("Waiting for the MCP client to finish.");
        else if (analyzer_lent())
            analyzer->setText("In use by an MCP client.");
        else
            analyzer->setText("Used by Logic Analyze. MCP clients ask for it when they need it.");
        back->setVisible(analyzer_lent());
        back->setEnabled(!reclaim_pending());
        err->setText(last_error());
        err->setVisible(!last_error().isEmpty());
    };
    refresh();

    connect(this, &McpBridge::changed, panel, refresh);
    connect(toggle, &QPushButton::clicked, panel, [this]() { set_enabled(!enabled()); });
    connect(confirm, &QPushButton::clicked, panel, [this]() { confirm_purchase(); });
    connect(back, &QPushButton::clicked, panel, [this]() { take_back(); });
    connect(open, &QPushButton::clicked, panel, [this]() { start_agent(); });

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
