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

#include <kdebug.h>

#include "kreversiboard.h"

static ChipColor opponentColorFor(ChipColor color)
{
    if(color == NoColor) 
        return color;
    else
        return ( color == White ? Black : White );
}

KReversiBoard::KReversiBoard()
{
    resetBoard();
}

void KReversiBoard::resetBoard()
{
    for(int r=0; r<8; ++r)
        for(int c=0; c<8; ++c)
            m_cells[r][c] = NoColor;
    // initial pos
    m_cells[3][3] = m_cells[4][4] = White;
    m_cells[3][4] = m_cells[4][3] = Black;

    m_score[White] = m_score[Black] = 2;
}

int KReversiBoard::playerScore( ChipColor player ) const
{
    return m_score[player];
}

void KReversiBoard::setChipColor(ChipColor color, int row, int col)
{
    Q_ASSERT( row < 8 && col < 8 );
    // first: if the current cell already contains a chip of the opponent,
    // we'll decrease opponents score
    if( m_cells[row][col] != NoColor && color != NoColor && m_cells[row][col] != color )
        m_score[opponentColorFor(color)]--;
    // if the cell contains some chip and is being replaced by NoColor,
    // we'll decrease the score of that color
    // Such replacements (with NoColor) occur during undoing
    if( m_cells[row][col] != NoColor && color == NoColor )
        m_score[ m_cells[row][col] ]--;

    // and now replacing with chip of 'color'
    m_cells[row][col] = color;

    if( color != NoColor )
        m_score[color]++;

    //kDebug() << "Score of White player: " << m_score[White] << endl;
    //kDebug() << "Score of Black player: " << m_score[Black] << endl;
}

ChipColor KReversiBoard::chipColorAt(int row, int col) const
{
    Q_ASSERT( row < 8 && col < 8 );
    return m_cells[row][col];
}
