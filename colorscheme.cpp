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
#include <colorscheme.h>

ColorScheme::ColorScheme(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
}

KColorScheme::ColorSet ColorScheme::getColorSet() const
{
    return m_colorSet;
}

void ColorScheme::setColorSet(KColorScheme::ColorSet colorSet)
{
    m_colorSet = colorSet;
    emit onBackgroundChange();
    emit onForegroundChange();
    emit onDecorationChange();
}

KColorScheme::BackgroundRole ColorScheme::getBackgroundRole() const
{
    return m_backgroundRole;
}

void ColorScheme::setBackgroundRole(KColorScheme::BackgroundRole role)
{
    m_backgroundRole = role;
    emit onBackgroundChange();
}

KColorScheme::ForegroundRole ColorScheme::getForegroundRole() const
{
    return m_foregroundRole;
}

void ColorScheme::setForegroundRole(KColorScheme::ForegroundRole role)
{
    m_foregroundRole = role;
    emit onForegroundChange();
}

KColorScheme::DecorationRole ColorScheme::getDecorationRole() const
{
    return m_decorationRole;
}

void ColorScheme::setDecorationRole(KColorScheme::DecorationRole role)
{
    m_decorationRole = role;
    emit onDecorationChange();
}

KColorScheme::ShadeRole ColorScheme::getShadeRole() const
{
    return m_shadeRole;
}

void ColorScheme::setShadeRole(KColorScheme::ShadeRole role)
{
    m_shadeRole = role;
    emit onShadeChange();
}

QColor ColorScheme::background() const
{
    return KStatefulBrush(m_colorSet,
                          m_backgroundRole)
            .brush(QPalette::Active).color();
}

QColor ColorScheme::foreground() const
{
    return KStatefulBrush(m_colorSet,
                          m_foregroundRole)
            .brush(QPalette::Active).color();
}

QColor ColorScheme::decoration() const
{
    return KStatefulBrush(m_colorSet,
                          m_decorationRole)
            .brush(QPalette::Active).color();
}

QColor ColorScheme::shade() const
{
    return KColorScheme(QPalette::Active).shade(m_shadeRole);
}
