/*
    SPDX-FileCopyrightText: 2013 Denis Kuplyakov <dener.kup@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef GAMESTARTINFORMATION_H
#define GAMESTARTINFORMATION_H

#include <QString>

/**
 * Represents information gathered from user by StartGameDialog.
 */
class GameStartInformation
{

public:
    /**
     * Indicates player type
     */
    enum PlayerType {
        /** Human player */
        Human,
        /** AI (or Computer) player */
        AI
    };

    /**
     * PlayerType for each player.
     * Use it like: type[Black]
     */
    PlayerType type[2];
    /**
     * Name for each player.
     * Use it like: name[Black]
     */
    QString name[2];
    /**
     * Skill for each player.
     * Has no sense for Human player
     * Use it like: skill[Black]
     */
    int skill[2];
};

#endif // GAMESTARTINFORMATION_H
