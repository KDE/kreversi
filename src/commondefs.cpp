/*
    SPDX-FileCopyrightText: 2006 Dmitry Suzdalev <dimsuz@gmail.com>
    SPDX-FileCopyrightText: 2013 Denis Kuplyakov <dener.kup@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "commondefs.h"

#include <KLocalizedString>


static QString chipPrefixString[2] = {QStringLiteral("chip_bw"), QStringLiteral("chip_color")};

QString Utils::chipPrefixToString(ChipsPrefix prefix) {
    return chipPrefixString[prefix];
}

ChipColor Utils::opponentColorFor(ChipColor color)
{
    if (color == NoColor)
        return NoColor;
    else
        return (color == White ? Black : White);
}


QString Utils::colorToString(ChipColor color)
{
    if (Preferences::useColoredChips())
        return (color == Black ? i18n("Blue") : i18n("Red"));
    return (color == Black ? i18n("Black") : i18n("White"));
}

QString Utils::moveToString(KReversiMove move)
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
