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

void KReversiBoard::setChipColor(int row, int col, ChipColor color)
{
    Q_ASSERT( row < 8 && col < 8 );
    // first: if the current cell already contains a chip of the opponent,
    // we'll decrease opponents score
    if( m_cells[row][col] != NoColor && m_cells[row][col] != color )
        m_score[opponentColorFor(color)]--;
    // and now replacing with chip of 'color'
    m_cells[row][col] = color;
    m_score[color]++;
    kDebug() << "Score of White player: " << m_score[White] << endl;
    kDebug() << "Score of Black player: " << m_score[Black] << endl;
}

ChipColor KReversiBoard::chipColorAt(int row, int col) const
{
    Q_ASSERT( row < 8 && col < 8 );
    return m_cells[row][col];
}
