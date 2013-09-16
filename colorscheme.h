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
#include <KColorScheme>

class ColorScheme : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(KColorScheme::ColorSet colorSet
               READ getColorSet WRITE setColorSet)
    Q_PROPERTY(KColorScheme::BackgroundRole backgroundRole
               READ getBackgroundRole WRITE setBackgroundRole)
    Q_PROPERTY(KColorScheme::ForegroundRole foregroundRole
               READ getForegroundRole WRITE setForegroundRole)
    Q_PROPERTY(KColorScheme::DecorationRole decorationRole
               READ getDecorationRole WRITE setDecorationRole)
    Q_PROPERTY(KColorScheme::ShadeRole shadeRole
               READ getShadeRole WRITE setShadeRole)

    Q_PROPERTY(QColor background READ background NOTIFY onBackgroundChange)
    Q_PROPERTY(QColor foreground READ foreground NOTIFY onForegroundChange)
    Q_PROPERTY(QColor decoration READ decoration NOTIFY onDecorationChange)
    Q_PROPERTY(QColor shade READ shade NOTIFY onShadeChange)
public:
    explicit ColorScheme(QDeclarativeItem *parent = 0);

    KColorScheme::ColorSet getColorSet() const;
    void setColorSet(KColorScheme::ColorSet colorSet);

    KColorScheme::BackgroundRole getBackgroundRole() const;
    void setBackgroundRole(KColorScheme::BackgroundRole role);

    KColorScheme::ForegroundRole getForegroundRole() const;
    void setForegroundRole(KColorScheme::ForegroundRole role);

    KColorScheme::DecorationRole getDecorationRole() const;
    void setDecorationRole(KColorScheme::DecorationRole role);

    KColorScheme::ShadeRole getShadeRole() const;
    void setShadeRole(KColorScheme::ShadeRole role);

    QColor background() const;
    QColor foreground() const;
    QColor decoration() const;
    QColor shade() const;

signals:
    void onBackgroundChange();
    void onForegroundChange();
    void onDecorationChange();
    void onShadeChange();

private:
    KColorScheme::ColorSet m_colorSet;
    KColorScheme::BackgroundRole m_backgroundRole;
    KColorScheme::ForegroundRole m_foregroundRole;
    KColorScheme::DecorationRole m_decorationRole;
    KColorScheme::ShadeRole m_shadeRole;
};

#endif // COLORSCHEME_H
