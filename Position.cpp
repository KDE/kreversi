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

//  public boolean MoveIsLegal(Move m)
//     Checks if a move is legal.

//  public boolean MoveIsPossible(int player)
//     Checks if there is a legal move for player.

//  public boolean MoveIsAtAllPossible()
//     Checks if there are any legal moves at all.

#include "Position.h"
#include <stdlib.h>

void Position::constrInit() {
  m_score.set(White, 2);
  m_score.set(Black, 2);

  for (uint i=0; i<10; i++)
    for (uint j=0; j<10; j++)
      m_board[i][j] = Nobody;

  m_board[4][4] = White;
  m_board[5][5] = White;
  m_board[5][4] = Black;
  m_board[4][5] = Black;
}

void Position::constrCopy(Position &p, Move &m) {
  for(uint r = 0; r < 10; r++)
    for(uint c = 0; c < 10; c++)
      m_board[r][c] = p.m_board[r][c];

  m_score = p.m_score;

  Player player = m.player();
  Player opponent = ::opponent(player);

  m_board[m.x()][m.y()] = player;
  m_score.inc(player);

  for (int xinc=-1; xinc<=1; xinc++)
    for (int yinc=-1; yinc<=1; yinc++)
      if (xinc != 0 || yinc != 0)
	{
      int x, y;

      for (x = m.x()+xinc, y = m.y()+yinc; m_board[x][y] == opponent;
	   x += xinc, y += yinc)
	;

      if (m_board[x][y] == player)
	for (x -= xinc, y -= yinc; x != m.x() || y != m.y();
	     x -= xinc, y -= yinc)
	  {
	    m_board[x][y] = player;
	    m_score.inc(player);
	    m_score.dec(opponent);
	  }
	}

  m_last_move = m;
}

Position::Position()
{
  constrInit();
}


Position::Position(Position &p, Move &m)
{
  constrCopy(p,m);
}


Player Position::player(uint x, uint y) const {
  return m_board[x][y];
}

uint Position::score(Player player) const { return m_score.score(player); }

bool Position::moveIsLegal(Move m) const
{
  if (m_board[m.x()][m.y()] != Nobody) return false;

  Player player = m.player();
  Player opponent = ::opponent(player);

  for (int xinc=-1; xinc<=1; xinc++)
    for (int yinc=-1; yinc<=1; yinc++)
      if (xinc != 0 || yinc != 0)
	{
	  int x, y;

	  for (x = m.x()+xinc, y = m.y()+yinc; m_board[x][y] == opponent;
	       x += xinc, y += yinc)
	    ;

	  if (m_board[x][y] == player &&
	      (x - xinc != m.x() || y - yinc != m.y()))
	    return true;
	}

  return false;
}


bool Position::moveIsPossible(Player player) const
{
  for (uint i=1; i<9; i++)
    for (uint j=1; j<9; j++)
      if (moveIsLegal(Move(i, j, player))) return true;

  return false;
}


bool Position::moveIsAtAllPossible() const
{
  return (moveIsPossible(White) || moveIsPossible(Black));
}
