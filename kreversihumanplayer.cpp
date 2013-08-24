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
