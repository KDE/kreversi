// -*- C++ -*-
// This code is freely distributable, but may neither be sold nor used or
// included in any product sold for profit without permission from
// Mats Luthman.

// The class Game represents a complete or incomplete Othello game. It uses
// the classes Score and Move (and internally  Position).
// You can make moves, take back one move at a time, reset to initial position
// and get certain data on the current position.

//  Public functions:

//  public Game()
//     Creates a game with the initial position.

//  public void Reset()
//     Resets to the initial position.

//  public boolean MakeMove(Move m)
//     Makes the move m. Returns false if the move is not legal or when called
//     with a move where the player is Score.NOBODY.

//  public boolean TakeBackMove()
//     Takes back a move. Returns true if not at the initial position.

//  public int GetSquare(int x, int y)
//     Returns the piece at (x, y). Returns Score.NOBODY if the square is not
//     occupied.

//  public int GetScore(int player)
//     Returns the score for player.

//  public Move GetLastMove()
//     Returns the last move. Returns null if at the initial position.

//  public boolean MoveIsLegal(Move m)
//     Checks if move m is legal.

//  public boolean MoveIsPossible(int player)
//     Checks if there is a legal move for player.

//  public boolean MoveIsAtAllPossible()
//     Checks if there are any legal moves at all.

//  public int GetMoveNumber()
//     Returns move number.

//  public int GetWhoseTurn()
//     Returns the player in turn to play (if there are no legal moves
//     Score.NOBODY is returned).

//  public Move[] TurnedByLastMove()
//     Returns a vector of the squares that were changed by the last move.
//     The move that was actually played is at index 0. At the initial
//     position the length of the vector returned is zero. (Could be used
//     for faster updates of a graphical board).


#ifndef __GAME__H__
#define __GAME__H__

#include "Score.h"
#include "Move.h"
#include "Position.h"

class Game { 
public:
  Game();
  void Reset();
  bool MakeMove(Move m);
  bool TakeBackMove();
  int GetSquare(int x, int y);
  int GetScore(int player);
  Move GetLastMove();
  bool MoveIsLegal(Move m);
  bool MoveIsPossible(int player);
  bool MoveIsAtAllPossible();
  int GetMoveNumber();
  int GetWhoseTurn();
  int GetWhoseTurnOpponent();
  bool squareModified(int x, int y);
  bool wasTurned(int x, int y);

private:
  Position m_positions[64];
  int m_movenumber;
};

#endif
