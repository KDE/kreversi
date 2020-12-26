/*
    SPDX-FileCopyrightText: 2013 Denis Kuplyakov <dener.kup@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "colorscheme.h"

ColorScheme::ColorScheme(QQuickItem *parent) :
QQuickItem(parent)
{
}

QColor ColorScheme::background() const
{
    return KStatefulBrush(KColorScheme::Tooltip,
                          KColorScheme::NormalBackground)
    .brush(QPalette::Active).color();
}

QColor ColorScheme::foreground() const
{
    return KStatefulBrush(KColorScheme::Tooltip,
                          KColorScheme::NormalText)
    .brush(QPalette::Active).color();
}

QColor ColorScheme::border() const
{
    return KStatefulBrush(KColorScheme::View,
                          KColorScheme::NormalText)
    .brush(QPalette::Active).color();
}
