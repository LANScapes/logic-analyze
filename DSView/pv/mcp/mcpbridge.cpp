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
#include "../ui/xtoolbutton.h"
#include "mcpcapture.h"
#include "mcpplatform.h"

#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QSettings>
#include <QSocketNotifier>
#include <QToolButton>
#include <QVBoxLayout>
#include <errno.h>
#include <glib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../deviceagent.h"
#include "../dialogs/dsdialog.h"
#include "../log.h"
#include "../sigsession.h"
#include "../toolbars/samplingbar.h"
#include "../ui/langresource.h"

namespace pv {
namespace mcp {

static const int kReconnectMs = 3 * 1000;
static const qint64 kRestartAgentMs = 30 * 1000;    // start the agent again at most this often
static const char *kEnabledKey = "MCP/enabled";
static const QColor kGreen(0x00, 0x99, 0x49);      // the USB 2 and Start icons' green
static const QColor kOrange(0xff, 0x95, 0x00);     // MCP is using the analyzer
static const char *kErrorMark = "<span style='color:#e0483e'>●</span> ";

// The MCP button's icon: nothing, a ring (starting) or a dot.
static QIcon dot_icon(const QColor &color, bool ring)
{
    QPixmap pm(64, 64);
    pm.fill(Qt::transparent);
    if (color.isValid()) {
        QPainter p(&pm);
        p.setRenderHint(QPainter::Antialiasing);
        if (ring) {
            p.setPen(QPen(color, 5));
            p.setBrush(Qt::NoBrush);
        }
        else {
            p.setPen(Qt::NoPen);
            p.setBrush(color);
        }
        p.drawEllipse(QPointF(32, 32), 13, 13);
    }
    return QIcon(pm);
}

McpBridge *McpBridge::_instance = nullptr;

McpBridge::McpBridge(SigSession *session, toolbars::SamplingBar *bar, QWidget *window)
    : QObject(window), _session(session), _window(window)
{
    _instance = this;

    _capture = new McpCapture(session, bar, this);
    connect(_capture, &McpCapture::started, this, [this](qint64 id) {
        send(capture_started_message(id));
    });
    connect(_capture, &McpCapture::done, this, [this](qint64 id, const QString &name, const QJsonObject &meta) {
        send(capture_done_message(id, name, meta));
    });
    connect(_capture, &McpCapture::failed, this, [this](qint64 id, const QString &code, const QString &message) {
        send(capture_error_message(id, code, message));
    });
    connect(_capture, &McpCapture::current_done, this, [this](qint64 id, const QString &name, const QJsonObject &meta) {
        send(current_ok_message(id, name, meta));
    });
    connect(_capture, &McpCapture::active_changed, this, [this](bool on) {
        _busy = on;
        emit changed();
    });

    // The MCP button; MainWindow puts it between Options and Help.
    _button = new XToolButton(bar);   // lines its label up with the other toolbar buttons
    _button->setObjectName("mcp_button");
    _button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    connect(_button, &QToolButton::clicked, this, [this]() { show_pane(); });
    connect(this, &McpBridge::changed, this, &McpBridge::update_button);
    retranslate();

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
    _capture->abandon();
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
    if (m.type != AgentMessage::Ok && m.type != AgentMessage::Error
            && m.type != AgentMessage::Invalid && !_ok) {
        drop("no gui_ok");
        return;
    }

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
        emit changed();
        break;

    case AgentMessage::Error:
        _error = L_S(STR_PAGE_MSG, S_ID(IDS_MSG_MCP_AGENT_VERSION),
                  "The Logic Analyze Agent is a different version. Update Logic Analyze.");
        drop("version");
        break;

    case AgentMessage::Devices:
        answer_devices(m.id);
        break;

    case AgentMessage::Capture: {
        if (!m.req_error.isEmpty()) {
            send(capture_error_message(m.id, "unsupported", m.req_error));
            break;
        }
        QString why;
        QString staging = platform::staging_dir(&why);
        if (staging.isEmpty()) {
            send(capture_error_message(m.id, "failed", why));
            break;
        }
        _capture->start(m.id, m.name, m.req, staging);
        break;
    }

    case AgentMessage::CaptureCancel:
        _capture->cancel(m.id);
        break;

    case AgentMessage::Current: {
        if (!m.req_error.isEmpty()) {
            send(capture_error_message(m.id, "unsupported", m.req_error));
            break;
        }
        QString why;
        QString staging = platform::staging_dir(&why);
        if (staging.isEmpty()) {
            send(capture_error_message(m.id, "failed", why));
            break;
        }
        _capture->write_current(m.id, m.name, staging);
        break;
    }

    case AgentMessage::Invalid:
        drop("unexpected message");
        break;
    }
}

void McpBridge::answer_devices(qint64 id)
{
    QStringList names;
    struct ds_device_base_info *array = NULL;
    int count = 0;
    if (ds_get_device_list(&array, &count) == SR_OK && array != NULL) {
        for (int i = 0; i < count; i++)
            names << QString::fromUtf8(array[i].name);
    }
    g_free(array);
    DeviceAgent *dev = _session->get_device();
    send(devices_ok_message(id, names, dev->have_instance() ? dev->name() : QString()));
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

// ---------------------------------------------------------------- button and pane

void McpBridge::retranslate()
{
    _button->setText(L_S(STR_PAGE_TOOLBAR, S_ID(IDS_TOOLBAR_MCP), "MCP"));
    update_button();
}

void McpBridge::update_button()
{
    QString tip;
    if (!_enabled) {
        _button->setIcon(dot_icon(QColor(), false));
        tip = L_S(STR_PAGE_TOOLBAR, S_ID(IDS_TOOLBAR_MCP_TIP_OFF), "MCP is off");
    }
    else if (_busy) {
        _button->setIcon(dot_icon(kOrange, false));
        tip = L_S(STR_PAGE_TOOLBAR, S_ID(IDS_TOOLBAR_MCP_TIP_BUSY), "MCP on: an AI assistant is capturing");
    }
    else {
        _button->setIcon(dot_icon(kGreen, !connected()));
        tip = L_S(STR_PAGE_TOOLBAR, S_ID(IDS_TOOLBAR_MCP_TIP_ON),
                  "MCP on: AI assistants can capture while Logic Analyze is open");
    }
    tip = tip.toHtmlEscaped();
    if (!_error.isEmpty())
        tip += "<br>" + QString(kErrorMark) + _error.toHtmlEscaped();
    _button->setToolTip(tip);
}

QWidget *McpBridge::show_pane()
{
    if (_pane) {
        _pane->show();
        _pane->raise();
        _pane->activateWindow();
        return _pane;
    }
    // A tool window: never modal, so it neither blocks the app nor quitting.
    dialogs::DSDialog *dlg = new dialogs::DSDialog(_window, true, false);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setModal(false);
    dlg->setWindowModality(Qt::NonModal);
    dlg->setTitle(L_S(STR_PAGE_DLG, S_ID(IDS_DLG_MCP_TITLE), "MCP"));
    dlg->setMinimumSize(480, 200);

    QWidget *panel = new QWidget(dlg);
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
    QLabel *now = new QLabel();
    now->setWordWrap(true);
    QPushButton *toggle = new QPushButton();
    QPushButton *stop = new QPushButton(L_S(STR_PAGE_DLG, S_ID(IDS_DLG_MCP_STOP), "Stop"));
    grid->addWidget(new QLabel(L_S(STR_PAGE_DLG, S_ID(IDS_DLG_MCP_STATUS), "MCP:")), 0, 0, Qt::AlignLeft | Qt::AlignTop);
    grid->addWidget(agent, 0, 1);
    grid->addWidget(toggle, 0, 2);
    grid->addWidget(new QLabel(L_S(STR_PAGE_DLG, S_ID(IDS_DLG_MCP_NOW), "Now:")), 1, 0, Qt::AlignLeft | Qt::AlignTop);
    grid->addWidget(now, 1, 1);
    grid->addWidget(stop, 1, 2);
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
        QStringList a = _capture->activity();
        if (a.isEmpty())
            now->setText(L_S(STR_PAGE_DLG, S_ID(IDS_DLG_MCP_IDLE), "Idle"));
        else
            now->setText(QString(L_S(STR_PAGE_DLG, S_ID(IDS_DLG_MCP_CAPTURING), "Capturing: ch %1, %2, %3 samples"))
                             .arg(a.value(0), a.value(1), a.value(2)));
        stop->setEnabled(!a.isEmpty());
        err->setText(last_error().isEmpty() ? QString() : QString(kErrorMark) + last_error().toHtmlEscaped());
        err->setVisible(!last_error().isEmpty());
    };
    refresh();

    connect(this, &McpBridge::changed, panel, refresh);
    connect(toggle, &QPushButton::clicked, panel, [this]() { set_enabled(!enabled()); });
    connect(stop, &QPushButton::clicked, panel, [this]() { _capture->user_stop(); });

    dlg->layout()->addWidget(panel);
    dlg->show();
    _pane = dlg;
    return dlg;
}

} // namespace mcp
} // namespace pv
