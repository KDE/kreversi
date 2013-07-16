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
