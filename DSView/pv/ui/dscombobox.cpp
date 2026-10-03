/*
 * This file is part of the DSView project.
 * DSView is based on PulseView.
 * 
 * Copyright (C) 2021 DreamSourceLab <support@dreamsourcelab.com>
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

#include "dscombobox.h"
#include <QToolBar>
#ifdef LANSCAPES_BRAND
#include <QAbstractItemView>
#include <QPainter>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#endif
#include <QFontMetrics>
#include <QString>
#include <QGuiApplication>
#include <QScreen>
#include "../config/appconfig.h"

DsComboBox::DsComboBox(QWidget *parent) 
    :QComboBox(parent)
{
    _bPopup = false;
    QComboBox::setSizeAdjustPolicy(QComboBox::AdjustToContents);   
}

#ifdef LANSCAPES_BRAND
namespace
{
    // Draws a combo list entry as a right-aligned number then its unit. The list's
    // own delegate still draws the row (highlight, check mark) from a blank copy
    // of the entry, so the theme is kept; only the text is placed here.
    class NumberColumnDelegate : public QStyledItemDelegate
    {
    public:
        NumberColumnDelegate(QComboBox *combo, QAbstractItemDelegate *base)
            : QStyledItemDelegate(combo), _combo(combo), _base(base), _blank(this) {}

        void paint(QPainter *painter, const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override
        {
            if (_blank.rowCount() != index.model()->rowCount())
                _blank.setRowCount(index.model()->rowCount());
            _blank.setColumnCount(1);
            _base->paint(painter, option, _blank.index(index.row(), 0));

            QString num, unit;
            split(index.data(Qt::DisplayRole).toString(), num, unit);
            const QFontMetrics fm(_combo->font());
            const int col = number_column(fm);
            // The menu style starts the text after the icon column (decoration + 4).
            QRect r = option.rect.adjusted(option.decorationSize.width() + 4, 0, 0, 0);
            const bool selected = option.state & QStyle::State_Selected;
            painter->save();
            painter->setFont(_combo->font());
            painter->setPen(option.palette.color(selected ? QPalette::HighlightedText : QPalette::Text));
            painter->drawText(QRect(r.left(), r.top(), col, r.height()),
                              Qt::AlignRight | Qt::AlignVCenter, num);
            painter->drawText(QRect(r.left() + col, r.top(), r.width() - col, r.height()),
                              Qt::AlignLeft | Qt::AlignVCenter, unit);
            painter->restore();
        }

        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
        {
            QString num, unit;
            split(index.data(Qt::DisplayRole).toString(), num, unit);
            const QFontMetrics fm(_combo->font());
            QSize s = _base->sizeHint(option, index);
            s.rwidth() += number_column(fm) - fm.horizontalAdvance(num);
            return s;
        }

    private:
        static void split(const QString &text, QString &num, QString &unit)
        {
            const int sp = text.indexOf(' ');
            num = sp < 0 ? text : text.left(sp);
            unit = sp < 0 ? QString() : text.mid(sp);
        }

        int number_column(const QFontMetrics &fm) const
        {
            int w = 0;
            for (int i = 0; i < _combo->count(); i++){
                QString num, unit;
                split(_combo->itemText(i), num, unit);
                w = qMax(w, fm.horizontalAdvance(num));
            }
            return w;
        }

        QComboBox *_combo;
        QAbstractItemDelegate *_base;
        mutable QStandardItemModel _blank;
    };
}

void DsComboBox::alignNumbersInList()
{
    view()->setItemDelegate(new NumberColumnDelegate(this, view()->itemDelegate()));
}

int DsComboBox::chevron_room() const
{
    for (QWidget *w = parentWidget(); w != nullptr; w = w->parentWidget()){
        if (qobject_cast<QToolBar*>(w))
            return 16;
    }
    return 0;
}

QSize DsComboBox::sizeHint() const
{
    return QComboBox::sizeHint() + QSize(chevron_room(), 0);
}

QSize DsComboBox::minimumSizeHint() const
{
    return QComboBox::minimumSizeHint() + QSize(chevron_room(), 0);
}
#endif

DsComboBox::~DsComboBox()
{

}

void DsComboBox::measureSize()
{
    int num = this->count();
    int maxWidth = 0;
    int height = 30;
    QFontMetrics fm = this->fontMetrics();

    for (int i=0; i<num; i++){
        QString text = this->itemText(i);
        QRect rc = fm.boundingRect(text);
 
        if (rc.width() > maxWidth){
            maxWidth = rc.width();
        }
        height = rc.height();
    }

    QString style = QString("QAbstractItemView{min-width:%1px; min-height:%2px;}")
                .arg(maxWidth + 30)
                .arg(height + 5);
    this->setStyleSheet(style);
}

void DsComboBox::showPopup()
{
    _bPopup = true;

#ifdef Q_OS_DARWIN

    measureSize();
    QComboBox::showPopup();

    QWidget *popup = this->findChild<QFrame*>();
    auto rc = popup->geometry();
    int x = rc.left() + 6;
    int y = rc.top();
    int w = rc.right() - rc.left();
#ifdef LANSCAPES_BRAND
    // Fit the list to its rows: a fixed 20 px more than Qt's height showed as an
    // empty row under a short list (the device modes).
    int rows_h = 0;
    for (int i = 0; i < count(); i++)
        rows_h += view()->sizeHintForRow(i);
    int h = qMin(rc.bottom() - rc.top() + 20,
                 popup->height() - view()->viewport()->height() + rows_h);
#else
    int h = rc.bottom() - rc.top() + 20;
#endif
    popup->setGeometry(x, y, w, h);

    int sy = QGuiApplication::primaryScreen()->size().height(); 
    if (sy <= 1080){
        popup->setMaximumHeight(750); 
    }

    popup->setStyleSheet("background-color:" + AppConfig::Instance().GetStyleColor().name());

#else
    QComboBox::showPopup();
#endif

}

void DsComboBox::hidePopup()
{
    QComboBox::hidePopup();
    _bPopup = false;
}
 