/*
 * Capture parity check (test builds of the Mac App Store edition only).
 *
 * dslcap stays as a developer tool only while it gives the same result as an
 * in-app MCP capture. This runs one request both ways on the Demo Device's
 * "protocol" pattern, which plays a fixed recording (DSView/demo/logic/
 * protocol.demo), so both runs see the same samples:
 *   1. dslcap --device Demo ... --out <out>/cli/parity
 *   2. the GUI's MCP capture path (McpCapture, offscreen) into <out>/gui/parity
 * and compares the JSON records (all fields but elapsed_s and bin) and the .bin
 * files byte for byte.
 *
 * Usage: capture_parity <dslcap> <res-dir> <out-dir>
 * Exit status 0 when both agree, 1 when they differ, 2 when a run fails.
 * QT_QPA_PLATFORM defaults to offscreen.
 */

#include <QApplication>
#include <QDialog>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QSettings>
#include <QTimer>
#include <cstdio>
#include <unistd.h>

#include "DSView/pv/appcontrol.h"
#include "DSView/pv/config/appconfig.h"
#include "DSView/pv/deviceagent.h"
#include "DSView/pv/log.h"
#include "DSView/pv/mainframe.h"
#include "DSView/pv/mcp/mcpbridge.h"
#include "DSView/pv/mcp/mcpcapture.h"
#include "DSView/pv/sigsession.h"
#include "DSView/pv/ui/langresource.h"

namespace {

// The request: 7 channels of the recording, 100000 of its 131072 samples at
// its 25 MHz, a trigger at 50 % (the demo device sends no trigger itself).
const int kChannels[] = {0, 1, 5, 9, 12, 13, 14};
const qint64 kRate = 25000000;
const qint64 kSamples = 100000;

void wait(int ms)
{
    QEventLoop loop;
    QTimer::singleShot(ms, &loop, &QEventLoop::quit);
    loop.exec();
}

int quit(int rc)
{
    QSettings(QApplication::organizationName(), QApplication::applicationName()).clear();
    fflush(stdout);
    _exit(rc); // skip teardown of the device and decoder threads
}

QJsonObject without_run_fields(QJsonObject o)
{
    o.remove("elapsed_s");
    o.remove("bin");
    return o;
}

} // namespace

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "usage: capture_parity <dslcap> <res-dir> <out-dir>\n");
        return 2;
    }
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM"))
        qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication a(argc, argv);
    // Own settings, so the check neither reads nor changes the user's.
    QApplication::setOrganizationName("Lanscapes");
    QApplication::setApplicationName("LogicAnalyzeParityCheck");

    QString dslcap = argv[1], res = argv[2], out = QDir(argv[3]).absolutePath();
    QDir(out).removeRecursively();
    QDir().mkpath(out + "/cli");
    QDir().mkpath(out + "/gui");

    // 1. dslcap.
    QStringList chans;
    for (int c : kChannels)
        chans << QString::number(c);
    QProcess p;
    p.start(dslcap, {"--device", "Demo", "--res", res, "--channels", chans.join(","),
                     "--samplerate", QString::number(kRate), "--samples", QString::number(kSamples),
                     "--mode", "buffer", "--trigger", "0:R", "--trigpos", "50", "--timeout", "60",
                     "--log-level", "0", "--out", out + "/cli/parity"});
    if (!p.waitForFinished(90000)) {
        printf("capture_parity: dslcap did not finish\n");
        return 2;
    }
    QList<QByteArray> lines = p.readAllStandardOutput().trimmed().split('\n');
    QJsonObject cli = QJsonDocument::fromJson(lines.last()).object();
    if (p.exitCode() != 0 || cli.contains("error")) {
        printf("capture_parity: dslcap failed (%d): %s\n", p.exitCode(), lines.last().constData());
        return 2;
    }

    // 2. The GUI, on the Demo Device.
    dsv_log_init();
    dsv_log_level(XLOG_LEVEL_ERR);
    AppConfig &app = AppConfig::Instance();
    app.LoadAll();
    app.frameOptions.language = LAN_EN;
    LangResource::Instance()->Load(LAN_EN);
    AppControl *control = AppControl::Instance();
    // The Demo Device only: a connected analyzer is never scanned or opened.
    ds_set_no_hardware(1);
    if (!control->Init()) {
        printf("capture_parity: init failed\n");
        return 2;
    }
    pv::MainFrame frame;
    control->Start();
    frame.resize(1087, 735);
    QTimer dismiss;   // a message box at start (a busy analyzer) would wait forever
    QObject::connect(&dismiss, &QTimer::timeout, []() {
        if (auto *d = qobject_cast<QDialog*>(QApplication::activeModalWidget()))
            d->reject();
    });
    dismiss.start(200);
    frame.show();
    wait(1500);

    pv::SigSession *session = control->GetSession();
    struct ds_device_base_info *list = NULL;
    int count = 0;
    if (ds_get_device_list(&list, &count) == SR_OK) {
        for (int i = 0; i < count; i++)
            if (QString(list[i].name).contains("Demo"))
                session->set_device(list[i].handle);
        g_free(list);
    }
    wait(1500);
    dismiss.stop();
    if (!session->get_device()->is_demo() || session->get_device()->get_demo_operation_mode() != "protocol") {
        printf("capture_parity: the Demo Device is not on its protocol pattern (%s)\n",
               session->get_device()->get_demo_operation_mode().toUtf8().constData());
        return quit(2);
    }

    pv::mcp::McpBridge *bridge = pv::mcp::McpBridge::instance();
    pv::mcp::McpCapture *cap = bridge->capture();
    cap->set_device_name("Demo");
    pv::mcp::CaptureRequest req;
    for (int c : kChannels)
        req.channels.push_back(c);
    req.samplerate_hz = kRate;
    req.samples = kSamples;
    req.trigger_channel = 0;
    req.trigger_edge = 'R';
    req.trigger_position_percent = 50;
    req.timeout_ms = 60000;

    QEventLoop loop;
    QJsonObject gui;
    QString error;
    QObject::connect(cap, &pv::mcp::McpCapture::done, [&](qint64, const QString &, const QJsonObject &meta) {
        gui = meta;
        loop.quit();
    });
    QObject::connect(cap, &pv::mcp::McpCapture::failed, [&](qint64, const QString &code, const QString &message) {
        error = code + ": " + message;
        loop.quit();
    });
    QTimer::singleShot(90000, &loop, [&]() { error = "no answer in 90 s"; loop.quit(); });
    cap->start(1, "parity", req, out + "/gui");
    if (gui.isEmpty() && error.isEmpty())
        loop.exec();
    if (!error.isEmpty()) {
        printf("capture_parity: the GUI capture failed: %s\n", error.toUtf8().constData());
        return quit(2);
    }

    // 3. Compare.
    int differences = 0;
    QFile gui_json(out + "/gui/parity.json");
    if (!gui_json.open(QFile::ReadOnly) || QJsonDocument::fromJson(gui_json.readAll()).object() != gui) {
        printf("DIFFERENT gui/parity.json is missing or differs from capture_done's meta\n");
        differences++;
    }
    QJsonObject c = without_run_fields(cli), g = without_run_fields(gui);
    QStringList keys = c.keys() + g.keys();
    keys.removeDuplicates();
    for (const QString &k : keys) {
        if (c.value(k) != g.value(k)) {
            printf("DIFFERENT meta %s: dslcap %s, GUI %s\n", k.toUtf8().constData(),
                   QJsonDocument(QJsonObject{{k, c.value(k)}}).toJson(QJsonDocument::Compact).constData(),
                   QJsonDocument(QJsonObject{{k, g.value(k)}}).toJson(QJsonDocument::Compact).constData());
            differences++;
        }
    }
    QFile cb(out + "/cli/parity.bin"), gb(out + "/gui/parity.bin");
    if (!cb.open(QFile::ReadOnly) || !gb.open(QFile::ReadOnly)) {
        printf("DIFFERENT a .bin file is missing\n");
        differences++;
    }
    else {
        QByteArray x = cb.readAll(), y = gb.readAll();
        if (x != y) {
            printf("DIFFERENT .bin: dslcap %lld bytes, GUI %lld bytes\n", (long long)x.size(), (long long)y.size());
            differences++;
        }
        else
            printf("capture_parity: .bin identical, %lld bytes\n", (long long)x.size());
    }
    printf("capture_parity: %s (%lld samples x %d channels, meta: %s)\n",
           differences ? "DIFFERENT" : "identical", (long long)gui.value("samples").toVariant().toLongLong(),
           (int)(sizeof kChannels / sizeof kChannels[0]),
           QJsonDocument(g).toJson(QJsonDocument::Compact).constData());
    return quit(differences ? 1 : 0);
}
