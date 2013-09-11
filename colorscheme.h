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

#include <QDeclarativeItem>
#include <QColor>

class ColorScheme : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QColor background READ background)
    Q_PROPERTY(QColor foreground READ foreground)
    Q_PROPERTY(QColor border READ border)

public:
    ColorScheme(QDeclarativeItem *parent = 0);

    QColor background() const;
    QColor foreground() const;
    QColor border() const;
};

#endif // COLORSCHEME_H
