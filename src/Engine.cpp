/*
    SPDX-FileCopyrightText: 1997 Mario Weilguni <mweilguni@sime.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

/*******************************************************************
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
 * look at http://www.luthman.nu/Othello/Othello.html
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
// this method could be found at the world wide web at
// https://cis.temple.edu/~ingargio/cis587/readings/alpha-beta.html
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

#include <QApplication>

// ================================================================
//           Classes SquareStackEntry and SquareStack


// A SquareStack is used to store changes to the squares on the board
// during search.


inline void SquareStackEntry::setXY(int x, int y)
{
    m_x = x;
    m_y = y;
}


SquareStackEntry::SquareStackEntry()
{
    setXY(0, 0);
}


// ----------------------------------------------------------------


SquareStack::SquareStack()
{
    init(0);
}


SquareStack::SquareStack(int size)
{
    init(size);
}


void SquareStack::resize(int size)
{
    m_squarestack.resize(size);
}


// (Re)initialize the stack so that is empty, and at the same time
// resize it to 'size'.
//

void SquareStack::init(int size)
{
    resize(size);

    m_top = 0;
    for (int i = 0; i < size; i++)
        m_squarestack[i].setXY(0, 0);
}



inline SquareStackEntry SquareStack::Pop()
{
    return m_squarestack[--m_top];
}


inline void SquareStack::Push(int x, int y)
{
    m_squarestack[m_top].m_x = x;
    m_squarestack[m_top++].m_y = y;
}


// ================================================================
//                       Class MoveAndValue


// Class MoveAndValue aggregates a move with its value.
//


inline void MoveAndValue::setXYV(int x, int y, int value)
{
    m_x     = x;
    m_y     = y;
    m_value = value;
}


MoveAndValue::MoveAndValue()
{
    setXYV(0, 0, 0);
}


MoveAndValue::MoveAndValue(int x, int y, int value)
{
    setXYV(x, y, value);
}

// ================================================================
//                       class Score

/* This class keeps track of the score for both colors.  Such a score
 * could be either the number of pieces, the score from the evaluation
 * function or anything similar.
 */
class Score
{
public:
    Score() {
        m_score[White] = 0;
        m_score[Black] = 0;
    }

    uint score(ChipColor color) const     {
        return m_score[color];
    }

    void set(ChipColor color, uint score) {
        m_score[color] = score;
    }
    void inc(ChipColor color)             {
        m_score[color]++;
    }
    void dec(ChipColor color)             {
        m_score[color]--;
    }
    void add(ChipColor color, uint s)     {
        m_score[color] += s;
    }
    void sub(ChipColor color, uint s)     {
        m_score[color] -= s;
    }

private:
    uint  m_score[2];
};

// ================================================================
//                        The Engine itself


// Some special values used in the search.
static const int LARGEINT      = 99999;
static const int ILLEGAL_VALUE = 8888888;
static const int BC_WEIGHT     = 3;


Engine::Engine(int st, int sd)/* : SuperEngine(st, sd) */
    : m_strength(st)
    , m_random(sd)
    , m_computingMove(false)
{
    m_score = new Score;
    m_bc_score = new Score;
    SetupBcBoard();
    SetupBits();
}


Engine::Engine(int st) //: SuperEngine(st)
    : m_strength(st)
    , m_random(QRandomGenerator::global()->generate())
    , m_computingMove(false)
{
    m_score = new Score;
    m_bc_score = new Score;
    SetupBcBoard();
    SetupBits();
}


Engine::Engine()// : SuperEngine(1)
    : m_strength(1)
    , m_random(QRandomGenerator::global()->generate())
    , m_computingMove(false)
{
    m_score = new Score;
    m_bc_score = new Score;
    SetupBcBoard();
    SetupBits();
}

Engine::~Engine()
{
    delete m_score;
    delete m_bc_score;
}

// keep GUI alive
void Engine::yield()
{
    qApp->processEvents();
}


// Calculate the best move from the current position, and return it.

KReversiMove Engine::computeMove(const KReversiGame& game, bool competitive)
{
    if (m_computingMove)
        return KReversiMove();

    m_computingMove = true;

    ChipColor color;

    // A competitive game is one where we try our damnedest to make the
    // best move.  The opposite is a casual game where the engine might
    // make "a mistake".  The idea behind this is not to scare away
    // newbies.  The member m_competitive is used during search for this
    // very move.
    m_competitive = competitive;

    // Suppose that we should give a heuristic evaluation.  If we are
    // close to the end of the game we can make an exhaustive search,
    // but that case is determined further down.
    m_exhaustive = false;

    // Get the color to calculate the move for.
    color = game.currentPlayer();
    if (color == NoColor) {
        m_computingMove = false;
        return KReversiMove();
    }

    // Figure out the current score
    m_score->set(White, game.playerScore(White));
    m_score->set(Black, game.playerScore(Black));

    // Treat the first move as a special case (we can basically just
    // pick a move at random).
    if (m_score->score(White) + m_score->score(Black) == 4) {
        m_computingMove = false;
        return ComputeFirstMove(game);
    }

    // Let there be room for 3000 changes during the recursive search.
    // This is more than will ever be needed.
    m_squarestack.init(3000);

    // Get the search depth.  If we are close to the end of the game,
    // the number of possible moves goes down, so we can search deeper
    // without using more time.
    m_depth = m_strength;
    if (m_score->score(White) + m_score->score(Black) + m_depth + 3 >= 64)
        m_depth = 64 - m_score->score(White) - m_score->score(Black);
    else if (m_score->score(White) + m_score->score(Black) + m_depth + 4 >= 64)
        m_depth += 2;
    else if (m_score->score(White) + m_score->score(Black) + m_depth + 5 >= 64)
        m_depth++;

    // If we are very close to the end, we can even make the search
    // exhaustive.
    if (m_score->score(White) + m_score->score(Black) + m_depth >= 64)
        m_exhaustive = true;

    // The evaluation is a linear combination of the score (number of
    // pieces) and the sum of the scores for the squares (given by
    // m_bc_score).  The earlier in the game, the more we use the square
    // values and the later in the game the more we use the number of
    // pieces.
    m_coeff = 100 - (100 *
                     (m_score->score(White) + m_score->score(Black)
                      + m_depth - 4)) / 60;

    // Initialize the board that we use for the search.
    for (uint x = 0; x < 10; x++)
        for (uint y = 0; y < 10; y++) {
            if (1 <= x && x <= 8
                    && 1 <= y && y <= 8)
                m_board[x][y] = game.chipColorAt(KReversiPos(y - 1, x - 1));
            else
                m_board[x][y] = NoColor;
        }

    // Initialize a lot of stuff that we will use in the search.

    // Initialize m_bc_score to the current bc score.  This is kept
    // up-to-date incrementally so that way we won't have to calculate
    // it from scratch for each evaluation.
    m_bc_score->set(White, CalcBcScore(White));
    m_bc_score->set(Black, CalcBcScore(Black));

    quint64 colorbits    = ComputeOccupiedBits(color);
    quint64 opponentbits = ComputeOccupiedBits(Utils::opponentColorFor(color));

    int maxval = -LARGEINT;
    int max_x = 0;
    int max_y = 0;

    MoveAndValue moves[60];
    int number_of_moves = 0;
    int number_of_maxval = 0;

    setInterrupt(false);

    quint64 null_bits;
    null_bits = 0;

    // The main search loop.  Step through all possible moves and keep
    // track of the most valuable one.  This move is stored in
    // (max_x, max_y) and the value is stored in maxval.
    m_nodes_searched = 0;
    for (int x = 1; x < 9; x++) {
        for (int y = 1; y < 9; y++) {
            // Don't bother with non-empty squares and squares that aren't
            // neighbors to opponent pieces.
            if (m_board[x][y] != NoColor
                    || (m_neighbor_bits[x][y] & opponentbits) == null_bits)
                continue;


            int val = ComputeMove2(x, y, color, 1, maxval,
                                   colorbits, opponentbits);

            if (val != ILLEGAL_VALUE) {
                moves[number_of_moves++].setXYV(x, y, val);

                // If the move is better than all previous moves, then record
                // this fact...
                if (val > maxval) {

                    // ...except that we want to make the computer miss some
                    // good moves so that beginners can play against the program
                    // and not always lose.  However, we only do this if the
                    // user wants a casual game, which is set in the settings
                    // dialog.
                    int randi = m_random.bounded(7);
                    if (maxval == -LARGEINT
                            || m_competitive
                            || randi < (int) m_strength) {
                        maxval = val;
                        max_x  = x;
                        max_y  = y;

                        number_of_maxval = 1;
                    }
                } else if (val == maxval)
                    number_of_maxval++;
            }

            // Jump out prematurely if interrupt is set.
            if (interrupted())
                break;
        }
    }

    // long endtime = times(&tmsdummy);

    // If there are more than one best move, the pick one randomly.
    if (number_of_maxval > 1) {
        int  r = m_random.bounded(number_of_maxval) + 1;
        int  i;

        for (i = 0; i < number_of_moves; ++i) {
            if (moves[i].m_value == maxval && --r <= 0)
                break;
        }

        max_x = moves[i].m_x;
        max_y = moves[i].m_y;
    }

    m_computingMove = false;
    // Return a suitable move.
    if (interrupted())
        return KReversiMove(NoColor, -1, -1);
    else if (maxval != -LARGEINT)
        return KReversiMove(color, max_y - 1, max_x - 1);
    else
        return KReversiMove(NoColor, -1, -1);
}


// Get the first move.  We can pick any move at random.
//

KReversiMove Engine::ComputeFirstMove(const KReversiGame& game)
{
    int    r;
    ChipColor  color = game.currentPlayer();

    r = m_random.bounded(4) + 1;

    if (color == White) {
        if (r == 1)      return  KReversiMove(color, 4, 2);
        else if (r == 2) return  KReversiMove(color, 5, 3);
        else if (r == 3) return  KReversiMove(color, 2, 4);
        else             return  KReversiMove(color, 3, 5);
    } else {
        if (r == 1)      return  KReversiMove(color, 3, 2);
        else if (r == 2) return  KReversiMove(color, 5, 4);
        else if (r == 3) return  KReversiMove(color, 2, 3);
        else             return  KReversiMove(color, 4, 5);
    }
}


// Play a move at (xplay, yplay) and generate a value for it.  If we
// are at the maximum search depth, we get the value by calling
// EvaluatePosition(), otherwise we get it by performing an alphabeta
// search.
//

int Engine::ComputeMove2(int xplay, int yplay, ChipColor color, int level,
                         int cutoffval, quint64 colorbits,
                         quint64 opponentbits)
{
    int               number_of_turned = 0;
    SquareStackEntry  mse;
    ChipColor             opponent = Utils::opponentColorFor(color);

    m_nodes_searched++;

    // Put the piece on the board and incrementally update scores and bitmaps.
    m_board[xplay][yplay] = color;
    colorbits |= m_coord_bit[xplay][yplay];
    m_score->inc(color);
    m_bc_score->add(color, m_bc_board[xplay][yplay]);

    // Loop through all 8 directions and turn the pieces that can be turned.
    for (int xinc = -1; xinc <= 1; xinc++)
        for (int yinc = -1; yinc <= 1; yinc++) {
            if (xinc == 0 && yinc == 0)
                continue;

            int x, y;

            for (x = xplay + xinc, y = yplay + yinc; m_board[x][y] == opponent;
                    x += xinc, y += yinc)
                ;

            // If we found the end of a turnable row, then go back and turn
            // all pieces on the way back.  Also push the squares with
            // turned pieces on the squarestack so that we can undo the move
            // later.
            if (m_board[x][y] == color)
                for (x -= xinc, y -= yinc; x != xplay || y != yplay;
                        x -= xinc, y -= yinc) {
                    m_board[x][y] = color;
                    colorbits |= m_coord_bit[x][y];
                    opponentbits &= ~m_coord_bit[x][y];

                    m_squarestack.Push(x, y);

                    m_bc_score->add(color, m_bc_board[x][y]);
                    m_bc_score->sub(opponent, m_bc_board[x][y]);
                    number_of_turned++;
                }
        }

    int retval = -LARGEINT;

    // If we managed to turn at least one piece, then (xplay, yplay) was
    // a legal move.  Now find out the value of the move.
    if (number_of_turned > 0) {

        // First adjust the number of pieces for each side.
        m_score->add(color, number_of_turned);
        m_score->sub(opponent, number_of_turned);

        // If we are at the bottom of the search, get the evaluation.
        if (level >= m_depth)
            retval = EvaluatePosition(color); // Terminal node
        else {
            int maxval = TryAllMoves(opponent, level, cutoffval, opponentbits,
                                     colorbits);

            if (maxval != -LARGEINT)
                retval = -maxval;
            else {

                // No possible move for the opponent, it is colors turn again:
                retval = TryAllMoves(color, level, -LARGEINT, colorbits, opponentbits);

                if (retval == -LARGEINT) {

                    // No possible move for anybody => end of game:
                    int finalscore = m_score->score(color) - m_score->score(opponent);

                    if (m_exhaustive)
                        retval = finalscore;
                    else {
                        // Take a sure win and avoid a sure loss (may not be optimal):

                        if (finalscore > 0)
                            retval = LARGEINT - 65 + finalscore;
                        else if (finalscore < 0)
                            retval = -(LARGEINT - 65 + finalscore);
                        else
                            retval = 0;
                    }
                }
            }
        }

        m_score->add(opponent, number_of_turned);
        m_score->sub(color, number_of_turned);
    }

    // Undo the move.  Start by unturning the turned pieces.
    for (int i = number_of_turned; i > 0; i--) {
        mse = m_squarestack.Pop();
        m_bc_score->add(opponent, m_bc_board[mse.m_x][mse.m_y]);
        m_bc_score->sub(color, m_bc_board[mse.m_x][mse.m_y]);
        m_board[mse.m_x][mse.m_y] = opponent;
    }

    // Now remove the new piece that we put down.
    m_board[xplay][yplay] = NoColor;
    m_score->sub(color, 1);
    m_bc_score->sub(color, m_bc_board[xplay][yplay]);

    // Return a suitable value.
    if (number_of_turned < 1 || interrupted())
        return ILLEGAL_VALUE;
    else
        return retval;
}


// Generate all legal moves from the current position, and do a search
// to see the value of them.  This function returns the value of the
// most valuable move, but not the move itself.
//

int Engine::TryAllMoves(ChipColor opponent, int level, int cutoffval,
                        quint64 opponentbits, quint64 colorbits)
{
    int maxval = -LARGEINT;

    // Keep GUI alive by calling the event loop.
    yield();

    quint64  null_bits;
    null_bits = 0;

    for (int x = 1; x < 9; x++) {
        for (int y = 1; y < 9; y++) {
            if (m_board[x][y] == NoColor
                    && (m_neighbor_bits[x][y] & colorbits) != null_bits) {
                int val = ComputeMove2(x, y, opponent, level + 1, maxval, opponentbits,
                                       colorbits);

                if (val != ILLEGAL_VALUE && val > maxval) {
                    maxval = val;
                    if (maxval > -cutoffval || interrupted())
                        break;
                }
            }
        }

        if (maxval > -cutoffval || interrupted())
            break;
    }

    if (interrupted())
        return -LARGEINT;

    return maxval;
}


// Calculate a heuristic value for the current position.  If we are at
// the end of the game, do this by counting the pieces.  Otherwise do
// it by combining the score using the number of pieces, and the score
// using the board control values.
//

int Engine::EvaluatePosition(ChipColor color)
{
    int retval;

    ChipColor opponent = Utils::opponentColorFor(color);

    int    score_color    = m_score->score(color);
    int    score_opponent = m_score->score(opponent);

    if (m_exhaustive)
        retval = score_color - score_opponent;
    else {
        retval = (100 - m_coeff) *
                 (m_score->score(color) - m_score->score(opponent))
                 + m_coeff * BC_WEIGHT * (m_bc_score->score(color)
                                          - m_bc_score->score(opponent));
    }

    return retval;
}


// Calculate bitmaps for each square, and also for neighbors of each
// square.
//

void Engine::SetupBits()
{
    //m_coord_bit = new long[9][9];
    //m_neighbor_bits = new long[9][9];

    quint64 bits = 1;

    // Store a 64 bit unsigned it with the corresponding bit set for
    // each square.
    for (int i = 1; i < 9; i++)
        for (int j = 1; j < 9; j++) {
            m_coord_bit[i][j] = bits;
            bits *= 2;
        }

    // Store a bitmap consisting of all neighbors for each square.
    for (int i = 1; i < 9; i++)
        for (int j = 1; j < 9; j++) {
            m_neighbor_bits[i][j] = 0;

            for (int xinc = -1; xinc <= 1; xinc++)
                for (int yinc = -1; yinc <= 1; yinc++) {
                    if (xinc != 0 || yinc != 0)
                        if (i + xinc > 0 && i + xinc < 9 && j + yinc > 0 && j + yinc < 9)
                            m_neighbor_bits[i][j] |= m_coord_bit[i + xinc][j + yinc];
                }
        }
}


// Set up the board control values that will be used in evaluation of
// the position.
//

void Engine::SetupBcBoard()
{
    // JAVA m_bc_board = new int[9][9];

    for (int i = 1; i < 9; i++)
        for (int j = 1; j < 9; j++) {
            if (i == 2 || i == 7)
                m_bc_board[i][j] = -1;
            else
                m_bc_board[i][j] = 0;

            if (j == 2 || j == 7)
                m_bc_board[i][j] -= 1;
        }

    m_bc_board[1][1] = 2;
    m_bc_board[8][1] = 2;
    m_bc_board[1][8] = 2;
    m_bc_board[8][8] = 2;

    m_bc_board[1][2] = -1;
    m_bc_board[2][1] = -1;
    m_bc_board[1][7] = -1;
    m_bc_board[7][1] = -1;
    m_bc_board[8][2] = -1;
    m_bc_board[2][8] = -1;
    m_bc_board[8][7] = -1;
    m_bc_board[7][8] = -1;
}


// Calculate the board control score.
//

int Engine::CalcBcScore(ChipColor color)
{
    int sum = 0;

    for (int i = 1; i < 9; i++)
        for (int j = 1; j < 9; j++)
            if (m_board[i][j] == color)
                sum += m_bc_board[i][j];

    return sum;
}


// Calculate a bitmap of the occupied squares for a certain color.
//

quint64 Engine::ComputeOccupiedBits(ChipColor color)
{
    quint64 retval = 0;

    for (int i = 1; i < 9; i++)
        for (int j = 1; j < 9; j++)
            if (m_board[i][j] == color) retval |= m_coord_bit[i][j];

    return retval;
}

