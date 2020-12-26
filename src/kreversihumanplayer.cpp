/*
    SPDX-FileCopyrightText: 2013 Denis Kuplyakov <dener.kup@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kreversihumanplayer.h"

KReversiHumanPlayer::KReversiHumanPlayer(ChipColor color, const QString &name):
    KReversiPlayer(color, name, true, true)
{
}

void KReversiHumanPlayer::prepare(KReversiGame* game)
{
    // ready to start
    m_game = game;
    m_state = WAITING;

    Q_EMIT ready();
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
    if (m_state != THINKING) {
        // it is not our turn ignore it
        return;
    }

    m_state = WAITING;
    Q_EMIT makeMove(KReversiMove(m_color, pos));
}
