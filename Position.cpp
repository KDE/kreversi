/* Yo Emacs, this -*- C++ -*-
 *******************************************************************
 *******************************************************************
 *
 *
 * KREVERSI
 *
 *
 *******************************************************************
 *
 * A Reversi (or sometimes called Othello) game
 *
 *******************************************************************
 *
 * Created 1997 by Mario Weilguni <mweilguni@sime.com>. This file
 * is ported from Mats Luthman's <Mats.Luthman@sylog.se> JAVA applet.
 * Many thanks to Mr. Luthman who has allowed me to put this port
 * under the GNU GPL. Without his wonderful game engine kreversi
 * would be just another of those Reversi programs a five year old
 * child could beat easily. But with it it's a worthy opponent!
 *
 * If you are interested on the JAVA applet of Mr. Luthman take a
 * look at http://www.sylog.se/~mats/
 *
 *******************************************************************
 *
 * This file is part of the KDE project "KREVERSI"
 *
 * KREVERSI is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * KREVERSI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KREVERSI; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *******************************************************************
 */

// The class Position is used to represent an Othello position as white and
// black pieces and empty squares (see class Score) on an 8x8 Othello board.
// It also stores information on the move that lead to the position.

//  Public functions:

//  public Position()
//     Creates an initial position.

//  public Position(Position p, Move m)
//     Creates the position that arise when the Move m is applied to the
//     Position p (m must be a legal move).

//  public int GetSquare(int x, int y)
//     Returns the color of the piece at the square (x, y) (Score.WHITE,
//     Score.BLACK or Score.NOBODY).

//  public int GetScore(int player) { return m_score.GetScore(player); }
//     Returns the the current number of pieces of color player.

//  public Move GetLastMove()
//     Returns the last move.

//  public boolean moveIsLegal(Move move)
//     Checks if a move is legal.

//  public boolean moveIsPossible(int player)
//     Checks if there is a legal move for player.

//  public boolean moveIsAtAllPossible()
//     Checks if there are any legal moves at all.


#include "Position.h"
#include <stdlib.h>


Position::Position()
{
  constrInit();
}


Position::Position(Position &pos, Move &move)
{
  constrCopy(pos, move);
}


Position &Position::operator=(Position &pos)
{
  // Copy the position itself.
  for (uint row = 0; row < 10; row++)
    for (uint col = 0; col < 10; col++)
      m_board[row][col] = pos.m_board[row][col];
  m_toMove = pos.m_toMove;

  m_lastMove = pos.m_lastMove;
  m_score    = pos.m_score;

  return *this;
}


// ----------------------------------------------------------------
//                Helpers to the constructors


// Construct a Position by setting it to the initial Reversi position.

void Position::constrInit()
{
  // Initialize the real board
  for (uint i = 0; i < 10; i++)
    for (uint j = 0; j < 10; j++)
      m_board[i][j] = Nobody;

  // The initial position
  m_board[4][4] = White;
  m_board[5][5] = White;
  m_board[5][4] = Black;
  m_board[4][5] = Black;

  // Black always starts the game in Reversi.
  m_toMove = Black;

  // Empty move;
  m_lastMove = Move();

  // Each side starts out with two pieces.
  m_score.set(White, 2);
  m_score.set(Black, 2);
}


// Construct a Position by copying another one and then make a move.
//

void Position::constrCopy(Position &pos, Move &move)
{
  *this = pos;
  makeMove(move);
}


// ----------------------------------------------------------------
//                          Access methods



Color Position::color(uint x, uint y) const
{
  return m_board[x][y];
}


uint Position::score(Color color) const 
{
  return m_score.score(color);
}


// Return TRUE if the move is legal.
//
// NOTE: This function does *NOT* test wether the move is done
//       by the color to move.  That must be checked separately.
//

bool Position::moveIsLegal(Move &move) const
{
  if (m_board[move.x()][move.y()] != Nobody) 
    return false;

  Color  color    = move.color();
  Color  opponent = ::opponent(color);

  // Check in all directions and see if there is a turnable row of
  // opponent pieces.  If there is at least one such row, then the
  // move is legal.
  for (int xinc = -1; xinc <= 1; xinc++) {
    for (int yinc = -1; yinc <= 1; yinc++) {
      int  x, y;

      if (xinc == 0 && yinc == 0)
	continue;

      // Find the end of such a row of pieces.
      for (x = move.x()+xinc, y = move.y()+yinc; m_board[x][y] == opponent;
	   x += xinc, y += yinc)
	;

      // If the row ends with a piece of our own and there was at
      // least one opponent piece between it and the move point, then
      // we have found a turnable row.
      if (m_board[x][y] == color 
	  && (x - xinc != move.x() || y - yinc != move.y()))
	return true;
    }
  }

  return false;
}


// See if 'color' can make a move in the current position.  This is
// independent of wether it is 'color's turn to move or not.

bool Position::moveIsPossible(Color color) const
{
  // Make it simple: Step through all squares and see if it is a legal move.
  for (uint i = 1; i < 9; i++)
    for (uint j = 1; j < 9; j++) {
      Move  move(color, i, j);

      if (moveIsLegal(move)) 
	return true;
    }

  return false;
}


// Return true if any side can move.  (If not, the game is over.)

bool Position::moveIsAtAllPossible() const
{
  return (moveIsPossible(White) || moveIsPossible(Black));
}


// Make a move in the position.  
//
// Return true if the move was legal, otherwise return false.
bool Position::makeMove(Move &move)
{
  if (move.color() == Nobody)
    return false;

  Color  color    = move.color();
  Color  opponent = ::opponent(color);

  // Put the piece on the board
  m_board[move.x()][move.y()] = color;
  m_score.inc(color);

  // Turn pieces.
  uint  scoreBefore = m_score.score(color);
  for (int xinc = -1; xinc <= 1; xinc++) {
    for (int yinc = -1; yinc <= 1; yinc++) {
      int x, y;

      // Skip the case where both xinc and yinc == 0, since then we
      // won't move in any direction at all.
      if (xinc == 0 && yinc == 0)
	continue;

      // Find the end point (x, y) of a possible row of turnable pieces.
      for (x = move.x()+xinc, y = move.y()+yinc; m_board[x][y] == opponent;
	   x += xinc, y += yinc)
	;

      // If the row was indeed turnable, then do it.
      if (m_board[x][y] == color) {
	for (x -= xinc, y -= yinc; x != move.x() || y != move.y();
	     x -= xinc, y -= yinc) {
	  // Turn the piece.
	  m_board[x][y] = color;

	  // Make the piece count correct again.
	  m_score.inc(color);
	  m_score.dec(opponent);
	}
      }
    }
  }

  // If nothing was turned, the move wasn't legal.
  if (m_score.score(color) == scoreBefore) {
    m_board[move.x()][move.y()] = Nobody;
    m_score.dec(color);

    return false;
  }

  // Store the last move that lead to the position.
  m_lastMove = move;

  // Set the next color to move.
  if (moveIsPossible(opponent))
    m_toMove = opponent;
  else if (moveIsPossible(color))
    m_toMove = color;
  else
    m_toMove = Nobody;

  return true;
}


MoveList  Position::generateMoves(Color color) const
{
  MoveList  moves;

  // Make it simple: Step through all squares and see if it is a legal move.
  for (uint i = 1; i < 9; i++) {
    for (uint j = 1; j < 9; j++) {
      Move  move(color, i, j);

      if (moveIsLegal(move)) 
	moves.append(move);
    }
  }

  return moves;
}
