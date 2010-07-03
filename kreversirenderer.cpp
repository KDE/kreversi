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

#include "kreversirenderer.h"
#include <kstandarddirs.h>
#include <qsvgrenderer.h>
#include <kpixmapcache.h>

#include <QPainter>

KReversiRenderer* KReversiRenderer::self()
{
    static KReversiRenderer instance;
    return &instance;
}

KReversiRenderer::KReversiRenderer()
{
    m_renderer = new QSvgRenderer();
    m_renderer->load( KStandardDirs::locate( "appdata", "pics/default_theme.svgz" ) );

    m_cache = new KPixmapCache("kreversi-cache");
    m_cache->setCacheLimit( 3*1024 );
}

void KReversiRenderer::renderBackground( QPainter *p, const QRectF& r ) const
{
    renderElement(p, "background", r);
}

void KReversiRenderer::renderBoard( QPainter *p, const QRectF& r ) const
{
    renderElement(p, "board", r);
}

void KReversiRenderer::renderBoardLabels( QPainter *p, const QRectF& r ) const
{
    renderElement(p, "board_numbers", r);
}

void KReversiRenderer::renderPossibleMove( QPainter *p, const QRectF& r  ) const
{
    renderElement( p, "move_hint", r );
}

void KReversiRenderer::renderElement (QPainter *p, const QString& elementid, const QRectF& r ) const
{
    QPixmap pix;
    QString cacheStr = elementid+QString("_%1x%2").arg(r.width()).arg(r.height());
    if(!m_cache->find(cacheStr, pix))
    {
        pix = QPixmap(r.size().toSize());
        pix.fill(Qt::transparent);
        QPainter paint(&pix);
        m_renderer->render(&paint, elementid);
        paint.end();
        m_cache->insert(cacheStr, pix);
    }

    p->drawPixmap(static_cast<int>(r.x()), static_cast<int>(r.y()), pix);
}

KReversiRenderer::~KReversiRenderer()
{
    delete m_renderer;
    delete m_cache;
}

QSize KReversiRenderer::defaultBoardSize() const
{
    QRectF boardSize = m_renderer->boundsOnElement("board");
    return QSize((int) boardSize.width(), (int) boardSize.height());
}

QSize KReversiRenderer::defaultChipSize() const
{
    QRectF chipSize = m_renderer->boundsOnElement("chip_bw_1");
    return QSize((int) chipSize.width(), (int) chipSize.height());
}
