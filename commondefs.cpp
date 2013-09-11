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
#include <commondefs.h>

#include <KLocale>

ChipColor Utils::opponentColorFor(ChipColor color)
{
    if (color == NoColor)
        return NoColor;
    else
        return (color == White ? Black : White);
}


QString Utils::colorToString(const ChipColor &color)
{
    if (Preferences::useColoredChips())
        return (color == Black ? i18n("Blue") : i18n("Red"));
    return (color == Black ? i18n("Black") : i18n("White"));
}

QString Utils::moveToString(const KReversiMove& move)
{
    QString moveString = colorToString(move.color);

    const char labelsHor[] = "ABCDEFGH";
    const char labelsVer[] = "12345678";

    moveString += QLatin1Char(' ');
    moveString += QLatin1Char(labelsHor[move.col]);
    moveString += QLatin1Char(labelsVer[move.row]);

    return moveString;
}

int Utils::difficultyLevelToInt()
{

    for (int i = 0; i < Kg::difficulty()->levels().size(); i++)
        if (Kg::difficultyLevel()
                == Kg::difficulty()->levels()[i]->standardLevel())
            return i;

    return -1;
}

const KgDifficultyLevel *Utils::intToDifficultyLevel(int skill)
{
    return Kg::difficulty()->levels()[skill];
}
