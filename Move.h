// -*- C++ -*-
// This code is freely distributable, but may neither be sold nor used or
// included in any product sold for profit without permission from
// Mats Luthman.

// The class Move is used to represent an Othello move with a player value
// (see class Score) and a pair of coordinates on an 8x8 Othello board.

#ifndef __MOVE__H__
#define __MOVE__H__

class Move
{
public:
  Move() { m_x = -1; m_y = -1; m_player = -1; }
  Move(Move &m);
  Move(int x, int y, int player);
  void copy(Move &m);

  int GetX();
  int GetY();
  int GetPlayer();

private:
  int m_x, m_y;
  int m_player;
};

#endif
