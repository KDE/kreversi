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
    // FIXME dimsuz: move this to single SVG. and introduce smth like setTheme()
    m_bkgndRenderer = new KSvgRenderer();
    m_bkgndRenderer->load( KStandardDirs::locate( "appdata", "pics/background.svgz" ) );

    m_boardRenderer = new KSvgRenderer();
    m_boardRenderer->load( KStandardDirs::locate("appdata", "pics/default_board.svgz") );

    m_boardLabelsRenderer = new KSvgRenderer();
    m_boardLabelsRenderer->load( KStandardDirs::locate("appdata", "pics/board_numbers.svgz") );

    m_possMovesRenderer = new KSvgRenderer();
    m_possMovesRenderer->load( KStandardDirs::locate("appdata", "pics/move_hint.svgz") );
}

void KReversiRenderer::renderBackground( QPainter *p, const QRectF& r )
{
    m_bkgndRenderer->render( p, r );
}

void KReversiRenderer::renderBoard( QPainter *p, const QRectF& r )
{
    m_boardRenderer->render( p, r );
}

void KReversiRenderer::renderBoardLabels( QPainter *p, const QRectF& r )
{
    m_boardLabelsRenderer->render( p, r );
}

void KReversiRenderer::renderPossibleMove( QPainter *p )
{
    m_possMovesRenderer->render( p );
}

KReversiRenderer::~KReversiRenderer()
{
    delete m_bkgndRenderer;
    delete m_boardRenderer;
    delete m_boardLabelsRenderer;
    delete m_possMovesRenderer;
}

QSize KReversiRenderer::defaultBoardSize() const
{
    return m_boardRenderer->defaultSize();
}
