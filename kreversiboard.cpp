#include <kdebug.h>

#include "kreversiboard.h"

KReversiBoard::KReversiBoard()
{
    resetBoard();
}

void KReversiBoard::resetBoard()
{
    for(int r=0; r<8; ++r)
        for(int c=0; c<8; ++c)
            m_cells[r][c] = NoColor;
}

void KReversiBoard::setChipColor(int row, int col, ChipColor color)
{
    Q_ASSERT( row < 8 && col < 8 );
    m_cells[row][col] = color;
}

ChipColor KReversiBoard::chipColorAt(int row, int col) const
{
    Q_ASSERT( row < 8 && col < 8 );
    return m_cells[row][col];
}
