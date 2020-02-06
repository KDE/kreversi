/*******************************************************************
 *
 * Copyright 2006 Dmitry Suzdalev <dimsuz@gmail.com>
 * Copyright 2013 Denis Kuplyakov <dener.kup@gmail.com>
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
#ifndef COMMONDEFS_H
#define COMMONDEFS_H

#include <QString>
#include <KgDifficulty>

#include "preferences.h"

/**
 * Used to indicate chip's state.
 */
enum ChipColor {
    /** White state */
    White = 0,
    /** Black state */
    Black = 1,
    /** No chip (empty cell) */
    NoColor = 2
};

static const int nRows = 8;
static const int nCols = 8;

/**
 * Represents position on board.
 */
struct KReversiPos {
    KReversiPos(int r = -1, int c = -1)
        : row(r), col(c) { }

    int row;
    int col;

    bool isValid() const {
        return (row >= 0 && col >= 0 && row < nRows && col < nCols);
    }
};

/**
 * Represents move of player.
 * It is KReversiPos + ChipColor
 */
struct KReversiMove: public KReversiPos {
    KReversiMove(ChipColor col = NoColor, int r = -1, int c = -1)
        : KReversiPos(r, c), color(col) { }

    KReversiMove(ChipColor col, const KReversiPos &pos)
        : KReversiPos(pos), color(col) { }

    ChipColor color;

    bool isValid() const {
        return (color != NoColor
                && row >= 0 && col >= 0
                && row < nRows && col < nCols);
    }
};

typedef QList<KReversiMove> MoveList;

/**
 * Indicates current color setting of user
 */
enum ChipsPrefix {
    /** Show Black and White chips */
    BlackWhite = 0,
    /** Show Red and Blue chips */
    Colored = 1
};

namespace Utils
{
/**
 * Gives appropriate prefix-string by @p prefix
 * @return @c "chip_bw" for @c BlackWhite
 *         @c "chip_color" for @c Colored
 */
QString chipPrefixToString(ChipsPrefix prefix);
/**
 * Return opposite color for @p color
 * @return @c Black for @c White
 *         @c White for @c Black
 *         @c NoColor for @c NoColor
 */
ChipColor opponentColorFor(ChipColor color);
/**
 * @return Human-readable string representing @p color
 */
QString colorToString(const ChipColor &color);
/**
 * @return Human-readable string representing @p move
 */
QString moveToString(const KReversiMove& move);
/**
 * @return Index of current difficulty level in increasing order
 */
int difficultyLevelToInt();
/**
 * @return Difficulty level that in @p skill place in increasing order among
 *         all difficulty levels
 */
const KgDifficultyLevel *intToDifficultyLevel(int skill);
}


#endif
