#ifndef KREVERSI_GAME_H
#define KREVERSI_GAME_H

#include <QObject>
#include <QStack>

#include "commondefs.h"

class KReversiBoard;
class Engine;

/**
 *  KReversiGame incapsulates all of the game logic.
 *  It creates KReversiBoard and manages a chips on it.
 *  Whenever the board state changes it emits corresponding signals.
 *  The idea is also to abstract from any graphic representation of the game process
 *
 *  KReversiGame is supposed to be driven by someone from outside.
 *  I.e. it receives commands and emits events when it's internal state changes
 *  due to this commands dispatching.
 *  The main commands are:
 *  startNextTurn() and  makePlayerMove()
 *
 *  See KReversiScene for example of working with KReversiGame
 */
class KReversiGame : public QObject
{
    Q_OBJECT
public:
    KReversiGame();
    ~KReversiGame();
    /**
     *  Starts next player turn.
     *  If game isn't over yet, then this function do the following:
     *  - if it is computer turn and computer can move, it'll make that move.
     *  - if it is computer turn and computer can't move it'll emit "computerCantMove"
     *  signal and exit
     *  - if it is player turn and player can move then this function 
     *  will do nothing - you can call makePlayerMove(row,col) to make player move (but see last item)
     *  - if it is player turn and player can't move it'll make a computer move
     *  - in demo mode this function will make computer play player moves,
     *  so you don't need to call makePlayerMove.
     *
     *  If game is over it'll emit gameOver()
     *  
     *  If it's still unclear how to use it please see KReversiScene for working example.
     *  In short: it calls startNextTurn() at the end of each turn and makePlayerMove() 
     *  in mouseReleaseEvent()
     *
     *  @param demoMode if true then computer will decide for player turn
     */
    void startNextTurn(bool demoMode);
    /**
     *  This will make the player move at row, col.
     *  If that is possible of course
     *  If demoMode is true, the computer will decide on what move to take.
     *  row and col values do not matter in that case.
     */
    void makePlayerMove(int row, int col, bool demoMode);
    /**
     *  This function will make computer decide where he 
     *  wants to put his chip... and he'll put it there!
     */
    void makeComputerMove();
    /**
     *  Undoes all the computer moves and one player move
     *  (so after calling this function it will be player turn)
     *  @return number of undone moves
     */
    int undo();
    /**
     *  Sets the strength of game engine (1 to 7)
     */
    void setEngineStrength(uint strength);
    /**
     *  Sets the computer skill level. From 1 to 7
     */
    void setComputerSkill(int skill);
    /**
     *  @return strength of the game engine
     */
    uint strength() const;
    /**
     *  @return whether the game is already over
     */
    bool isGameOver() const; // perhaps this doesn't need to be public
    /**
     *  @return whether any player move is at all possible
     */
    bool isAnyPlayerMovePossible() const;// perhaps this doesn't need to be public
    /**
     *  @return whether any computer move is at all possible
     */
    bool isAnyComputerMovePossible() const;// perhaps this doesn't need to be public
    /**
     *  @return a color of the current player
     */
    ChipColor currentPlayer() const { return m_curPlayer; }
    
    // NOTE: this is just a wrapper around KReversiBoard::playerScore
    // Maybe consider merging KReversiBoard into this class?
    // same applies to chipColorAt
    /**
     *  @return score (number of chips) of the player
     */
    int playerScore( ChipColor player ) const;
    // NOTE: this is just a wrapper around KReversiBoard::playerScore
    ChipColor chipColorAt( int row, int col ) const;
    /**
     *  @return if undo is possible
     */
    bool canUndo() const { return !m_undoStack.isEmpty(); }
    /**
     *  Returns a hint to current player
     */
    KReversiMove getHint() const;
    /**
     *  @return last move made
     */
    KReversiMove getLastMove() const;
    /**
     *  Returns true, if it's computer's turn now
     */
    bool isComputersTurn() const { return m_curPlayer == m_computerColor; }
    /**
     *  @return a list of chips which were changed during last move.
     *  First of them will be the move itself, and the rest - chips which
     *  were turned by that move
     */
    MoveList changedChips() const { return m_changedChips; }
    /**
     *  @return a list of possible moves for current player
     */
    MoveList possibleMoves() const;
signals:
    void gameOver();
    void boardChanged();
    void moveFinished();
    void computerCantMove();
private:
    enum Direction { Up, Down, Right, Left, UpLeft, UpRight, DownLeft, DownRight };
    /**
     * This function will tell you if the move is possible.
     * That's why it was given such a name ;)
     */
    bool isMovePossible( const KReversiMove& move ) const; 
    /**
     *  Searches for "chunk" in direction dir for move.
     *  As my English-skills are somewhat limited, let me introduce 
     *  new terminology ;).
     *  I'll define a "chunk" of chips for color "C" as follows:
     *  (let "O" be the color of the opponent for "C")
     *  CO[O]C <-- this is a chunk
     *  where [O] is one or more opponent's pieces
     */
    bool hasChunk( Direction dir, const KReversiMove& move) const;
    /**
     *  Performs move, i.e. marks all the chips that player wins with
     *  this move with current player color
     */
    void makeMove( const KReversiMove& move );
    /**
     *  Board itself
     */
    KReversiBoard *m_board;
    /**
     *  Color of the current player
     */
    ChipColor m_curPlayer;
    /**
     *  The color of the human played chips
     */
    ChipColor m_playerColor;
    /**
     *  The color of the computer played chips
     */
    ChipColor m_computerColor;
    /**
     *  Our AI
     */
    Engine *m_engine;
     // Well I'm not brief at all :). That's because I think that my
     // English is not well shaped sometimes, so I try to describe things
     // so that me and others can understand. Even simple things.
     // Espesially when I think that my description sucks :)
    /**
     *  This list holds chips that were changed/added during last move
     *  First of them will be the chip added to the board by the player
     *  during last move. The rest of them - chips that were turned by that
     *  move.
     */
    MoveList m_changedChips;
    /**
     *  This is an undo stack.
     *  It contains a lists of chips changed with each turn.
     *  @see m_changedChips
     */
    QStack<MoveList> m_undoStack;
};
#endif
