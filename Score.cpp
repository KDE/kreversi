
// This code is freely distributable, but may neither be sold nor used or
// included in any product sold for profit without permission from
// Mats Luthman.

// The class Score maintains the score for two players. The constants
// BLACK and WHITE are used to access the scores for each player. The constant
// NOBODY can be used in any context where a value other than WHITE and BLACK
// is meaningful. The function GetOpponent() transforms WHITE to BLACK and
// vice versa. The other funcions are self explanatory.

#include "Score.h"

const int Score::NOBODY=0;
const int Score::WHITE=1;
const int Score::BLACK=2;

Score::Score() 
{ 
  m_score[0] = 0; 
  m_score[1] = 0;
}

Score::Score(int whitescore, int blackscore) 
{
  m_score[WHITE-1] = whitescore;
  m_score[BLACK-1] = blackscore;
}


int Score::GetOpponent(int player) { return player % 2 + 1; }


void Score::ScoreCopy(Score &s)
{
  m_score[0] = s.m_score[0];
  m_score[1] = s.m_score[1];
}


int Score::GetScore(int player) { return m_score[player-1]; }


void Score::SetScore(int player, int score) { m_score[player-1] = score; }


void Score::ScoreAdd(int player, int amount)
{
  m_score[player-1] += amount;
}


void Score::ScoreSubtract(int player, int amount)
{
  m_score[player-1] -= amount;
}
