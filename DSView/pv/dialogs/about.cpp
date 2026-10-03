/*
 * This file is part of the DSView project.
 * DSView is based on PulseView.
 *
 * Copyright (C) 2012 Joel Holdsworth <joel@airwebreathe.org.uk>
 * Copyright (C) 2013 DreamSourceLab <support@dreamsourcelab.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


#include "about.h"

#include <QPixmap>
#include <QApplication>
#include <QTextBrowser>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QScrollBar>
  
#include "../config/appconfig.h"
#include "config.h"
#include <QUrl>
#include "../dsvdef.h"
#include "../utility/encoding.h"
#include "../ui/langresource.h"

namespace pv {
namespace dialogs {

About::About(QWidget *parent) :
    DSDialog(parent, true)
{
    setFixedHeight(600);
    setFixedWidth(800);

    #if defined(__x86_64__) || defined(_M_X64)
        QString arch = "x64";
    #elif defined(__i386) || defined(_M_IX86)
        QString arch = "x86";
    #elif defined(__arm__) || defined(_M_ARM)
        QString arch = "arm";
    #elif defined(__aarch64__)
        QString arch = "arm64";
    #else
        QString arch = "other";
    #endif

#ifdef LANSCAPES_BRAND
    QString version = QString("<font size=24>%1 %2 (%3)</font><br />%4<br /><br />")
                      .arg(QApplication::applicationName())
                      .arg(QApplication::applicationVersion())
                      .arg(arch)
                      .arg(BRAND_ORG_NAME);

    QString url = QString("Source code: <a href=\"%1\" style=\"color:#C0C0C0\">%1</a><br />"
                          "Releases and issues: <a href=\"%1/releases\" style=\"color:#C0C0C0\">%1/releases</a><br />"
                          "Support and source requests: %2<br />"
                          "<br />")
                  .arg(BRAND_REPO_URL, BRAND_SUPPORT_EMAIL);

    QString thanks = QString(
        "<font size=16>License</font><br />"
#ifdef LANSCAPES_APPSTORE
        "The %1 analyzer (everything in this app except the MCP server described below) "
        "is free software: you can redistribute it and/or modify it under the terms of the "
#else
        "%1 is free software: you can redistribute it and/or modify it under the terms of the "
#endif
        "GNU General Public License, version 3 or (at your option) any later version. "
        "It is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY. "
        "The complete source code is available at the address above.<br /><br />"
        "Based on <a href=\"https://github.com/DreamSourceLab/DSView\" style=\"color:#C0C0C0\">DSView %2</a> "
        "by DreamSourceLab, which is based on PulseView and the "
        "<a href=\"https://sigrok.org/\" style=\"color:#C0C0C0\">sigrok project</a>.<br />"
        "Device firmware &copy; DreamSourceLab, MIT license (see licenses in the app bundle).<br /><br />"
        "Copyright &copy; 2026 %3 (changes and additions).<br />"
        "Copyright &copy; DreamSourceLab and the sigrok and PulseView contributors.<br /><br />"
        "Uses Qt &copy; The Qt Company Ltd. and other contributors, under the GNU LGPL "
        "version 3; Python &copy; Python Software Foundation, under the PSF License; and the "
        "other libraries listed in the third-party notices below, each under its own license. "
        "The notices say where to get the source of each LGPL library and how to replace it.<br /><br />"
        )
        .arg(QApplication::applicationName())
        .arg(DS_VERSION_STRING)
        .arg(BRAND_ORG_NAME);

    // MCP integration: proprietary and paid; only the App Store edition contains it.
#ifdef LANSCAPES_APPSTORE
    thanks += QString(
        "<font size=16>MCP server</font><br />"
        "This edition includes the %1 MCP server, which lets AI assistants such as Claude "
        "capture, measure and decode signals through the analyzer. The MCP server is "
        "proprietary software &copy; 2026 %2. It is NOT free software and is NOT covered "
        "by the GPL: it is licensed to the purchaser of this App Store edition under the "
        "%1 End User License Agreement, and may not be copied, redistributed or used "
        "outside %1.<br /><br />")
        .arg(QApplication::applicationName()).arg(BRAND_ORG_NAME);
#else
    thanks += QString(
        "<font size=16>MCP server</font><br />"
        "The %1 MCP server, which lets AI assistants capture, measure and decode signals "
        "through the analyzer, is proprietary, paid software &copy; 2026 %2. It is not "
        "part of this free edition and is available only in the %1 edition on the "
        "Mac App Store.<br /><br />")
        .arg(QApplication::applicationName()).arg(BRAND_ORG_NAME);
#endif

    // Full license texts shipped in the bundle.
    QString lic = GetAppDataDir() + "/licenses/";
    auto link = [&](const QString &file, const QString &label) {
        return QString("<a href=\"%1\" style=\"color:#C0C0C0\">%2</a><br />")
            .arg(QUrl::fromLocalFile(lic + file).toString(QUrl::FullyEncoded).toHtmlEscaped(), label.toHtmlEscaped());
    };
    thanks += "<font size=16>License texts</font><br />";
    thanks += link("GPL-3.0.txt", "GNU General Public License, version 3");
    thanks += link("DreamSourceLab-firmware-MIT.txt", "DreamSourceLab firmware license (MIT)");
    thanks += link("THIRD-PARTY-NOTICES.txt", "Third-party notices (Qt, Python and other bundled libraries)");
#ifdef LANSCAPES_APPSTORE
    thanks += link("EULA.txt", QApplication::applicationName() + " End User License Agreement");
#endif
    thanks += "<br /><br />";

    QString changlogs;
#else
    QString version = tr("<font size=24>DSView %1 (%2)</font><br />")
                      .arg(QApplication::applicationVersion())
                      .arg(arch);

    QString site_url = QApplication::organizationDomain();
    if (site_url.startsWith("http") == false){
        site_url = "https://" + site_url;
    }

    QString url = tr("Website: <a href=\"%1\" style=\"color:#C0C0C0\">%1</a><br />"
                     "Github: <a href=\"%2\" style=\"color:#C0C0C0\">%2</a><br />"
                     "Copyright: <label href=\"#\" style=\"color:#C0C0C0\">%3</label><br />"
                     "<br /><br />")
                  .arg(site_url)
                  .arg("https://github.com/DreamSourceLab/DSView")
                  .arg(tr("© DreamSourceLab. All rights reserved."));

    QString thanks = tr("<font size=16>Special Thanks</font><br />"
                        "<a href=\"%1\" style=\"color:#C0C0C0\">All backers on kickstarter</a><br />"
                        "<a href=\"%2\" style=\"color:#C0C0C0\">All members of Sigrok project</a><br />"
                        "All contributors of all open-source projects</a><br />"
                        "<br /><br />")
                        .arg("https://www.kickstarter.com/projects/dreamsourcelab/dslogic-multifunction-instruments-for-everyone")
                        .arg("http://sigrok.org/");

    QString changlogs = tr("<font size=16>Changelogs</font><br />");

    QDir dir(GetAppDataDir());
    AppConfig &app = AppConfig::Instance(); 
    int lan = app.frameOptions.language;
    if (lan != LAN_CN)
        lan = LAN_EN; // the change log exists in Chinese and English only

    QString filename = dir.absolutePath() + "/NEWS" + QString::number(lan);
    QFile news(filename);
    if (news.open(QIODevice::ReadOnly)) {
   
        QTextStream stream(&news);
        encoding::set_utf8(stream);

        QString line;
        while (!stream.atEnd()){
            line = stream.readLine();
            changlogs += line + "<br />";
        }
    }
    news.close();    

#endif

#ifndef LANSCAPES_BRAND
    QPixmap pix(":/icons/dsl_logo.svg");
    QImage logo = pix.toImage();
#endif

    QTextBrowser *about = new QTextBrowser(this);
    about->setOpenExternalLinks(true);
    about->setFrameStyle(QFrame::NoFrame);
    QTextCursor cur = about->textCursor();
#ifndef LANSCAPES_BRAND
    cur.insertImage(logo);
    cur.insertHtml("<br /><br /><br />");
#else
    QImage icon(GetAppDataDir() + "/about-icon.png");
    if (!icon.isNull()) {
        cur.insertImage(icon.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        cur.insertHtml("<br /><br />");
    }
#endif
    cur.insertHtml(version+url+thanks+changlogs);
    about->moveCursor(QTextCursor::Start);

    QVBoxLayout *xlayout = new QVBoxLayout();
    xlayout->addWidget(about);

    layout()->addLayout(xlayout);
    setTitle(L_S(STR_PAGE_DLG, S_ID(IDS_DLG_ABOUT), "About"));
}

About::~About()
{
}

} // namespace dialogs
} // namespace pv
