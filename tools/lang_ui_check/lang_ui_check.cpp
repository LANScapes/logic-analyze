/*
 * Language layout check for the main window (test builds only).
 *
 * Builds the main window offscreen with the Demo Device, then for each
 * language (and the test pseudo-language) switches the language at run time,
 * the way the Help > Language menu does, and checks the toolbar in both
 * orientations, the docks and the main dialogs:
 *   - a QLabel, QAbstractButton or QComboBox item whose text is wider than the
 *     room it has (text width from QFontMetrics; a button's room is its width
 *     less the chrome its size hint adds around the text);
 *   - a toolbar that pushes buttons into its ">>" extension menu.
 * Each widget is saved as <out>/<lang>/<widget>.png; <out>/report.txt lists the
 * problems and <out>/sheets/<widget>.html shows a failing widget across all
 * languages. In the pseudo-language, a visible text without its brackets did
 * not come through L_S(); those are listed too.
 *
 * Exit status 1 when a real language overflows; pseudo-language findings are
 * reported but do not fail the run.
 *
 * Usage: lang_ui_check [out-dir] [--shots dir]
 *   --shots also saves clean pictures for the manual per language, named as
 *   its figures: main-window, device-options, capture-mode-menu, decoder-dock,
 *   stage-trigger-panel and serial-trigger-panel (.png).
 * QT_QPA_PLATFORM defaults to offscreen.
 */

#include <QApplication>
#include <QAbstractButton>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QDockWidget>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QMenu>
#include <QTabWidget>
#include <QMainWindow>
#include <QRegularExpression>
#include <QSettings>
#include <QStyleOptionComboBox>
#include <QTextStream>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <functional>
#include <map>
#include <set>
#include <unistd.h>

#include "DSView/mystyle.h"
#include "DSView/pv/appcontrol.h"
#include "DSView/pv/config/appconfig.h"
#include "DSView/pv/dialogs/applicationpardlg.h"
#include "DSView/pv/dialogs/deviceoptions.h"
#include "DSView/pv/log.h"
#include "DSView/pv/mainframe.h"
#include "DSView/pv/mainwindow.h"
#include "DSView/pv/sigsession.h"
#include "DSView/pv/ui/langresource.h"

namespace {

// The app's style, without the accelerator underlines macOS never draws.
class CheckStyle : public MyStyle
{
public:
    int styleHint(StyleHint hint, const QStyleOption *option = nullptr, const QWidget *widget = nullptr,
                  QStyleHintReturn *ret = nullptr) const override
    {
        if (hint == QStyle::SH_UnderlineShortcut)
            return 0;
        return MyStyle::styleHint(hint, option, widget, ret);
    }
};

struct Issue {
    QString lang, widget, kind, key, text;
    int over;
};

QString g_out;
QString g_lang;      // folder name of the language being checked
int g_lang_id = 0;
std::vector<Issue> g_issues;
std::map<QString, QStringList> g_keys; // displayed text -> string ids
std::set<QString> g_failing;           // widgets with an issue in any language

QString plain(QString s)
{
    s.remove(QRegularExpression("\\(&\\w\\)"));
    s.replace("&&", "\x01");
    s.remove('&');
    s.replace("\x01", "&");
    return s.trimmed();
}

void wait(int ms)
{
    QEventLoop loop;
    QTimer::singleShot(ms, &loop, &QEventLoop::quit);
    loop.exec();
}

// The string ids behind each text of the current language, to name the key in reports.
void load_keys(const lang_key_item *lang)
{
    g_keys.clear();
    QDir dir(GetAppDataDir() + "/lang/" + lang->name);
    QStringList files;
    for (const QString &f : dir.entryList({"*.json"}, QDir::Files))
        files << dir.filePath(f);
    QDir dec(dir.filePath("dec"));
    for (const QString &f : dec.entryList({"*.json"}, QDir::Files))
        files << dec.filePath(f);
    for (const QString &path : files){
        QFile f(path);
        if (!f.open(QFile::ReadOnly))
            continue;
        for (const QJsonValue &v : QJsonDocument::fromJson(f.readAll()).array()){
            QJsonObject o = v.toObject();
            QString text = o["text"].toString();
#ifdef LANSCAPES_BRAND
            text.replace("DSView", BRAND_APP_NAME);
#endif
#ifdef LANG_PSEUDO
            if (lang->id == LAN_PSEUDO)
                text = LangResource::pseudo(text);
#endif
            g_keys[plain(text)] << o["id"].toString();
        }
    }
}

QString key_of(const QString &text)
{
    auto it = g_keys.find(plain(text));
    return it == g_keys.end() ? QString("-") : it->second.join(",");
}

void add(const QString &widget, const QString &kind, const QString &text, int over)
{
    g_issues.push_back({g_lang, widget, kind, key_of(text), plain(text), over});
    g_failing.insert(widget);
}

int text_width(const QFontMetrics &fm, const QString &text)
{
    int w = 0;
    for (const QString &line : plain(text).split('\n'))
        w = std::max(w, fm.horizontalAdvance(line));
    return w;
}

bool hard_coded(const QString &text)
{
    // In the pseudo-language every L_S() text is in brackets.
    QString t = plain(text);
    return t.count(QRegularExpression("[A-Za-z]")) >= 3 && !t.contains('[');
}

void check(QWidget *root, const QString &name)
{
    QList<QWidget*> all = root->findChildren<QWidget*>();
    all.prepend(root);
    for (QWidget *w : all){
        if (w != root && !w->isVisibleTo(root))
            continue;
        QFontMetrics fm(w->font());

        if (auto *l = qobject_cast<QLabel*>(w)){
            QString t = l->text();
            if (t.isEmpty() || l->wordWrap() || Qt::mightBeRichText(t))
                continue;
            int room = l->contentsRect().width() - 2 * l->margin();
            int need = text_width(fm, t);
            if (need > room + 1)
                add(name, "label", t, need - room);
            if (g_lang_id == LAN_PSEUDO && hard_coded(t))
                add(name, "not via L_S", t, 0);
        }
        else if (auto *b = qobject_cast<QAbstractButton*>(w)){
            QString t = b->text();
            if (t.isEmpty() || qobject_cast<QToolButton*>(b) && b->objectName() == "qt_toolbar_ext_button")
                continue;
            // The size hint is the text plus the button's chrome around it.
            int over = b->sizeHint().width() - b->width();
            if (over > 1)
                add(name, "button", t, over);
            // Standard dialog buttons come from Qt's own translations.
            if (g_lang_id == LAN_PSEUDO && hard_coded(t) && !qobject_cast<QDialogButtonBox*>(b->parentWidget()))
                add(name, "not via L_S", t, 0);
        }
        else if (auto *c = qobject_cast<QComboBox*>(w)){
            QStyleOptionComboBox opt;
            opt.initFrom(c);
            opt.editable = c->isEditable();
            int room = c->style()->subControlRect(QStyle::CC_ComboBox, &opt,
                                                  QStyle::SC_ComboBoxEditField, c).width();
            for (int i = 0; i < c->count(); i++){
                int need = fm.horizontalAdvance(c->itemText(i));
                if (need > room + 1)
                    add(name, "combo item", c->itemText(i), need - room);
            }
        }
        // A toolbar that is too short puts its last buttons in the ">>" menu.
        if (auto *tb = qobject_cast<QToolBar*>(w)){
            for (QAction *a : tb->actions()){
                QWidget *aw = tb->widgetForAction(a);
                if (a->isVisible() && aw && !aw->isVisible()){
                    QString what = a->text().isEmpty() ? aw->metaObject()->className() : a->text();
                    if (auto *b = qobject_cast<QAbstractButton*>(aw))
                        what = b->text();
                    add(name, "in toolbar >> menu", what, 0);
                }
            }
        }
    }
    QDir().mkpath(g_out + "/" + g_lang);
    root->grab().save(g_out + "/" + g_lang + "/" + name + ".png");
}

// Opens a modal dialog with open(), checks it while it is up, then closes it.
void check_modal(const QString &name, const std::function<void()> &open)
{
    QTimer::singleShot(800, [name](){
        QWidget *m = QApplication::activeModalWidget();
        if (m == nullptr){
            add(name, "dialog did not open", name, 0);
            return;
        }
        check(m, name);
        if (auto *d = qobject_cast<QDialog*>(m))
            d->reject();
        else
            m->close();
    });
    open();
}

// --shots: clean pictures for the manual, named as its figures are.
void save_shots(const QString &dir, QWidget *frame, pv::MainWindow *mw)
{
    QString out = dir + "/" + g_lang + "/";
    QDir().mkpath(out);
    frame->grab().save(out + "main-window.png");

    if (auto *dock = mw->findChild<QDockWidget*>("protocol_dock")){
        bool shown = dock->isVisible();
        dock->show();
        wait(200);
        dock->grab().save(out + "decoder-dock.png");
        dock->setVisible(shown);
    }
    if (auto *dock = mw->findChild<QDockWidget*>("trigger_dock")){
        bool shown = dock->isVisible();
        dock->show();
        QTabWidget *tabs = dock->findChild<QTabWidget*>();
        if (tabs){
            bool enabled = tabs->isEnabled();
            tabs->setEnabled(true); // the tabs are greyed until advanced trigger is on
            const char *names[] = {"stage-trigger-panel.png", "serial-trigger-panel.png"};
            for (int i = 0; i < 2 && i < tabs->count(); i++){
                tabs->setCurrentIndex(i);
                wait(200);
                dock->grab().save(out + names[i]);
            }
            tabs->setCurrentIndex(0);
            tabs->setEnabled(enabled);
        }
        dock->setVisible(shown);
    }
    // The capture mode menu: the toolbar menu whose first item is Single.
    QString single = plain(L_S(STR_PAGE_TOOLBAR, S_ID(IDS_TOOLBAR_CAPTURE_MODE_SINGLE), "&Single"));
    for (QMenu *menu : mw->findChildren<QMenu*>()){
        if (!menu->actions().isEmpty() && plain(menu->actions().first()->text()) == single){
            menu->ensurePolished();
            menu->adjustSize();
            menu->grab().save(out + "capture-mode-menu.png");
            break;
        }
    }
}

void write_sheets(const std::vector<const lang_key_item*> &langs)
{
    QDir().mkpath(g_out + "/sheets");
    for (const QString &widget : g_failing){
        QFile f(g_out + "/sheets/" + widget + ".html");
        f.open(QFile::WriteOnly | QFile::Truncate);
        QTextStream s(&f);
        s << "<!doctype html><meta charset=utf-8><title>" << widget << "</title>"
          << "<style>body{font:13px sans-serif;background:#222;color:#ddd}"
          << "img{display:block;max-width:100%;border:1px solid #555}"
          << "h2{margin:18px 0 4px}li{color:#f88}</style><h1>" << widget << "</h1>";
        for (const lang_key_item *lang : langs){
            s << "<h2>" << lang->native << " (" << lang->name << ")</h2><ul>";
            for (const Issue &i : g_issues)
                if (i.widget == widget && i.lang == QString(lang->id == LAN_PSEUDO ? "pseudo" : lang->name))
                    s << "<li>" << i.kind << ": " << i.key << " \"" << i.text.toHtmlEscaped()
                      << "\" " << i.over << " px over</li>";
            s << "</ul><img src=\"../" << (lang->id == LAN_PSEUDO ? "pseudo" : lang->name)
              << "/" << widget << ".png\">";
        }
    }
}

} // namespace

int main(int argc, char *argv[])
{
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM"))
        qputenv("QT_QPA_PLATFORM", "offscreen");
#ifdef Q_OS_MACOS
    // The offscreen platform sets fonts at 96 dpi and in a generic face; the
    // app on macOS has 72 dpi and the system face, so text has its real width.
    if (qEnvironmentVariableIsEmpty("QT_FONT_DPI"))
        qputenv("QT_FONT_DPI", "72");
#endif
    QApplication a(argc, argv);
#ifdef Q_OS_MACOS
    if (QGuiApplication::platformName() == "offscreen")
        QApplication::setFont(QFont(".AppleSystemUIFont", 13));
#endif
    a.setStyle(new CheckStyle);
    // Own settings, so the check neither reads nor changes the user's.
    QApplication::setOrganizationName("Lanscapes");
    QApplication::setApplicationName("LogicAnalyzeLangCheck");
    QString shots;
    QStringList args;
    for (int i = 1; i < argc; i++){
        if (QString(argv[i]) == "--shots" && i + 1 < argc)
            shots = QDir(argv[++i]).absolutePath();
        else
            args << argv[i];
    }
    g_out = QDir(args.isEmpty() ? "lang-ui-check" : args.first()).absolutePath();
    QDir(g_out).removeRecursively();

    dsv_log_init();
    dsv_log_level(XLOG_LEVEL_ERR);
    AppConfig &app = AppConfig::Instance();
    app.LoadAll();
    app.frameOptions.language = LAN_EN;
    LangResource::Instance()->Load(LAN_EN);

    AppControl *control = AppControl::Instance();
    if (!control->Init()){
        fprintf(stderr, "init failed\n");
        return 2;
    }
    pv::MainFrame frame;
    control->Start();
    frame.resize(1087, 735);
    frame.show();
    wait(1500);

    // The Demo Device; a connected analyzer is never used.
    pv::SigSession *session = control->GetSession();
    struct ds_device_base_info *list = NULL;
    int count = 0;
    if (ds_get_device_list(&list, &count) == SR_OK){
        for (int i = 0; i < count; i++)
            if (QString(list[i].name).contains("Demo"))
                session->set_device(list[i].handle);
        g_free(list);
    }
    wait(1500);

    auto *mw = frame.findChild<pv::MainWindow*>();
    auto *toolbar = mw->findChild<QToolBar*>("main_toolbar");
    QObject *logobar = nullptr;
    for (QToolBar *tb : mw->findChildren<QToolBar*>())
        if (tb->metaObject()->className() == QString("pv::toolbars::LogoBar"))
            logobar = tb;

    std::vector<const lang_key_item*> langs;
    for (const lang_key_item &l : lang_id_keys)
        langs.push_back(&l);

    for (const lang_key_item *lang : langs){
        g_lang_id = lang->id;
        g_lang = lang->id == LAN_PSEUDO ? "pseudo" : lang->name;
        load_keys(lang);
        mw->switchLanguage(lang->id);
        wait(300);

        mw->addToolBar(Qt::TopToolBarArea, toolbar);
        wait(300);
        check(toolbar, "toolbar_horizontal");
        mw->addToolBar(Qt::LeftToolBarArea, toolbar);
        wait(300);
        check(toolbar, "toolbar_vertical");
        mw->addToolBar(Qt::TopToolBarArea, toolbar);
        wait(200);

        for (QDockWidget *dock : mw->findChildren<QDockWidget*>()){
            if (dock->widget() == nullptr)
                continue;
            bool shown = dock->isVisible();
            dock->show();
            wait(200);
            check(dock->widget(), "dock_" + dock->objectName().replace(" ", "_").toLower());
            dock->setVisible(shown);
        }

        check_modal("dialog_device_options", [mw](){
            pv::dialogs::DeviceOptions dlg(mw);
            dlg.exec();
        });
        if (!shots.isEmpty() && lang->id != LAN_PSEUDO){
            QDir().mkpath(shots + "/" + g_lang);
            QFile::copy(g_out + "/" + g_lang + "/dialog_device_options.png",
                        shots + "/" + g_lang + "/device-options.png");
            save_shots(shots, &frame, mw);
        }
        check_modal("dialog_display_options", [mw](){
            pv::dialogs::ApplicationParamDlg dlg;
            dlg.ShowDlg(mw);
        });
        check_modal("dialog_log_options", [logobar](){
            QMetaObject::invokeMethod(logobar, "on_action_setting_log");
        });
    }

    write_sheets(langs);

    int failures = 0;
    QFile f(g_out + "/report.txt");
    f.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream report(&f);
    for (const Issue &i : g_issues){
        QString line = QString("%1\t%2\t%3\t%4\t\"%5\"\t%6 px over")
                           .arg(i.lang, i.widget, i.kind, i.key, i.text).arg(i.over);
        report << line << "\n";
        if (i.lang != "pseudo"){
            failures++;
            fprintf(stdout, "OVERFLOW %s\n", line.toUtf8().constData());
        }
    }
    report.flush();
    f.close();
    int pseudo = (int)g_issues.size() - failures;
    fprintf(stdout, "%d languages, %d overflows; pseudo-language: %d findings (see %s/report.txt)\n",
            (int)langs.size(), failures, pseudo, g_out.toUtf8().constData());
    fflush(stdout);

    QSettings(QApplication::organizationName(), "LogicAnalyzeLangCheck").clear();
    if (GetUserDataDir().endsWith("/LogicAnalyzeLangCheck"))
        QDir(GetUserDataDir()).removeRecursively(); // the check's own profile folder
    _exit(failures ? 1 : 0); // skip teardown of the device and decoder threads
}
