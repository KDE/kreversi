/*******************************************************************
 *
 * Copyright 2006-2007 Dmitry Suzdalev <dimsuz@gmail.com>
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
#ifndef KREVERSI_RENDERER_H
#define KREVERSI_RENDERER_H

#include <QPixmap>

class QSvgRenderer;
class KPixmapCache;

class KReversiRenderer
{
public:
    static KReversiRenderer* self();

    void renderBackground( QPainter *p, const QRectF& r ) const;
    void renderBoard( QPainter *p, const QRectF& r ) const;
    void renderBoardLabels( QPainter *p, const QRectF& r ) const;
    void renderPossibleMove( QPainter *p, const QRectF& r  ) const;
    void renderElement (QPainter *p, const QString& elementid, const QRectF& r ) const;
    QSize defaultBoardSize() const;
    QSize defaultChipSize() const;
private:
    // disable copy - it's singleton
    KReversiRenderer();
    KReversiRenderer( const KReversiRenderer& );
    KReversiRenderer& operator=( const KReversiRenderer& );
    ~KReversiRenderer();

    /**
     *  Svg renderer instance
     */
    QSvgRenderer *m_renderer;
    KPixmapCache *m_cache;
};
#endif
