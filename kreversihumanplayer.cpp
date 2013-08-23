#include "kreversihumanplayer.h"

KReversiHumanPlayer::KReversiHumanPlayer(ChipColor color):
    KReversiPlayer(color)
{
}

void KReversiHumanPlayer::prepare(KReversiGame* game)
{
    // ready to start
    m_game = game;
    m_state = WAITING;

    emit ready();
}

void KReversiHumanPlayer::takeTurn()
{
    m_state = THINKING;
}

void KReversiHumanPlayer::skipTurn()
{
    m_state = WAITING; // waiting for opponent move
}

void KReversiHumanPlayer::gameOver()
{
    m_state = UNKNOWN; // game ended
}

void KReversiHumanPlayer::onUICellClick(KReversiPos pos)
{
    qDebug() << "UI CELL CLICK!" << ' ' << m_state;
    if (m_state != THINKING) {
        // it is not our turn ignore it
        return;
    }

    m_state = WAITING;
    emit makeMove(KReversiMove(m_color, pos));
}
