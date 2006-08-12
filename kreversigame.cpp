#include <kdebug.h>

#include "kreversiboard.h"
#include "kreversigame.h"


KReversiGame::KReversiGame()
    : m_curPlayer(Black), m_computerColor( White )
{
    m_board = new KReversiBoard();
}

KReversiGame::~KReversiGame()
{
    delete m_board;
}

void KReversiGame::putChipAt( int row, int col )
{
    if( m_board->chipColorAt( row, col ) != NoColor )
    {
        kDebug() << "This cell isn't empty! No move Possible" << endl;
        return;
    }
    m_board->setChipColor( row, col, m_curPlayer );
    m_curPlayer = (m_curPlayer == White ? Black : White );
    emit boardChanged();
}

const KReversiBoard& KReversiGame::board() const
{
    return *m_board;
}

#include "kreversigame.moc"
