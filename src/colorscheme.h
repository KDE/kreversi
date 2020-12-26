/*
    SPDX-FileCopyrightText: 2013 Denis Kuplyakov <dener.kup@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

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
    explicit ColorScheme(QQuickItem *parent = nullptr);
    
    QColor background() const;
    QColor foreground() const;
    QColor border() const;
    
Q_SIGNALS:
    void placeHolder();
};

#endif // COLORSCHEME_H
