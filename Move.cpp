
// This code is freely distributable, but may neither be sold nor used or
// included in any product sold for profit without permission from
// Mats Luthman.

// The class Move is used to represent an Othello move with a player value
// (see class Score) and a pair of coordinates on an 8x8 Othello board.

#include "Move.h"

Move::Move(Move &m) 
{ 
  copy(m);
}

void Move::copy(Move &m) {
  m_x = m.m_x; 
  m_y = m.m_y; 
  m_player = m.m_player; 
}

Move::Move(int x, int y, int player) { 
  m_x = x; 
  m_y = y; 
  m_player =player; 
}

int Move::GetX() { return m_x; }
int Move::GetY() { return m_y; }
int Move::GetPlayer() { return m_player; }
