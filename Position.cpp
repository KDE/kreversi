
// This code is freely distributable, but may neither be sold nor used or
// included in any product sold for profit without permission from
// Mats Luthman.

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
  m_score.InitScore(2,2);
  
  for (int i=0; i<10; i++)
    for (int j=0; j<10; j++)
      m_board[i][j] = Score::NOBODY;
    
  m_board[4][4] = Score::WHITE;
  m_board[5][5] = Score::WHITE;
  m_board[5][4] = Score::BLACK;
  m_board[4][5] = Score::BLACK;
}

void Position::constrCopy(Position &p, Move &m) {
  m_score.InitScore(0, 0);

  // JAVA m_board = new int[10][10];

  /* JAVA for (int i=0; i<10; i++)
     System.arraycopy(p.m_board[i], 0, m_board[i], 0, 10); */
  for(int r = 0; r < 10; r++)
    for(int c = 0; c < 10; c++)
      m_board[r][c] = p.m_board[r][c];

  m_score.ScoreCopy(p.m_score);

  int player = m.GetPlayer();
  int opponent = Score::GetOpponent(player);

  m_board[m.GetX()][m.GetY()] = player;
  m_score.ScoreAdd(player, 1);

  for (int xinc=-1; xinc<=1; xinc++)
    for (int yinc=-1; yinc<=1; yinc++)
      if (xinc != 0 || yinc != 0)
	{
      int x, y;
      
      for (x = m.GetX()+xinc, y = m.GetY()+yinc; m_board[x][y] == opponent;
	   x += xinc, y += yinc)
	;
      
      if (m_board[x][y] == player)
	for (x -= xinc, y -= yinc; x != m.GetX() || y != m.GetY();
	     x -= xinc, y -= yinc)
	  {
	    m_board[x][y] = player;
	    m_score.ScoreAdd(player, 1);
	    m_score.ScoreSubtract(opponent, 1);
	  }
	}
  
  m_last_move.copy(m);
}

Position::Position()
{
  constrInit();
}


Position::Position(Position &p, Move &m)
{
  constrCopy(p,m);
}

Position::~Position() {
}

int Position::GetSquare(int x, int y) { 
  return m_board[x][y]; 
}

int Position::GetScore(int player) { return m_score.GetScore(player); }

Move Position::GetLastMove() { return m_last_move; }

bool Position::MoveIsLegal(Move m)
{
  if (m_board[m.GetX()][m.GetY()] != Score::NOBODY) return false;

  int player = m.GetPlayer();
  int opponent = Score::GetOpponent(player);

  for (int xinc=-1; xinc<=1; xinc++)
    for (int yinc=-1; yinc<=1; yinc++)
      if (xinc != 0 || yinc != 0)
	{
	  int x, y;
	  
	  for (x = m.GetX()+xinc, y = m.GetY()+yinc; m_board[x][y] == opponent;
	       x += xinc, y += yinc)
	    ;
	  
	  if (m_board[x][y] == player &&
	      (x - xinc != m.GetX() || y - yinc != m.GetY()))
	    return true;
	}
  
  return false;
}


bool Position::MoveIsPossible(int player)
{
  for (int i=1; i<9; i++)
    for (int j=1; j<9; j++)
      if (MoveIsLegal(Move(i, j, player))) return true;
  
  return false;
}


bool Position::MoveIsAtAllPossible()
{
  return (bool)(MoveIsPossible(Score::WHITE) || MoveIsPossible(Score::BLACK));
}
