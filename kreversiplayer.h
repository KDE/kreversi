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

#include <kreversigame.h>
#include <QString>

class KReversiGame;

/**
 * Indicates current state of the player
 */
enum KReversiPlayerState {
    /** Player is waiting for his move or smth else */
    WAITING,
    /** Player is thinking about his move */
    THINKING,
    /** Player state is unknown */
    UNKNOWN
};
/**
 * Represents abstract player. It is interface.
 */
class KReversiPlayer: public QObject
{
    Q_OBJECT
public:
    /**
     * Construct player with specified @p color and @p name.
     * @param hintAllowed determines whether hints are allowed for player
     * @param undoAllowed determines whether undos are allowed for player
     */
    explicit KReversiPlayer(ChipColor color, QString name,
                            bool hintAllowed, bool undoAllowed);

    /**
     *  Used to get player color
     *  @return color of player
     */
    ChipColor getColor() const;

    /**
     *  Used to get player's name
     *  @return player's name
     */
    QString getName() const;

    /**
     *  @return is hint allowed for player or not
     */
    bool isHintAllowed() const;

    /**
     *  KReversiGame triggers it to to increase used hints count
     */
    void hintUsed();

    /**
     *  @return how many times player has used hints
     */
    int getHintsCount();

    /**
     *  @return is undo allowed for player
     */
    bool isUndoAllowed() const;

    /**
     *  KReversiGame triggers it to to increase used undos count
     */
    void undoUsed();

    /**
     *  @return how many times player has used undo
     */
    int getUndoCount();

public slots:
    //TODO: do this with overriding to not bother user of assigning game
    /**
     *  Triggered by KReversiGame before game starts
     *  Implementation should assign @p game to m_game
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
     *  Game which player is playing
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

    /**
     *  Player's name to be shown at UI
     */
    QString m_name;

    /**
     *  Are hints enabled for player
     */
    bool m_hintAllowed;

    /**
     *  How many times player has used hint
     */
    int m_hintCount;

    /**
     *  Can player request undo or not
     */
    bool m_undoAllowed;

    /**
     *  How many times player has used undo
     */
    int m_undoCount;
};

#endif // KREVERSIPLAYER_H
