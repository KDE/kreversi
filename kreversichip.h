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
#ifndef KREVERSI_CHIP_H
#define KREVERSI_CHIP_H

#include <KGameRenderedItem>
#include <KGameRenderer>
#include "commondefs.h"

class QGraphicsScene;

class KReversiChip : public KGameRenderedItem
{
public:
    KReversiChip( KGameRenderer* renderer, ChipColor color,
        const QString& chipsPrefix, int size, QGraphicsScene* scene );
    void setChipPrefix( const QString& chipPrefix );
    void setChipSize( int newSize );
    void setColor( ChipColor color );
    void setRowCol( int row, int col ) { m_row = row; m_col = col; }

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
    /**
     * Render the pixmap for marking the last move
     */
    static void initLastMoveMarker( int size );

    enum { Type = UserType + 1 };
    virtual int type() const { return Type; }
private:
    int currentFrameNumber() const;

    ChipColor m_color;
    /**
     *  Current animation frame
     */
    int m_curFrame;
    int m_row;
    int m_col;
    
    static QPixmap* s_lastMoveMarker;
};

#endif
