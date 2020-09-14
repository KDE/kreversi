/*******************************************************************
 *
 * Copyright 2013 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * This file is part of the KDE project "KReversi"
 *
 * KReversi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * KReversi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KReversi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 ********************************************************************/

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


