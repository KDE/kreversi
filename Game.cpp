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

// The class Game represents a complete or incomplete Othello game. It uses
// the classes Score and Move (and internally  Position).
// You can make moves, take back one move at a time, reset to initial position
// and get certain data on the current position.

//  Public functions:

//  public Game()
//     Creates a game with the initial position.

//  public void Reset()
//     Resets to the initial position.

//  public boolean MakeMove(Move m)
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


#include "Game.h"


Game::Game()
{
  m_positions[0].constrInit();
  m_movenumber = 0;
}


// Reset the Game to before the first move, i.e. a new game.
//

void Game::Reset()
{
  m_positions[0].constrInit();
  m_movenumber = 0;
}


// Make a move in the game, resulting in a new position.  
//
// If everything went well, return true.  Otherwise return false and
// do nothing.

bool Game::MakeMove(Move move)
{
  // Some sanity checks.
  if (move.color() == Nobody)   return false;
  if (toMove() != move.color()) return false;

  // Don't allow illegal moves.
  if (! m_positions[m_movenumber].moveIsLegal(move))
    return false;

  // Construct a new position from the previous position and the move.
  m_positions[m_movenumber+1].constrCopy(m_positions[m_movenumber], move);
  m_movenumber++;

  return true;
}


// Take back the last move.  
//
// Note: The removed move is not remembered, so a redo is not possible.
//

bool Game::TakeBackMove()
{
  if (m_movenumber == 0) 
    return false;

  m_movenumber--;

  return true;
}


// Return the color of the square at (x, y)
//

Color Game::color(uint x, uint y) const
{
  return m_positions[m_movenumber].color(x, y);
}


// Return the score, i.e. the number of pieces, for color at the
// current position.
//

uint Game::score(Color color) const
{
  return m_positions[m_movenumber].score(color);
}


// Return the last move made in the game.
//

Move Game::lastMove() const 
{
  return m_positions[m_movenumber].lastMove();
}


// Return true if the move is legal in the current position.
//

bool Game::moveIsLegal(Move move) const
{
  return m_positions[m_movenumber].moveIsLegal(move);
}


// Return true if the color can make a move in the current position.

bool Game::moveIsPossible(Color color) const
{
  return m_positions[m_movenumber].moveIsPossible(color);
}


// Return true if any side can make a move in the current position.
//

bool Game::moveIsAtAllPossible() const
{
  return m_positions[m_movenumber].moveIsAtAllPossible();
}


// Return the color to move in the current position.  This will be the
// same color that moved last time if the opponent can't make a move.
//
// If no side can move, then return Nobody.
//

Color Game::toMove() const
{
  if (m_movenumber == 0)
    return Black;

  Color  color    = lastMove().color();
  Color  opponent = ::opponent(color);

  if (moveIsPossible(opponent)) return opponent;
  if (moveIsPossible(color))    return color;

  return Nobody;
}


// Return true if the square at (x, y) was changed during the last move.
//

bool Game::squareModified(uint x, uint y) const 
{
  if (moveNumber() == 0)
    return true;
  else
    return (m_positions[m_movenumber].color(x, y) 
	    != m_positions[m_movenumber - 1].color(x, y));
}


// Return true if the piece at square (x, y) was turned during the last move.
//

bool Game::wasTurned(uint x, uint y) const 
{
  if (moveNumber() == 0)
    return false;
  else {
    Color c1 = m_positions[m_movenumber - 1].color(x, y);
    Color c2 = m_positions[m_movenumber].color(x, y);

    if (c1 == Nobody)
      return false;
    else if (c1 == c2)
      return false;
    else
      return true;
  }
}
