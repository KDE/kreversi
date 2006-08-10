#include <kdebug.h>

#include "kreversiboard.h"

KReversiBoard::KReversiBoard(int boardSize)
    : m_boardSize(boardSize)
{
    resetBoard();
}

void KReversiBoard::resetBoard()
{
    m_cells.resize( m_boardSize * m_boardSize );
    m_cells.fill( noChip );
}

void KReversiBoard::setChip(int row, int col, ChipType type)
{
    Q_ASSERT( row < m_boardSize && col < m_boardSize );
    m_cells[row * m_boardSize + col] = type;
}

KReversiBoard::ChipType KReversiBoard::chipAt(int row, int col) const
{
    Q_ASSERT( row < m_boardSize && col < m_boardSize );
    return m_cells.at( row * m_boardSize + col );
}
