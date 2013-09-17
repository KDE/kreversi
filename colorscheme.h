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

/**
 * Wrapper to access KColorScheme color methods from QML code.
 *
 * Used to access KDE's current color theme's colors from QML code.
 * To use it you must register KColorScheme and QPalette classes with
 * qmlRegisterUncreatableType, as their enums are used to access KColorScheme
 * methods.
 *
 * Example usage of class:
 * \code{.qml}
   Rectangle {
       color: backgroundTokken.background

       ColorScheme {
            id: backgroundTokken
            colorSet: KColorScheme.Tooltip
            backgroundRole: KColorScheme.NormalBackground
       }
   }
   \endcode
 *
 * @see KColorScheme
 */
class ColorScheme : public QDeclarativeItem
{
    //TODO: add QPalette::ColorGroup param
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

    /**
     * @return current KColorScheme::ColorSet parameter value
     */
    KColorScheme::ColorSet getColorSet() const;
    /**
     * Sets KColorScheme::ColorSet parameter value to @p colorSet
     */
    void setColorSet(KColorScheme::ColorSet colorSet);

    /**
     * @return current KColorScheme::BackgroundRole parameter value
     */
    KColorScheme::BackgroundRole getBackgroundRole() const;
    /**
     * Sets KColorScheme::BackgroundRole parameter value to @p role
     */
    void setBackgroundRole(KColorScheme::BackgroundRole role);

    /**
     * @return current KColorScheme::ForegroundRole parameter value
     */
    KColorScheme::ForegroundRole getForegroundRole() const;
    /**
     * Sets KColorScheme::ForegroundRole parameter value to @p role
     */
    void setForegroundRole(KColorScheme::ForegroundRole role);

    /**
     * @return current KColorScheme::DecorationRole parameter value
     */
    KColorScheme::DecorationRole getDecorationRole() const;
    /**
     * Sets KColorScheme::DecorationRole parameter value to @p role
     */
    void setDecorationRole(KColorScheme::DecorationRole role);

    /**
     * @return current KColorScheme::ShadeRole parameter value
     */
    KColorScheme::ShadeRole getShadeRole() const;
    /**
     * Sets KColorScheme::ShadeRole parameter value to @p role
     */
    void setShadeRole(KColorScheme::ShadeRole role);

    QColor background() const;
    QColor foreground() const;
    QColor decoration() const;
    QColor shade() const;

signals:
    /**
     * Emitted when one of parameters that affects background has been changed.
     * @see colorSet backgroundRole
     */
    void onBackgroundChange();
    /**
     * Emitted when one of parameters that affects foreground has been changed.
     * @see colorSet foregroundRole
     */
    void onForegroundChange();
    /**
     * Emitted when one of parameters that affects decoration has been changed.
     * @see colorSet decorationRole
     */
    void onDecorationChange();
    /**
     * Emitted when one of parameters that affects shade has been changed.
     * @see shadeRole
     */
    void onShadeChange();

private:
    KColorScheme::ColorSet m_colorSet;
    KColorScheme::BackgroundRole m_backgroundRole;
    KColorScheme::ForegroundRole m_foregroundRole;
    KColorScheme::DecorationRole m_decorationRole;
    KColorScheme::ShadeRole m_shadeRole;
};

#endif // COLORSCHEME_H
