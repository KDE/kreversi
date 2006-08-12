#ifndef KREVERSI_GAME_H
#define KREVERSI_GAME_H

#include <QObject>

#include "kreversiboard.h"

/**
 *  KReversiGame incapsulates all of the game logic.
 *  It creates KReversiBoard and manages a chips on it.
 *  Whenever the board state changes it emits corresponding signals.
 *  FIXME dimsuz: enumerate them briefly
 *  The idea is also to abstract from any graphic representation of the game process
 */
class KReversiGame : public QObject
{
    Q_OBJECT
public:
    KReversiGame();
    ~KReversiGame();
    /**
     *  @returns the board so the callers can examine its current state
     */
    const KReversiBoard& board() const;
    ChipColor currentPlayer() const { return NoColor;/* FIXME dimsuz: implement */ }
    int playerScore( ChipColor player ) const { return m_board->playerScore( player ); }
    ChipColor chipColorAt( int row, int col ) const { return m_board->chipColorAt( row, col ); }
    void putChipAt(int row, int col);
signals:
    void boardChanged();
private:
    KReversiBoard *m_board;
    /**
     *  Color of the current player
     */
    ChipColor m_curPlayer;
    /**
     *  The color of the computer played chips
     */
    ChipColor m_computerColor;
};
#endif
