/*******************************************************************
 *
 * Copyright 2013 Inge Wallin <inge@lysator.liu.se>
 * Copyright 2006 Dmitry Suzdalev <dimsuz@gmail.com>
 *
 * This file is part of the KDE project "KReversi"
 *
 * KReversi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * KReversi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KReversi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 ********************************************************************/


// KReversi
#include "reversi.h"


namespace Reversi
{

// ----------------------------------------------------------------
//                             class Position


 struct diffs {
     int dr;
     int dc;
 } directionDiffs[8] = {
    {-1,  0},   // Up
    { 1,  0},   // Down
    { 0,  1},   // Right
    { 0, -1},   // Left
    {-1, -1},   // UpLeft
    {-1,  1},   // UpRight
    { 1, -1},   // DownLeft
    { 1,  1}    // DownRight
};
const int numDirections = sizeof(directionDiffs) / sizeof(struct diffs);


Position::Position()
    : m_toMove(Black)
{
    // Reset the board.
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            m_board[r][c] = NoColor;

    // Set up the initial configuration.
    m_board[3][3] = m_board[4][4] = White;
    m_board[3][4] = m_board[4][3] = Black;

    // the score for the initial configuration
    m_score[Black] = m_score[White] = 2;
}

Position::~Position()
{
}

bool Position::isLegalMove(Color color, int row, int col) const
{
    // first - the trivial case:
    if (m_board[row][col] != NoColor || color == NoColor)
        return false;

    for (int direction = 0; direction < numDirections; ++direction) {
        if (hasChunk(color, row, col, direction))
            return true;
    }

    return false;
}

bool Position::isLegalMove(Move &move) const
{
    return isLegalMove(move.color, move.row, move.col);
}

Color Position::colorAt(int row, int col) const
{
    Q_ASSERT(row < 8 && col < 8);
    return m_board[row][col];
}

int Position::playerScore(Color color) const
{
    Q_ASSERT(color == Black || color == White);
    return m_score[color];
}


bool Position::isGameOver() const
{
    return !(hasLegalMove(Black) || hasLegalMove(White));
}


bool Position::hasLegalMove(Color color) const
{
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if (isLegalMove(color, row, col))
                return true;
        }
    }

    return false;
}

SquareList Position::legalMoves(Color color) const
{
    SquareList moves;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Square sq(row, col);
            if (isLegalMove(color, row, col))
                moves.append(sq);
        }
    }

    return moves;
}

bool Position::makeMove(Move &move)
{
    Color color = move.color;
    int   row = move.row;
    int   col = move.col;
    Color opponent = opponentColorFor(move.color);
    bool  legalMove = false;

    move.m_turnedPieces.clear();
    move.m_wasToMove = m_toMove;

    m_board[row][col] = color;
    m_score[color]++;

    // Now turn color of all chips that were won.
    for (int direction = 0; direction < numDirections; ++direction) {
        if (!hasChunk(color, row, col, direction))
            continue;

        legalMove = true;

        // We already know that there is a chunk in this direction so
        // we don't have to check for edges.
        int r = row + directionDiffs[direction].dr;
        int c = col + directionDiffs[direction].dc;
        while (m_board[r][c] == opponent) {
            m_board[r][c] = color;

            Square sq(r, c);
            move.m_turnedPieces.append(sq);
            
            m_score[color]++;
            m_score[opponent]--;

            r += directionDiffs[direction].dr;
            c += directionDiffs[direction].dc;
        }
    }

    m_toMove = opponent;

    return legalMove;
}

bool Position::undoMove(Move &move, bool checkFirst)
{
    Color color = move.color;
    int   row = move.row;
    int   col = move.col;
    Color opponent = opponentColorFor(move.color);

    if (checkFirst) {
        if (m_board[row][col] != color)
            return false;

        foreach (const Square &sq, move.turnedPieces()) {
            if (m_board[sq.row][sq.col] != color)
                return false;
        }
    }

    m_board[row][col] = NoColor;
    m_score[color]--;

    // Return back the board and score to their previous states.
    foreach (const Square &sq, move.turnedPieces()) {
        if (m_board[sq.row][sq.col] != color)
            return false;
        m_board[sq.row][sq.col] = opponent;
        m_score[color]--;
        m_score[opponent]++;
    }

    // Return back the toMove value to its previous state.
    m_toMove = move.m_wasToMove;

    return true;
}


// ----------------------------------------------------------------
//                 Private functions


bool Position::hasChunk(Color color, int row, int col, int dir) const
{
    // On each step (as we proceed) we must ensure that current chip is of the
    // opponent color.
    // We'll do our steps until we reach the chip of player color or we reach
    // the end of the board in this direction.
    // If we found player-colored chip and number of opponent chips between it and
    // the starting one is greater than zero, then Hurray! we found a chunk

    Color opponent = opponentColorFor(color);
    int r = row + directionDiffs[dir].dr;
    int c = col + directionDiffs[dir].dc;

    // Initial step. Check if the square is on the edge or there is an
    // opponent piece next to the original square.
    if (r < 0 || r > 7 || c < 0 || c > 7)
        return false;
    if (m_board[r][c] != opponent)
        return false;

    do {
        r += directionDiffs[dir].dr;
        c += directionDiffs[dir].dc;

        // At the edge?
        if (r < 0 || r > 7 || c < 0 || c > 7)
            return false;

        // End of an actual chunk?
        if (m_board[r][c] == color)
            return true;
        // no chunk
        if (m_board[r][c] == NoColor)
            return false;
    } while (true);

    // Not reached
    return false;
}


} // namespace Reversi

