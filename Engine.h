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

// The class Engine produces moves from a Game object through calls to the
// function ComputeMove().
//
// First of all: this is meant to be a simple example of a game playing
// program. Not everything is done in the most clever way, particularly not
// the way the moves are searched, but it is hopefully made in a way that makes
// it easy to understand. The function ComputeMove2() that does all the work
// is actually not much more than a hundred lines. Much could be done to
// make the search faster though, I'm perfectly aware of that. Feel free
// to experiment.
//
// The method used to generate the moves is called minimax tree search with
// alpha-beta pruning to a fixed depth. In short this means that all possible
// moves a predefined number of moves ahead are either searched or refuted
// with a method called alpha-beta pruning. A more thorough explanation of
// this method could be found at the world wide web at http:
// //yoda.cis.temple.edu:8080/UGAIWWW/lectures96/search/minimax/alpha-beta.html
// at the time this was written. Searching for "minimax" would also point
// you to information on this subject. It is probably possible to understand
// this method by reading the source code though, it is not that complicated.
//
// At every leaf node at the search tree, the resulting position is evaluated.
// Two things are considered when evaluating a position: the number of pieces
// of each color and at which squares the pieces are located. Pieces at the
// corners are valuable and give a high value, and having pieces at squares
// next to a corner is not very good and they give a lower value. In the
// beginning of a game it is more important to have pieces on "good" squares,
// but towards the end the total number of pieces of each color is given a
// higher weight. Other things, like how many legal moves that can be made in a
// position, and the number of pieces that can never be turned would probably
// make the program stronger if they were considered in evaluating a position,
// but that would make things more complicated (this was meant to be very
// simple example) and would also slow down computation (considerably?).
//
// The member m_board[10][10]) holds the current position during the
// computation. It is initiated at the start of ComputeMove() and
// every move that is made during the search is made on this board. It should
// be noted that 1 to 8 is used for the actual board, but 0 and 9 can be
// used too (they are always empty). This is practical when turning pieces
// when moves are made on the board. Every piece that is put on the board
// or turned is saved in the stack m_squarestack (see class SquareStack) so
// every move can easily be reversed after the search in a node is completed.
//
// The member m_bc_board[][] holds board control values for each square
// and is initiated by a call to the function private void SetupBcBoard()
// from Engines constructor. It is used in evaluation of positions except
// when the game tree is searched all the way to the end of the game.
//
// The two members m_coord_bit[9][9] and m_neighbor_bits[9][9] are used to
// speed up the tree search. This goes against the principle of keeping things
// simple, but to understand the program you do not need to understand them
// at all. They are there to make it possible to throw away moves where
// the piece that is played is not adjacent to a piece of opposite color
// at an early stage (because they could never be legal). It should be
// pointed out that not all moves that pass this test are legal, there will
// just be fewer moves that have to be tested in a more time consuming way.
//
// There are also two other members that should be mentioned: Score m_score
// and Score m_bc_score. They hold the number of pieces of each color and
// the sum of the board control values for each color during the search
// (this is faster than counting at every leaf node).
//

// The classes SquareStackEntry and SquareStack implement a
// stack that is used by Engine to store pieces that are turned during
// searching (see ComputeMove()).
//
// The class MoveAndValue is used by Engine to store all possible moves
// at the first level and the values that were calculated for them.
// This makes it possible to select a random move among those with equal
// or nearly equal value after the search is completed.

#ifndef __ENGINE__H__
#define __ENGINE__H__

#include "SuperEngine.h"
#include "Position.h"
#include "Game.h"
#include "Move.h"
#include "Score.h"
#include <qmemarray.h>
#include <sys/times.h>
#include <qbitarray.h>


// Class ULONG64 is used as a bitmap for the squares.

#if defined(__GNUC__)
#define ULONG64 unsigned long long int
#else
class ULONG64 : public QBitArray {
public:
  ULONG64();
  ULONG64( unsigned int );
  void shl();
};
#endif


// SquareStackEntry and SquareStack are used during search to keep
// track of turned pieces.

class SquareStackEntry
{
public:
  SquareStackEntry();

  void setXY(int x, int y);

public:
  int  m_x;
  int  m_y;
};


class SquareStack
{
public:
  SquareStack();
  SquareStack(int size);

  void              resize(int size);
  void              init(int size);
  SquareStackEntry  Pop();
  void              Push(int x, int y);

private:
  QMemArray<SquareStackEntry>  m_squarestack;
  int                          m_top;
};


// Connect a move with its value.

class MoveAndValue
{
public:
  MoveAndValue();
  MoveAndValue(int x, int y, int value);

  void  setXYV(int x, int y, int value);

public:
  int  m_x;
  int  m_y;
  int  m_value;
};


// The real beef of this program: the engine that finds good moves for
// the computer player.
//
class Engine : public SuperEngine {
public:
  Engine(int st, int sd);
  Engine(int st);
  Engine();

  Move     computeMove(Game *game, bool competitive);

private:
  Move     ComputeFirstMove(Game *game);
  int      ComputeMove2(int xplay, int yplay, Color color, int level,
			int cutoffval,
			ULONG64 colorbits, ULONG64 opponentbits);

  int      TryAllMoves(Color opponent, int level, int cutoffval,
		       ULONG64 opponentbits, ULONG64 colorbits);

  int      EvaluatePosition(Color color);
  void     SetupBcBoard();
  void     SetupBits();
  int      CalcBcScore(Color color);
  ULONG64  ComputeOccupiedBits(Color color);

  void yield();

private:
  static const int  LARGEINT;
  static const int  ILLEGAL_VALUE;
  static const int  BC_WEIGHT;

  Color        m_board[10][10];
  int          m_bc_board[9][9];
  Score        m_score;
  Score        m_bc_score;
  SquareStack  m_squarestack;
  
  int          m_depth;
  int          m_coeff;
  int          m_nodes_searched;
  bool         m_exhaustive;
  bool         m_competitive;

  ULONG64      m_coord_bit[9][9];
  ULONG64      m_neighbor_bits[9][9];
};

#endif
