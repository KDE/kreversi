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

// This file defines the two classes SimpleMove and Move.
//
// The class Move is used to represent an Othello move with a player value
// (see class Score) and a pair of coordinates on an 8x8 Othello board.
// Each coordinate can have values between 1 and 8, inclusive.
// 
// The difference between a Move and a SimpleMove is that a SimpleMove
// can be done (performed) in a Position, but a Move can be both done
// and undone.  In addition to the info in SimpleMove, the class Move
// stores information that is used in undoing the move and visualizing
// it.
//
// The reason for the class SimpleMove is that it saves memory.  The
// class Game stores an array of Moves, since the BoardView needs
// information about which pieces were turned by the move.
//


#ifndef __MOVE__H__
#define __MOVE__H__


#include "qvaluelist.h"
#include "qstring.h"

#include "Score.h"


class Position;


class SimpleMove
{
public:
  SimpleMove()   { m_color = Nobody; m_x = -1; m_y = -1; }
  SimpleMove(Color color, int x, int y);
  SimpleMove(const SimpleMove &move);

  //Move &operator=(Move &move);

  Color color() const { return m_color; }
  int   x()     const { return m_x; }
  int   y()     const { return m_y; }

  QString  asString() const;

protected:
  Color  m_color;
  int    m_x;
  int    m_y;
};


// Note: This class is not memory optimized.  The list of turned
//       pieces can surely be made much smaller.

class Move : public SimpleMove
{
  friend class Position;

public:
  Move();
  Move(Color color, int x, int y);
  Move(const Move &move);
  Move(const SimpleMove &move);

  bool   squareModified(uint x, uint y) const;
  bool   wasTurned(uint x, uint y)      const;

private:
  QValueList<char>  m_turnedPieces;
};


typedef QValueList<Move>            MoveList;


#endif
