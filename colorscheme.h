/*******************************************************************
 *
 * Copyright 2013 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * This file is part of the KDE project "KReversi"
 *
 * KReversi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * KReversi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KReversi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 ********************************************************************/

#ifndef COLORSCHEME_H
#define COLORSCHEME_H

#include <QQuickItem>
#include <KColorScheme>
#include <QColor>

class ColorScheme : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QColor background READ background NOTIFY placeHolder)
    Q_PROPERTY(QColor foreground READ foreground NOTIFY placeHolder)
    Q_PROPERTY(QColor border READ border NOTIFY placeHolder)
public:
    ColorScheme(QQuickItem *parent = 0);
    
    QColor background() const;
    QColor foreground() const;
    QColor border() const;
    
signals:
    void placeHolder();
};

#endif // COLORSCHEME_H
