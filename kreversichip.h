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
    void setRowCol( int row, int col ) { m_row = row; m_col = col; };

    int row() const { return m_row; }
    int col() const { return m_col; }
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

    enum { Type = UserType + 1 };
    virtual int type() const { return Type; }
private:
    ChipColor m_color;
    const KReversiChipFrameSet* m_frameSet;
    /**
     *  Current animation frame
     */
    int m_curFrame;
    int m_row;
    int m_col;
};

class KSvgRenderer;

/**
 *  This class will load and hold a chip animation frameset.
 *  As all chips share the same frames it's good to
 *  put them in a single storage class like this,
 *  so any chip can retrieve any frame any time :).
 *
 *  This class is based on a number of assumptions which
 *  reflect the current chips.svgz pixmap format.
 *  Here they are:
 *  1. Animation sequence goes from black to white colored chips
 *  2a. as a consequence of 1: frame(0) is black chip
 *  2b. as a consequence of 1: frame(frameCount()) is white chip
 *  But 2a and 2b shouldn't matter, because there's chipPixmap()
 *  3. The svg image contains 4 cols and 3 rows of chip pixmaps, i.e 12 frames
 */
class KReversiChipFrameSet
{
public:
    KReversiChipFrameSet();
    ~KReversiChipFrameSet();
    /**
     *  Loads a chips (svg) pixmap found in path chipsPath, which 
     *  contains chip's animation sequence.
     *  The chips frames are extracted from it and put into 
     *  m_frames QList
     *  Supposes that this pixmap represents an animation sequence 
     *  going from black to white.
     *  @param chipsPath a path to svg pixmap containing whole animation sequence
     *  @param chipSize if not equal to 0 then chips will be scaled to chipSize x chipSize each
     *  else chips will be loaded unscaled
     */
    void loadFrames( const QString& chipsPath, int chipSize = 0 );
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
    /**
     *  Sets chip pixmap size to size.
     *  I.e. re-renders svg image so that each individual chip
     *  pixmap in m_frames will have size passed as a parameter to this function
     */
    void setChipSize( int newSize );
    /**
     *  Returns default chip size
     */
    int defaultChipSize() const { return m_frames.at(0).width(); }
private:
    QList<QPixmap> m_frames;
    KSvgRenderer *m_renderer;
};
#endif
