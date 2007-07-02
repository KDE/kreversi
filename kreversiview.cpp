/*******************************************************************
 *
 * Copyright 2006 Dmitry Suzdalev <dimsuz@gmail.com>
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
#include "kreversiview.h"
#include "kreversiscene.h"

#include <QGraphicsScene>
#include <QResizeEvent>

#include <kdebug.h>

KReversiView::KReversiView( KReversiScene* scene, QWidget *parent )
    : QGraphicsView(scene, parent), m_scene(scene)
{
    setCacheMode( QGraphicsView::CacheBackground );
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle( QFrame::NoFrame );
    setOptimizationFlags( QGraphicsView::DontClipPainter |
                          QGraphicsView::DontSavePainterState |
                          QGraphicsView::DontAdjustForAntialiasing );
}

void KReversiView::resizeEvent( QResizeEvent* ev )
{
    m_scene->resizeScene( ev->size().width(), ev->size().height() );
}
