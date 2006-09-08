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
#ifndef KREVERSI_CHIP_H
#define KREVERSI_CHIP_H

#include <QGraphicsPixmapItem>
#include "commondefs.h"

class QGraphicsScene;
class KReversiChipFrameSet;

class KReversiChip : public QGraphicsPixmapItem
{
public:
    KReversiChip( ChipColor color, const KReversiChipFrameSet *frameSet, QGraphicsScene *scene );
    void setFrameSet( const KReversiChipFrameSet *frameSet );
    void setColor( ChipColor color );
    ChipColor color() const { return m_color; }
    /**
     *  Called during animation
     *  NOTE: it doesn't change the color of the chip when
     *  animation finishes - you've to do it yourself
     *  @return whether the animation sequence is finished
     */
    bool nextFrame();
    /**
     *  Toggles showing of little marker on top of the chip.
     *  It is used to indicate last-made move
     */
    void showLastMoveMarker(bool show);
private:
    ChipColor m_color;
    const KReversiChipFrameSet* m_frameSet;
    /**
     *  Current animation frame
     */
    int m_curFrame;
};

/**
 *  This class will hold a chip animation frameset.
 *  As all chips share the same frames it's good to
 *  put them in a single storage class like this,
 *  so any chip can retrieve any frame any time :).
 *
 *  This class is based on a number of assumptions which
 *  reflect the current allchips.png pixmap format.
 *  Here they are:
 *  1. Animation sequence goes from black to white colored chips
 *  2a. as a consequence of 1: frame(0) is black chip
 *  2b. as a consequence of 1: frame(frameCount()) is white chip
 *  But 2a and 2b shouldn't matter, because there's chipPixmap()
 *
 *  These are subject to changing/rethinking ;)
 */
class KReversiChipFrameSet
{
public:
    /**
     *  Initializes this class with 'allFrames' QPixmap, which 
     *  contains chip's animation sequence.
     *  The chips frames are extracted from it and put into 
     *  m_frames QList
     *  Supposes that allFrames represents an animation sequence 
     *  going from black to white.
     *  @param allFrames a pixmap containing whole animation sequence
     *  @param frameSize size of each frame's pixmap. Frames are squares.
     */
    KReversiChipFrameSet( const QPixmap& allFrames, int frameSize );
    /**
     *  Retruns a pixmap which corresponds to frame with number frameNo.
     *  It takes the chip color into account. This means that
     *  based on the assumption No. 1 (see class description), while frameNo 
     *  increases it will return
     *  frames from black to white if chip color == Black and
     *  from  white to black if chip color == White.
     *  It allows this class to hide all this pixmap-format assumtion tricks
     *  from KReversiChip which can simply increase frameNo and be happy :)
     */
    QPixmap frame( ChipColor color, int frameNo ) const;
    /**
     *  Returns a pixmap with a chip of corresponding color
     */
    QPixmap chipPixmap(ChipColor color) const;
    /**
     *  Returns number of frames in animation
     */
    int frameCount() const { return m_frames.count(); }
private:
    QList<QPixmap> m_frames;
};
#endif
