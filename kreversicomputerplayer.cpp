#include "kreversicomputerplayer.h"

KReversiComputerPlayer::KReversiComputerPlayer(ChipColor color):
    KReversiPlayer(color)
{
    m_engine = new Engine(1);
}

KReversiComputerPlayer::~KReversiComputerPlayer()
{
    delete m_engine;
}

void KReversiComputerPlayer::prepare(KReversiGame *game)
{
    m_game = game;
    m_state = WAITING;
}

void KReversiComputerPlayer::takeTurn()
{
    m_state = THINKING;
    KReversiMove move = m_engine->computeMove(*m_game, true);
    move.color = m_color;
    m_state = WAITING;
    emit makeMove(move);
}

void KReversiComputerPlayer::skipTurn()
{
    m_state = WAITING;
}

void KReversiComputerPlayer::gameOver()
{
    m_state = UNKNOWN;
}

void KReversiComputerPlayer::setSkill(int skill)
{
    m_engine->setStrength(skill);
}


