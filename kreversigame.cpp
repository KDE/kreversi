#include <kdebug.h>

#include "kreversiboard.h"
#include "kreversigame.h"


KReversiGame::KReversiGame( int boardSize )
{
    m_board = new KReversiBoard(boardSize);
}

KReversiGame::~KReversiGame()
{
    delete m_board;
}

const KReversiBoard& KReversiGame::board() const
{
    return *m_board;
}
