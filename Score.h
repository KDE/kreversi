// -*- C++ -*-
// This code is freely distributable, but may neither be sold nor used or
// included in any product sold for profit without permission from
// Mats Luthman.

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
