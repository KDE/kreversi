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

#ifndef __POSITION__H__
#define __POSITION__H__


#include "Move.h"
#include "Score.h"


class Position
{
public:
  Position();
  Position(Position &pos, SimpleMove &move);
  Position(Position &pos, Move &move);

  Position &operator=(Position &pos);

  void   constrCopy(Position &pos, SimpleMove &move);

  void   setupStartPosition();

  // Access methods
  Color  toMove()              const { return m_toMove;             }
  Color  color(uint x, uint y) const;
  uint   score(Color color)    const { return m_score.score(color); }

  // Moves in the current position.
  bool   moveIsPossible(Color color) const;
  bool   moveIsAtAllPossible()       const;
  bool   moveIsLegal(SimpleMove &move)     const;
  bool   doMove(SimpleMove &move, QValueList<char> *turned = 0);
  bool   doMove(Move &move);
  bool   undoMove(Move &move);

  MoveList  generateMoves(Color color) const;

  QString   asString() const;

private:
  // The actual position itself.  Use the simplest representation possible.
  Color  m_board[10][10];
  Color  m_toMove;

  // Some extra data
  Score       m_score;		// The number of pieces for each side.
};


#endif
