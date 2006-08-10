#ifndef KREVERSI_GAME_H
#define KREVERSI_GAME_H

#include <QObject>

class KReversiBoard;

/**
 *  KReversiGame incapsulates all of the game logic.
 *  It creates KReversiBoard and manages a chips on it.
 *  Whenever the board state changes it emits corresponding signals.
 *  FIXME dimsuz: enumerate them briefly
 *  The idea is also to abstract from any graphic representation of the game process
 */
class KReversiGame : public QObject
{
public:
    KReversiGame();
    ~KReversiGame();
    /**
     *  @returns the board so the callers can examine its current state
     */
    const KReversiBoard& board() const;
private:
    KReversiBoard *m_board;
};
#endif
