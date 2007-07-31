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
#include "kreversichip.h"
#include "kreversirenderer.h"
#include <QPainter>
#include <QPixmap>
#include <QImage>

#include <kdebug.h>

KReversiChip::KReversiChip( ChipColor color, const KReversiChipFrameSet* frameSet, QGraphicsScene* scene )
    : QGraphicsPixmapItem( 0, scene ), m_color(color), m_frameSet(frameSet), m_curFrame(0)
{
    setColor(m_color);
}

void KReversiChip::setFrameSet( const KReversiChipFrameSet* frameSet )
{
    m_frameSet = frameSet;
}

void KReversiChip::setColor( ChipColor color )
{
    m_color = color;
    setPixmap( m_frameSet->chipPixmap( m_color ) );
}

bool KReversiChip::nextFrame()
{
    setPixmap( m_frameSet->frame( m_color, m_curFrame++ ) );
    bool finished = (m_curFrame == m_frameSet->frameCount());
    if(finished)
        m_curFrame = 0;
    return finished;
}

void KReversiChip::showLastMoveMarker(bool show)
{
    if(show)
    {
        QPixmap origPix = pixmap();
        QPixmap pix = origPix;
        QPainter p(&pix);
        p.fillRect( 1, 1, pix.width()-2, pix.height()-2, Qt::gray );
        p.drawPixmap(0,0, origPix);
        p.end();
        setPixmap(pix);
    }
    else
        setPixmap( m_frameSet->chipPixmap( m_color ) );
}

// -------------------------------------------------------------------------------

KReversiChipFrameSet::KReversiChipFrameSet()
{
}

KReversiChipFrameSet::~KReversiChipFrameSet()
{
}

void KReversiChipFrameSet::switchChipSet( const QString& chipsPrefix, int chipSize )
{
    m_currentChipsPrefix = chipsPrefix;
    int size = chipSize == 0 ? KReversiRenderer::self()->defaultChipSize().width() : chipSize;
    setChipSize( size );
}

void KReversiChipFrameSet::setChipSize( int newSize )
{
    QImage baseImg;
    m_frames.clear();
    for (int i=1; i<=12; i++) {
    //Construct an image object to render the contents of the .svgz file
    baseImg = QImage(newSize, newSize, QImage::Format_ARGB32_Premultiplied);
    //Fill the buffer, it is unitialised by default
    baseImg.fill(0);
    QPainter p(&baseImg);
    QString nextelement(m_currentChipsPrefix.arg(i));
    KReversiRenderer::self()->renderElement(&p, nextelement, QRectF(0,0,newSize,newSize));
    p.end();
    m_frames.append( QPixmap::fromImage(baseImg) );
    }
}

QPixmap KReversiChipFrameSet::frame( ChipColor color, int frameNo ) const
{
    int idx = 0;
    if( color == White )
        idx = m_frames.count() - frameNo - 1;
    else if(color == Black)
        idx = frameNo;

    Q_ASSERT( idx >= 0 && idx < m_frames.count() );

    return m_frames.at( idx );
}

QPixmap KReversiChipFrameSet::chipPixmap( ChipColor color ) const
{
    return ( color == White ? m_frames.at(frameCount()-1) : m_frames.at(0) );
}
