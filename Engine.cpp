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


#include "Engine.h"
#include <qapp.h>

const int Engine::LARGEINT = 99999;
const int Engine::ILLEGAL_VALUE = 8888888;
const int Engine::BC_WEIGHT = 3;

inline void SquareStackEntry::setXY(int x, int y) { 
  m_x = x; 
  m_y = y; 
}

#if !defined(__GNUC__)

ULONG64::ULONG64() : QBitArray(64) {
  fill(0);
}

ULONG64::ULONG64( unsigned int value ) : QBitArray(64) {
  fill(0);
  for(int i = 0; i < 32; i++) {
    setBit(i, (bool)(value & 1));
    value >>= 1;
  }
}

void ULONG64::shl() {
  for(int i = 63; i > 0; i--)
    setBit(i, testBit(i-1));
  setBit(0, 0);
}

#endif


SquareStackEntry::SquareStackEntry() { 
  setXY(0,0); 
}


SquareStack::SquareStack() {
  init(0);
}


SquareStack::SquareStack(int size) {
  init(size);
}


void SquareStack::resize(int size) {
  m_squarestack.resize(size);
}


void SquareStack::init(int size) { 
  resize(size); 
  m_top = 0;     
  for (int i=0; i<size; i++) 
    m_squarestack[i].setXY(0,0);
}


inline SquareStackEntry SquareStack::Pop() { 
  return m_squarestack[--m_top]; 
}


inline void SquareStack::Push(int x, int y) {
  m_squarestack[m_top].m_x = x;
  m_squarestack[m_top++].m_y = y;
}


inline void MoveAndValue::setXYV(int x, int y, int value) {
  m_x = x;
  m_y = y;
  m_value = value;
}


MoveAndValue::MoveAndValue() {
  setXYV(0,0,0);
}


MoveAndValue::MoveAndValue(int x, int y, int value) {
  setXYV(x, y, value);
}


Engine::Engine(int st, int sd) : SuperEngine(st, sd) {
  SetupBcBoard(); 
  SetupBits(); 
}


Engine::Engine(int st) : SuperEngine(st) { 
  SetupBcBoard(); 
  SetupBits(); 
}


Engine::Engine() : SuperEngine(5) { 
  SetupBcBoard(); 
  SetupBits(); 
}


// keep GUI alive
void Engine::yield() {
  qApp->processEvents();
}


Move Engine::ComputeMove(Game g) {
  m_exhaustive = false;

  int player = g.GetWhoseTurn();

  if (player == Score::NOBODY) 
    return Move(-1,-1,-1);

  m_score.InitScore(g.GetScore(Score::WHITE), g.GetScore(Score::BLACK));
  if (m_score.GetScore(Score::WHITE) + m_score.GetScore(Score::BLACK) == 4)
    return ComputeFirstMove(g);

  // JAVA m_board = new int[10][10];
  //m_squarestack = new SquareStack(3000); // More than enough...
  m_squarestack.init(3000);
  m_depth = m_strength;

  if (m_score.GetScore(Score::WHITE) + m_score.GetScore(Score::BLACK) +
      m_depth + 4 >= 64)
    m_depth =
      64 - m_score.GetScore(Score::WHITE) - m_score.GetScore(Score::BLACK);
  else if (m_score.GetScore(Score::WHITE) + m_score.GetScore(Score::BLACK) +
	   m_depth + 7 >= 64)
    m_depth += 3;
  else if (m_score.GetScore(Score::WHITE) + m_score.GetScore(Score::BLACK) +
	   m_depth + 9 >= 64)
    m_depth += 2;
  else if (m_score.GetScore(Score::WHITE) + m_score.GetScore(Score::BLACK) +
	   m_depth + 11 >= 64)
    m_depth++;

  if (m_score.GetScore(Score::WHITE) + m_score.GetScore(Score::BLACK) +
      m_depth >= 64) m_exhaustive = true;

  m_coeff =
    100 - (100*
	   (m_score.GetScore(Score::WHITE) + m_score.GetScore(Score::BLACK) +
	    m_depth - 4))/60;

  m_nodes_searched = 0;

  for (int x=0; x<10; x++)
    for (int y=0; y<10; y++)
      m_board[x][y] = Score::NOBODY;

  for (int x=1; x<9; x++)
    for (int y=1; y<9; y++)
      m_board[x][y] = g.GetSquare(x, y);

  m_bc_score.InitScore(CalcBcScore(Score::WHITE), CalcBcScore(Score::BLACK));

  ULONG64 playerbits = ComputeOccupiedBits(player);
  ULONG64 opponentbits = ComputeOccupiedBits(Score::GetOpponent(player));

  int maxval = -LARGEINT;
  int max_x = 0;
  int max_y = 0;

  MoveAndValue moves[60];
  int number_of_moves = 0;
  int number_of_maxval = 0;

  SetInterrupt(false);

  ULONG64 null_bits;
  null_bits = 0;

  //struct tms tmsdummy; 
  //long starttime = times(&tmsdummy);

  for (int x=1; x<9; x++)
    for (int y=1; y<9; y++)
      if (m_board[x][y] == Score::NOBODY &&
	  (m_neighbor_bits[x][y] & opponentbits) != null_bits)
	{

	  int val = ComputeMove2(x, y, player, 1, maxval, 
				 playerbits, opponentbits);

	  if (val != ILLEGAL_VALUE)
	    {
	      moves[number_of_moves++].setXYV(x, y, val);

	      if (val > maxval)
		{
		  maxval = val;
		  max_x = x;
		  max_y = y;
		  number_of_maxval = 1;
		}
	      else if (val == maxval) number_of_maxval++;
	    }

	  if (GetInterrupt()) break;
	}

  // long endtime = times(&tmsdummy);

  if (number_of_maxval > 1)
    {
      int r = (m_random.nextInt() % number_of_maxval) + 1;

      int i;

      for (i=0; i < number_of_moves; i++)
	if (moves[i].m_value == maxval && --r <= 0) break;

      max_x = moves[i].m_x;
      max_y = moves[i].m_y;
    }

  if (GetInterrupt()) {       
    return Move(-1, -1, -1);
  } else if (maxval != -LARGEINT) {
    return Move(max_x, max_y, player);
  } else {
    return Move(-1, -1, -1);
  }
}


Move Engine::ComputeFirstMove(Game g) {
  int r;
  int player = g.GetWhoseTurn();

  r = (m_random.nextInt() % 4) + 1;

  if (player == Score::WHITE)
    {
      if (r == 1) return Move(3, 5, player);
      else if (r == 2) return  Move(4, 6, player);
      else if (r == 3) return  Move(5, 3, player);
      else return  Move(6, 4, player);
    }
  else
    {
      if (r == 1) return  Move(3, 4, player);
      else if (r == 2) return  Move(5, 6, player);
      else if (r == 3) return  Move(4, 3, player);
      else return  Move(6, 5, player);
    }
}


int Engine::ComputeMove2(int xplay, int yplay, int player, int level,
			 int cutoffval, ULONG64 playerbits, 
			 ULONG64 opponentbits)
{
  int number_of_turned = 0;
  SquareStackEntry mse;
  int opponent = Score::GetOpponent(player);

  m_nodes_searched++;

  m_board[xplay][yplay] = player;
  playerbits |= m_coord_bit[xplay][yplay];
  m_score.ScoreAdd(player, 1);
  m_bc_score.ScoreAdd(player, m_bc_board[xplay][yplay]);

  ///////////////////
  // Turn all pieces:
  ///////////////////

  for (int xinc=-1; xinc<=1; xinc++)
    for (int yinc=-1; yinc<=1; yinc++)
      if (xinc != 0 || yinc != 0)
	{
	  int x, y;

	  for (x = xplay+xinc, y = yplay+yinc; m_board[x][y] == opponent;
	       x += xinc, y += yinc)
	    ;

	  if (m_board[x][y] == player)
	    for (x -= xinc, y -= yinc; x != xplay || y != yplay;
		 x -= xinc, y -= yinc)
	      {
		m_board[x][y] = player;
		playerbits |= m_coord_bit[x][y];
		opponentbits &= ~m_coord_bit[x][y];
		m_squarestack.Push(x, y);
		m_bc_score.ScoreAdd(player, m_bc_board[x][y]);
		m_bc_score.ScoreSubtract(opponent, m_bc_board[x][y]);
		number_of_turned++;
	      }
	}

  int retval = -LARGEINT;

  if (number_of_turned > 0)
    {
      //////////////
      // Legal move:
      //////////////

      m_score.ScoreAdd(player, number_of_turned);
      m_score.ScoreSubtract(opponent, number_of_turned);

      if (level >= m_depth) retval = EvaluatePosition(player); // Terminal node
      else
	{
	  int maxval = TryAllMoves(opponent, level, cutoffval, opponentbits,
				   playerbits);

	  if (maxval != -LARGEINT) retval = -maxval;
	  else
	    {
	      ///////////////////////////////////////////////////////////////
	      // No possible move for the opponent, it is players turn again:
	      ///////////////////////////////////////////////////////////////
	      retval= TryAllMoves(player, level, -LARGEINT, playerbits,
				  opponentbits);

	      if (retval == -LARGEINT)
		{
		  ///////////////////////////////////////////////
		  // No possible move for anybody => end of game:
		  ///////////////////////////////////////////////

		  int finalscore =
		    m_score.GetScore(player) - m_score.GetScore(opponent);

		  if (m_exhaustive) retval = finalscore;
		  else
		    {
		      // Take a sure win and avoid a sure loss (may not be optimal):

		      if (finalscore > 0) retval = LARGEINT - 65 + finalscore;
		      else if (finalscore < 0) retval = -(LARGEINT - 65 + finalscore);
		      else retval = 0;
		    }
		}
	    }
	}

      m_score.ScoreAdd(opponent, number_of_turned);
      m_score.ScoreSubtract(player, number_of_turned);
    }

  /////////////////
  // Restore board:
  /////////////////

  for (int i = number_of_turned; i > 0; i--)
    {
      mse = m_squarestack.Pop();
      m_bc_score.ScoreAdd(opponent, m_bc_board[mse.m_x][mse.m_y]);
      m_bc_score.ScoreSubtract(player, m_bc_board[mse.m_x][mse.m_y]);
      m_board[mse.m_x][mse.m_y] = opponent;
    }

  m_board[xplay][yplay] = Score::NOBODY;
  m_score.ScoreSubtract(player, 1);
  m_bc_score.ScoreSubtract(player, m_bc_board[xplay][yplay]);

  if (number_of_turned < 1 || GetInterrupt()) return ILLEGAL_VALUE;
  else return retval;
}


int Engine::TryAllMoves(int opponent, int level, int cutoffval,
			ULONG64 opponentbits, ULONG64 playerbits)
{
  int maxval = -LARGEINT;

  // keep GUI alive
  yield();

  ULONG64 null_bits;
  null_bits = 0;

  for (int x=1; x<9; x++)
    {
      for (int y=1; y<9; y++)
	if (m_board[x][y] == Score::NOBODY &&
	    (m_neighbor_bits[x][y] & playerbits) != null_bits)
	  {
	    int val = ComputeMove2(x, y, opponent, level+1, maxval, opponentbits,
				   playerbits);

	    if (val != ILLEGAL_VALUE && val > maxval)
	      {
		maxval = val;
		if (maxval > -cutoffval || GetInterrupt()) break;
	      }
	  }

      if (maxval > -cutoffval || GetInterrupt()) break;
    }

  if (GetInterrupt()) return -LARGEINT;
  return maxval;
}


int Engine::EvaluatePosition(int player)
{
  int retval;

  int opponent = Score::GetOpponent(player);
  int score_player = m_score.GetScore(player);
  int score_opponent = m_score.GetScore(opponent);

  if (m_exhaustive) retval = score_player - score_opponent;
  else
    {
      retval = (100-m_coeff) *
	(m_score.GetScore(player) - m_score.GetScore(opponent)) +
	m_coeff * BC_WEIGHT *
	(m_bc_score.GetScore(player)-m_bc_score.GetScore(opponent));
    }

  return retval;
}


void Engine::SetupBits()
{
  //m_coord_bit = new long[9][9];
  //m_neighbor_bits = new long[9][9];

  ULONG64 bits = 1;

  for (int i=1; i < 9; i++)
    for (int j=1; j < 9; j++)
      {
	m_coord_bit[i][j] = bits;
#if !defined(__GNUC__)
	bits.shl();
#else
	bits *= 2;
#endif
      }

  for (int i=1; i < 9; i++)
    for (int j=1; j < 9; j++)
      {
	m_neighbor_bits[i][j] = 0;

	for (int xinc=-1; xinc<=1; xinc++)
	  for (int yinc=-1; yinc<=1; yinc++)
	    if (xinc != 0 || yinc != 0)
	      if (i + xinc > 0 && i + xinc < 9 && j + yinc > 0 && j + yinc < 9)
		m_neighbor_bits[i][j] |= m_coord_bit[i + xinc][j + yinc];
      }
}


void Engine::SetupBcBoard()
{
  // JAVA m_bc_board = new int[9][9];

  for (int i=1; i < 9; i++)
    for (int j=1; j < 9; j++)
      {
	if (i == 2 || i == 7) m_bc_board[i][j] = -2; else m_bc_board[i][j] = 0;
	if (j == 2 || j == 7) m_bc_board[i][j] -= 2;
      }

  m_bc_board[1][1] = 20;
  m_bc_board[8][1] = 20;
  m_bc_board[1][8] = 20;
  m_bc_board[8][8] = 20;

  m_bc_board[1][2] = -2;
  m_bc_board[2][1] = -2;
  m_bc_board[1][7] = -2;
  m_bc_board[7][1] = -2;
  m_bc_board[8][2] = -2;
  m_bc_board[2][8] = -2;
  m_bc_board[8][7] = -2;
  m_bc_board[7][8] = -2;
}


int Engine::CalcBcScore(int player)
{
  int sum = 0;

  for (int i=1; i < 9; i++)
    for (int j=1; j < 9; j++)
      if (m_board[i][j] == player) 
	sum += m_bc_board[i][j];

  return sum;
}


ULONG64 Engine::ComputeOccupiedBits(int player)
{
  ULONG64 retval = 0;

  for (int i=1; i < 9; i++)
    for (int j=1; j < 9; j++)
      if (m_board[i][j] == player) retval |= m_coord_bit[i][j];

  return retval;
}

