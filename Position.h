// -*- C++ -*-
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


#ifndef __POSITION__H__
#define __POSITION__H__

#include "Move.h"
#include "Score.h"

class Position
{
public:
  Position();
  Position(Position &p, Move &m);
  ~Position();

  void constrInit();
  void constrCopy(Position &p, Move &m);

  int GetSquare(int x, int y);
  int GetScore(int player);
  Move GetLastMove();
  bool MoveIsLegal(Move m);
  bool MoveIsPossible(int player);
  bool MoveIsAtAllPossible();

private:
  int m_board[10][10];
  Move m_last_move;
  Score m_score;
};

#endif
