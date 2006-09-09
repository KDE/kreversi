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
#ifndef KREVERSI_BOARD_H
#define KREVERSI_BOARD_H

#include "commondefs.h"

/**
 *  This class represents a reversi board.
 *  It only holds a current state of the game, 
 *  i.e. what type of chip the cells contain - nothing more.
 *  So this is just a plain QVector with some nice additions.
 */
class KReversiBoard
{
public:
    KReversiBoard();
    /**
     *  Resets board to initial (start-of-new-game) state
     */
    void resetBoard();
    /**
     *  Sets the type of chip at (row,col)
     */
    void setChipColor(ChipColor type, int row, int col);
    /**
     *  Retruns type of the chip in cell at (row, col)
     */
    ChipColor chipColorAt(int row, int col) const;
    /**
     *  Returns number of chips owned by this player
     */
    int playerScore( ChipColor player ) const;
private:
    /**
     *  The board itself
     */
    ChipColor m_cells[8][8];
    /**
     *  Score of each player
     */
    int m_score[2];
};
#endif
