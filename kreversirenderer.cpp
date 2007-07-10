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
#include <ksvgrenderer.h>

KReversiRenderer* KReversiRenderer::self()
{
    static KReversiRenderer instance;
    return &instance;
}

KReversiRenderer::KReversiRenderer()
{
    // TODO introduce smth like setTheme()
    m_renderer = new KSvgRenderer();
    m_renderer->load( KStandardDirs::locate( "appdata", "pics/default_theme.svgz" ) );
}

void KReversiRenderer::renderBackground( QPainter *p, const QRectF& r )
{
    m_renderer->render( p, "background", r );
}

void KReversiRenderer::renderBoard( QPainter *p, const QRectF& r )
{
    m_renderer->render( p, "board", r );
}

void KReversiRenderer::renderBoardLabels( QPainter *p, const QRectF& r )
{
    m_renderer->render( p, "board_numbers", r );
}

void KReversiRenderer::renderPossibleMove( QPainter *p )
{
    m_renderer->render( p, "move_hint" );
}

KReversiRenderer::~KReversiRenderer()
{
    delete m_renderer;
}

QSize KReversiRenderer::defaultBoardSize() const
{
    QRectF boardSize = m_renderer->boundsOnElement("board");
    return QSize((int) boardSize.width(), (int) boardSize.height());
}
