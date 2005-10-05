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

#include "Move.h"


SimpleMove::SimpleMove(Color color, int x, int y)
{
  m_color = color;
  m_x     = x;
  m_y     = y;
}


SimpleMove::SimpleMove(const SimpleMove &move)
{
  *this = move;
}


QString SimpleMove::asString() const
{
  if (m_x == -1)
    return QString("pass");
  else
    return QString("%1%2").arg(" ABCDEFGH"[m_x]).arg(" 12345678"[m_y]);
}


// ================================================================


Move::Move()
  : SimpleMove()
{
  m_turnedPieces.clear();
}


Move::Move(Color color, int x, int y)
  : SimpleMove(color, x, y)
{
  m_turnedPieces.clear();
}


Move::Move(const Move &move)
  : SimpleMove((SimpleMove&) move)
{
  m_turnedPieces.clear();
}


Move::Move(const SimpleMove &move)
  : SimpleMove(move)
{
  m_turnedPieces.clear();
}


// ----------------------------------------------------------------


bool Move::squareModified(uint x, uint y) const
{
  return (m_x == (int) x && m_y == (int) y) || wasTurned(x, y);
}


bool Move::wasTurned(uint x, uint y) const
{
  // findIndex returns the first index where the item is found, or -1
  // if not found.
  return (m_turnedPieces.findIndex(10 * x + y) != -1);
}
