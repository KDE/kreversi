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

// The class Score maintains the score for two players. The constants
// BLACK and WHITE are used to access the scores for each player. The constant
// NOBODY can be used in any context where a value other than WHITE and BLACK
// is meaningful. The function GetOpponent() transforms WHITE to BLACK and
// vice versa. The other funcions are self explanatory.

#ifndef __SCORE__H__
#define __SCORE__H__

#include "misc.h"

class Score {
public:
  Score();
  Score(int whitescore, int blackscore);
  void ScoreCopy(Score &s);
  void InitScore(int w, int b) { SetScore(Score::BLACK, b); SetScore(Score::WHITE, w); }
  int  GetScore(int player);
  void SetScore(int player, int score);
  void ScoreAdd(int player, int amount);
  void ScoreSubtract(int player, int amount);

  static int GetOpponent(int player);
  
private:
  int m_score[2];

public:
  static const int NOBODY;
  static const int WHITE;
  static const int BLACK;
};

#endif
