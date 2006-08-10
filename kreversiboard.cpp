#include "kreversiboard.h"

KReversiBoard::KReversiBoard(int boardSize)
    : m_boardSize(boardSize)
{
    resetBoard();
}

void KReversiBoard::resetBoard()
{
    m_cells.resize( m_boardSize * m_boardSize );
    m_cells.fill( emptyCell );
}
