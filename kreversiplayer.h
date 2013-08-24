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

#ifndef KREVERSI_PLAYER_H
#define KREVERSI_PLAYER_H

#include "kreversigame.h"
class KReversiGame;

enum KReversiPlayerState {
    WAITING,
    THINKING,
    UNKNOWN
};

class KReversiPlayer: public QObject
{
    Q_OBJECT
public:
    explicit KReversiPlayer(ChipColor color);

    /**
     * Used to get player color
     * @return color of player
     */
    ChipColor getColor() const;

public slots:
    /**
     *  Triggered by KReversiGame before game starts
     */
    virtual void prepare(KReversiGame* game) = 0;

    /**
     *  It triggered from KReversiGame.
     *  Means that player should start think about his move.
     */
    virtual void takeTurn() = 0;

    /**
     *  It triggered from KReversiGame.
     *  Means that player can't do move and skips it.
     */
    virtual void skipTurn() = 0;

    /**
     *  Triggered by KReversiGame to notify player that game is over.
     */
    virtual void gameOver() = 0;

signals:
    /**
     *  Player emit it when want to notify about his move
     */
    void makeMove(KReversiMove);

    /**
     * Player emit it after he has prepared after getting prepare command
     */
    void ready();

protected:
    /**
     *  Game player is playing
     */
    KReversiGame *m_game;

    /**
     *  Is player thinking or waiting
     */
    KReversiPlayerState m_state;

    /**
     *  Player's chip color
     */
    ChipColor m_color;
};

#endif // KREVERSIPLAYER_H
