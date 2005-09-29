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

#ifndef __SCORE__H__
#define __SCORE__H__

#include <qglobal.h>

enum Color { White = 0, Black = 1, NbColors = 2, Nobody = NbColors };

Color opponent(Color color);


/* This class keeps track of the score for both colors.  Such a score
 * could be either the number of pieces, the score from the evaluation
 * function or anything similar.
 */

class Score {
public:
  Score();

  uint score(Color color) const     { return m_score[color]; }

  void set(Color color, uint score) { m_score[color] = score; }
  void inc(Color color)             { m_score[color]++; }
  void dec(Color color)             { m_score[color]--; }
  void add(Color color, uint s)     { m_score[color] += s; }
  void sub(Color color, uint s)     { m_score[color] -= s; }
  
private:
  uint  m_score[NbColors];
};

#endif
