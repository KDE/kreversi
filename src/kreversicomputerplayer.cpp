/*
    SPDX-FileCopyrightText: 2013 Denis Kuplyakov <dener.kup@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kreversicomputerplayer.h"

KReversiComputerPlayer::KReversiComputerPlayer(ChipColor color, const QString &name):
    KReversiPlayer(color, name, false, false), m_lowestSkill(100) // setting it big enough
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

    Q_EMIT ready();
}

void KReversiComputerPlayer::takeTurn()
{
    m_state = THINKING;
    KReversiMove move = m_engine->computeMove(*m_game, true);
    move.color = m_color;
    m_state = WAITING;
    Q_EMIT makeMove(move);
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
    m_lowestSkill = qMin(m_lowestSkill, skill);
}

int KReversiComputerPlayer::lowestSkill()
{
    return m_lowestSkill;
}


