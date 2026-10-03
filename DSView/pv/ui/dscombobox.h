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

#ifndef DSCOMBOBOX_H
#define DSCOMBOBOX_H

#include <QComboBox>
#include <QKeyEvent>


class DsComboBox : public QComboBox
{
public:
    explicit DsComboBox(QWidget *parent = nullptr);

    ~DsComboBox(); 

public:
    void showPopup() override;

    void hidePopup() override;

#ifdef LANSCAPES_BRAND
    // Lists "<number> <unit>" entries with the numbers right-aligned on their last
    // digit (the 1 of "1 MHz" under the 0 of "500 kHz").
    void alignNumbersInList();

    // In a toolbar the brand stylesheet sets the chevron into the right padding, and
    // the style counts its width against the text as well; reserve that room.
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
#endif

    inline bool  IsPopup(){
        return _bPopup;
    } 

private:
    void measureSize();
#ifdef LANSCAPES_BRAND
    int chevron_room() const;
#endif

private: 
    bool    _bPopup;
};


#endif // DSCOMBOBOX_H
