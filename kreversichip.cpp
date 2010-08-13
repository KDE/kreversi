/*******************************************************************
 *
 * Copyright 2006 Dmitry Suzdalev <dimsuz@gmail.com>
 * Copyright 2010 Brian Croom <brian.s.croom@gmail.com>
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
#include "kreversichip.h"
#include <QPainter>
#include <QPixmap>
#include <QImage>
#include <QGraphicsScene>

#include <kdebug.h>

QPixmap* KReversiChip::s_lastMoveMarker;

KReversiChip::KReversiChip( KGameRenderer* renderer, ChipColor color,
        const QString& chipsPrefix, int size, QGraphicsScene* scene )
    : KGameRenderedItem( renderer, chipsPrefix ), m_color(color), m_curFrame(0)
{
    scene->addItem(this);
    setRenderSize( QSize(size, size) );
    setColor(m_color);
}

void KReversiChip::setChipPrefix( const QString& chipPrefix )
{
    setSpriteKey( chipPrefix );
}

void KReversiChip::setChipSize( int newSize )
{
    setRenderSize( QSize(newSize, newSize) );
}

void KReversiChip::setColor( ChipColor color )
{
    m_color = color;
    setFrame(currentFrameNumber());
}

bool KReversiChip::nextFrame()
{
    setFrame(currentFrameNumber());
    m_curFrame++;
    if(m_curFrame == frameCount())
    {
        m_curFrame = 0;
        return true;
    }
    return false;
}

void KReversiChip::showLastMoveMarker(bool show)
{
    QList<QGraphicsItem*> children = childItems();
    qDeleteAll(children);
    if(show)
    {
        QGraphicsItem* marker = new QGraphicsPixmapItem(*s_lastMoveMarker, this);
        marker->setFlag(QGraphicsItem::ItemStacksBehindParent);
    }
}

void KReversiChip::initLastMoveMarker( int size )
{
    if(s_lastMoveMarker != NULL)
        delete s_lastMoveMarker;
    s_lastMoveMarker = new QPixmap(size, size);
    s_lastMoveMarker->fill(Qt::transparent);
    QPainter p(s_lastMoveMarker);
    qreal offset = size*.02;
    p.fillRect(QRectF(offset, offset, size-(offset*4), size-(offset*4)), Qt::gray );
    p.end();
}

int KReversiChip::currentFrameNumber() const
{
    if( m_color == White )
        return frameCount() - m_curFrame;
    else if( m_color == Black )
        return m_curFrame+1;
    else return -1;
}

