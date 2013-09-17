/*******************************************************************
 *
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
