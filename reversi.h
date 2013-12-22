/*******************************************************************
 *
 * Copyright 2013 Inge Wallin <inge@lysator.liu.se>
 * Copyright 2006 Dmitry Suzdalev <dimsuz@gmail.com>
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

#ifndef REVERSI_H
#define REVERSI_H

// Qt
#include <QStack>

// KReversi
//#include "commondefs.h"


// Let's establish some terminology:
//
//  * "Square"
//    a coordinate on the board, like A3 or D5.  The letter is used to
//    establish the horizontal coordinate, starting with 'A' from the
//    left and the figure is used to establish the vertical
//    coordinate, starting with '1' from the top.
//
//  * "Color"
//    the color of the piece on a certain square. Piece colors inside
//    this engine are Black or White even if they can be shown using
//    other colors in the GUI.  This could also be Empty if there is
//    no piece on the square.
//
//  * "Board"
//    the current configuration of pieces on the board, regardless
//    of any other circumstances.  This is not as useful as the
//    Position. 
//
//  * "Position"
//    a snapshot of the game at a certain point in time with complete
//    information of all the current status of the game. In Reversi,
//    this means a complete board configuration and which player that
//    is to move.
//
//    In games like chess it would also include things like castling
//    status of each side, if there is an en-passant situation active
//    and also the number of moves since the last capture (>50 moves
//    without capture means that the game is a draw). But reversi
//    doesn't have any of those complications


namespace Reversi
{

// Note that the color is sometimes used as an array index so
// don't change this without a careful prestudy.
enum Color {
    Black = 0,
    White = 1,
    NoColor = 2      // Used to represent Empty in the board and invalid in a move
};

inline Color opponentColorFor(Color color)
{
    return (color == NoColor
            ? color
            : static_cast<Color>(1 - static_cast<int>(color)));
}


struct Square
{
    Square(int r = -1, int c = -1)
        : row(r), col(c)
    { }

    int row;
    int col;

    bool isValid() const { return (row != -1 || col != -1); }
};

typedef QList<Square> SquareList;


// Represent a move in a reversi game.
//
// The move also contains information to also undo the move in a game,
// but this information is not valid until the move is applied to a
// position. After that it can be stored in e.g. an undolist.

class Move
{
public: 
    friend class Position;

    Move(Color col, Square sq)
        : color(col), row(sq.row), col(sq.col)
    { }
    Move(Color col = NoColor, int r = -1, int c = -1)
        : color(col), row(r), col(c)
    { }

    Square     square() const { return Square(row, col); }
    bool       isValid() const { return ( color != NoColor || row != -1 || col != -1 ); }
    SquareList turnedPieces() const { return m_turnedPieces; }
    void       clear() { m_turnedPieces.clear(); }

public: // for convenience
    Color color;
    int row;
    int col;

private:
    // The following information is used to restore the position when
    // calling undoMove(). It is only valid after the move is applied
    // to a position. 
    SquareList m_turnedPieces;
    Color      m_wasToMove;
};


/**
 *  ReversiPosition represents the current state of the game.
 *
 *  This includes the state of the board and whose turn it is to
 *  move. It does not include the game history, though.
 */
class Position
{
public:
    Position();
    ~Position();

    /**
     *  @return color of the chip at position [row, col]
     */
    Color colorAt(int row, int col) const;

    /**
     *  @return a color of the player to move in the current position.
     */
    Color toMove() const { return m_toMove; }

    /**
     *  @return score (number of chips) of the player
     */
    int playerScore(Color player) const;

    void setToMove(Color color) { m_toMove = color; }

    /**
     *  @return whether the game is over, i.e. if no side has any
     *  legal moves.
     */
    bool isGameOver() const;

    bool isLegalMove(Color color, int row, int col) const;
    bool isLegalMove(Move &move) const;

    /**
     *  @return whether color has a legal move in the current
     *  position. This only considers the board configuration and does
     *  not take into account the player to move.
     */
    bool hasLegalMove(Color color) const;

    /**
     *  @return a list of legal moves for color.
     */
    SquareList legalMoves(Color color) const;

    /**
     * This will make a move for color at row, col if that move is
     * legal.  If the move is not legal, then nothing will happen.
     * This function does not take into account who was to move in the
     * situation but after the move the player to move will be set to
     * the opponent of color.
     *
     * Return true if the move was legal and false otherwise.
     */
    bool makeMove(Move &move);

    /**
     * Undoes the move.
     * If checkFirst is true, it will check before doing anything is
     * the undo is possible. If false it will not do that. This means
     * that if checkFirst is false and the undo fails, the resulting
     * position may be garbled. 
     *
     * return true if the undo succeeded, false otherwise.
     */
    bool undoMove(Move &move, bool checkFirst = true);

private:

    /**
     *  Searches for a "chunk" of opponent pieces in direction dir.
     *
     *  We define a "chunk" of chips for color "C" as follows:
     *  (let "O" be the color of the opponent for color "C")
     *  CO[O]C <-- this is a chunk
     *  where [O] is one or more opponent's pieces
     */
    bool hasChunk(Color color, int row, int col, int dir) const;

    /**
     *  Sets the type of chip at (row,col)
     */
    void setChipColor(Color type, int row, int col);

    /**
     *  The board itself
     */
    Color m_board[8][8];

    /**
     *  Color of the player to move
     */
    Color m_toMove;

    // ----------------------------------------------------------------
    // Auxiliary information, which can be computed from the position
    // but which we keep track of anyway for convenience reasons.

    /**
     * The number of pieces for each player.
     *
     */
    int m_score[2];
};

} // namespace Reversi

#endif
