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
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *******************************************************************
 */

// The class Position is used to represent an Othello position as white and
// black pieces and empty squares (see class Score) on an 8x8 Othello board.


#include "kdebug.h"

#include "Position.h"
#include <stdlib.h>


Position::Position()
{
  setupStartPosition();
}


Position::Position(Position &pos, SimpleMove &move)
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

  m_score    = pos.m_score;

  return *this;
}


// ----------------------------------------------------------------
//                Helpers to the constructors


// Construct a Position by copying another one and then make a move.
//

void Position::constrCopy(Position &pos, SimpleMove &move)
{
  *this = pos;
  doMove(move);
}


// Setup the Position by setting it to the initial Reversi position.

void Position::setupStartPosition()
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

  // Each side starts out with two pieces.
  m_score.set(White, 2);
  m_score.set(Black, 2);
}


// ----------------------------------------------------------------
//                          Access methods


Color Position::color(uint x, uint y) const
{
  if (x < 1 || x > 8 || y < 1 || y > 8)
    return Nobody;

  return m_board[x][y];
}


// ----------------------------------------------------------------
//                      Moves in the position


// Return true if the move is legal.
//
// NOTE: This function does *NOT* test wether the move is done
//       by the color to move.  That must be checked separately.
//

bool Position::moveIsLegal(SimpleMove &move) const
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
      SimpleMove  move(color, i, j);

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
//
bool Position::doMove(SimpleMove &move, QValueList<char> *turned)
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
	  if (turned)
	    turned->append(10 * x + y);

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

  // Set the next color to move.
  if (moveIsPossible(opponent))
    m_toMove = opponent;
  else if (moveIsPossible(color))
    m_toMove = color;
  else
    m_toMove = Nobody;

  return true;
}


bool Position::doMove(Move &move)
{
  move.m_turnedPieces.clear();
  return doMove((SimpleMove &) move, &move.m_turnedPieces);
}


bool Position::undoMove(Move &move)
{
  Color  color = move.color();
  Color  other = opponent(color);

  // Sanity checks
  // 1. The move must be on the board and be of the right color.
  if (color != m_board[move.x()][move.y()]) {
    //kdDebug() << "move on the board is wrong color: " << (int) color << "["
    //      << move.x() << "," << move.y() << "]" << endl;
    return false;
  }

  // 2. All turned pieces must be on the board anb be of the right color.
  QValueList<char>::iterator  it;
  for (it = move.m_turnedPieces.begin(); 
       it != move.m_turnedPieces.end(); 
       ++it) {
    int  sq = *it;

    if (m_board[sq / 10][sq % 10] != color) {
      //kdDebug() << "turned piece the board is wrong color: [" 
      //	<< sq / 10 << "," << sq % 10 << "]" << endl;
      return false;
    }
  }

  // Ok, everything seems allright.  Let's do it!
  // 1. Unturn all the turned pieces.
  for (it = move.m_turnedPieces.begin(); 
       it != move.m_turnedPieces.end(); 
       ++it) {
    int  sq = *it;

    m_board[sq / 10][sq % 10] = other;
    m_score.dec(color);
    m_score.inc(other);
  }

  // 2. Remove the move itself.
  m_score.dec(color);
  m_board[move.x()][move.y()] = Nobody;


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


QString Position::asString() const
{
  QString  result;

  for (uint y = 1; y < 9; ++y) {
    for (uint x = 1; x < 9; ++x) {
      switch (m_board[x][y]) {
      case Nobody:  result.append(' ');  break;
      case Black:   result.append('*');  break;
      case White:   result.append('o');  break;
      default:      result.append('?');  break;
      }
    }

    result.append('\n');
  }

  return result;
}
