/*
    SPDX-FileCopyrightText: 2006 Dmitry Suzdalev <dimsuz@gmail.com>
    SPDX-FileCopyrightText: 2013 Denis Kuplyakov <dener.kup@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

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

    KReversiMove(ChipColor col, KReversiPos pos)
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
QString colorToString(ChipColor color);
/**
 * @return Human-readable string representing @p move
 */
QString moveToString(KReversiMove move);
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
