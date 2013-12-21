/*
    Copyright 2013 Denis Kuplyakov <dener.kup@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef COLORSCHEME_H
#define COLORSCHEME_H

// Qt
#include <QDeclarativeItem>
#include <QColor>

// KDE
#include <KColorScheme>


class ColorScheme : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QColor background READ background NOTIFY placeHolder)
    Q_PROPERTY(QColor foreground READ foreground NOTIFY placeHolder)
    Q_PROPERTY(QColor border READ border NOTIFY placeHolder)

public:
    ColorScheme(QDeclarativeItem *parent = 0);

    QColor background() const;
    QColor foreground() const;
    QColor border() const;
    
signals:
    void placeHolder();
};

#endif // COLORSCHEME_H
