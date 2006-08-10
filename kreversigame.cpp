#include <kdebug.h>

#include "kreversiboard.h"
#include "kreversigame.h"


KReversiGame::KReversiGame()
{
    m_board = new KReversiBoard();
}

KReversiGame::~KReversiGame()
{
    delete m_board;
}

const KReversiBoard& KReversiGame::board() const
{
    return *m_board;
}
