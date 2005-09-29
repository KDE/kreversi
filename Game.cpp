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

// The class Game represents a complete or incomplete Othello game. It uses
// the classes Score and Move (and internally  Position).
// You can make moves, take back one move at a time, reset to initial position
// and get certain data on the current position.

//  Public functions:

//  public Game()
//     Creates a game with the initial position.

//  public void Reset()
//     Resets to the initial position.

//  public boolean makeMove(Move &move)
//     Makes the move m. Returns false if the move is not legal or when called
//     with a move where the player is Score.NOBODY.

//  public boolean TakeBackMove()
//     Takes back a move. Returns true if not at the initial position.

//  public int GetSquare(int x, int y)
//     Returns the piece at (x, y). Returns Score.NOBODY if the square is not
//     occupied.

//  public int GetScore(int player)
//     Returns the score for player.

//  public Move GetLastMove()
//     Returns the last move. Returns null if at the initial position.

//  public boolean MoveIsLegal(Move m)
//     Checks if move m is legal.

//  public boolean MoveIsPossible(int player)
//     Checks if there is a legal move for player.

//  public boolean MoveIsAtAllPossible()
//     Checks if there are any legal moves at all.

//  public int GetMoveNumber()
//     Returns move number.

//  public int GetWhoseTurn()
//     Returns the player in turn to play (if there are no legal moves
//     Score.NOBODY is returned).

//  public Move[] TurnedByLastMove()
//     Returns a vector of the squares that were changed by the last move.
//     The move that was actually played is at index 0. At the initial
//     position the length of the vector returned is zero. (Could be used
//     for faster updates of a graphical board).


#include <assert.h>

#include "Game.h"



Game::Game()
{
  newGame();
}

Game::~Game()
{
}


// Start a new game and reset the position to before the first move.
//

void Game::newGame()
{
  m_position.setupStartPosition();
  m_moveNumber = 0;
}


// Return the last move made in the game.
//

Move Game::lastMove() const 
{
  // If no moves where made, return a NULL move.
  if (m_moveNumber == 0)
    return Move();

  return m_moves[m_moveNumber - 1];
}


Move
Game::move(uint moveNo) const
{
  assert(moveNo < m_moveNumber);

  return m_moves[moveNo];
}



// Return true if the move is legal in the current position.
//

bool Game::moveIsLegal(SimpleMove &move) const
{
  return m_position.moveIsLegal(move);
}


// Return true if the color can make a move in the current position.

bool Game::moveIsPossible(Color color) const
{
  return m_position.moveIsPossible(color);
}


// Return true if any side can make a move in the current position.
//

bool Game::moveIsAtAllPossible() const
{
  return m_position.moveIsAtAllPossible();
}


// Make a move in the game, resulting in a new position.  
//
// If everything went well, return true.  Otherwise return false and
// do nothing.

bool Game::doMove(Move &move)
{
  Position  lastPos = m_position;

  // Some sanity checks.
  if (move.color() == Nobody)
    return false;

  if (toMove() != move.color())
    return false;

  // Make the move in the position and store it.  Don't allow illegal moves.
  if (!m_position.doMove(move))
    return false;

  m_moves[m_moveNumber++] = move;

  return true;
}


bool Game::doMove(SimpleMove &smove)
{
  Move  move(smove);

  return doMove(move);
}


// Take back the last move.  
//
// Note: The removed move is not remembered, so a redo is not possible.
//

bool Game::undoMove()
{
  if (m_moveNumber == 0) 
    return false;

#if 0
  m_position.setupStartPosition();
  m_moveNumber--;
  for (uint i = 0; i < m_moveNumber; i++)
    m_position.doMove(m_moves[i]);
#else
  m_position.undoMove(m_moves[--m_moveNumber]);
#endif

  return true;
}


// ----------------------------------------------------------------
//                    Reversi specific methods


// Return true if the square at (x, y) was changed during the last move.
//

bool Game::squareModified(uint x, uint y) const 
{
  // If the move number is zero, we want to redraw all squares.
  // That's why we return true here.
  if (m_moveNumber == 0)
    return true;

  return m_moves[m_moveNumber - 1].squareModified(x, y);
}


// Return true if the piece at square (x, y) was turned during the last move.
//

bool Game::wasTurned(uint x, uint y) const 
{
  // Nothing turned before the first move.
  if (m_moveNumber == 0)
    return false;

  Color color = m_position.color(x, y);

  if (color == Nobody)
    return false;

  return m_moves[m_moveNumber - 1].wasTurned(x, y);
}
