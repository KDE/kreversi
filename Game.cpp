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


#include <qobject.h>
#include "Game.h"
#include "misc.h"

Game::Game()
{
  // JAVA m_positions = new Position[61];
  // JAVA m_positions[0] = new Position();
  m_positions[0].constrInit();
  m_movenumber = 0;
}

void Game::Reset()
{
  // JAVA m_positions = new Position[61];
  // JAVA m_positions[0] = new Position();
  m_positions[0].constrInit();
  m_movenumber = 0;
}


bool Game::MakeMove(Move m)
{
  if (m.GetPlayer() == Score::NOBODY) return false;
  if (GetWhoseTurn() != m.GetPlayer()) return false;
  if (! m_positions[m_movenumber].MoveIsLegal(m)) return false;
  
  m_positions[m_movenumber+1].constrCopy(m_positions[m_movenumber], m);
  m_movenumber++;

  return true;
}


bool Game::TakeBackMove()
{
  if (m_movenumber <= 0) return false;

  // JAVA m_positions[m_movenumber--] = null;
  m_movenumber--;

  return true;
}


int Game::GetSquare(int x, int y)
{
  return m_positions[m_movenumber].GetSquare(x, y);
}


int Game::GetScore(int player)
{
  return m_positions[m_movenumber].GetScore(player);
}


Move Game::GetLastMove() { return m_positions[m_movenumber].GetLastMove(); }


bool Game::MoveIsLegal(Move m)
{
  return m_positions[m_movenumber].MoveIsLegal(m);
}


bool Game::MoveIsPossible(int player)
{
  return m_positions[m_movenumber].MoveIsPossible(player);
}


bool Game::MoveIsAtAllPossible()
{
  return m_positions[m_movenumber].MoveIsAtAllPossible();
}


int Game::GetMoveNumber() { return m_movenumber; }


int Game::GetWhoseTurn()
{
  if (m_movenumber <= 0) return Score::BLACK;
  
  int player = GetLastMove().GetPlayer();
  int opponent = Score::GetOpponent(player);
  
    if (MoveIsPossible(opponent)) return opponent;
    
    if (MoveIsPossible(player)) return player;
    
    return Score::NOBODY;
}

int Game::GetWhoseTurnOpponent() {
  int color = GetWhoseTurn();
  if(color == Score::NOBODY)
    return Score::NOBODY;
  else if(color == Score::BLACK)
    return Score::WHITE;
  else
    return Score::BLACK;
}

bool Game::squareModified(int x, int y) {
  if(GetMoveNumber() == 1)
    return TRUE;
  else
    return (bool)(m_positions[m_movenumber].GetSquare(x, y) != m_positions[m_movenumber-1].GetSquare(x, y));
}

bool Game::wasTurned(int x, int y) {
  if(GetMoveNumber() == 0)
    return FALSE;
  else {
    int c1 = m_positions[m_movenumber-1].GetSquare(x, y);
    int c2 = m_positions[m_movenumber].GetSquare(x, y);
    
    if(c1 == Score::NOBODY)
      return FALSE;
    else if(c1 == c2)
      return FALSE;
    else
      return TRUE;
  }
}
