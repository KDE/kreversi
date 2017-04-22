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

#ifndef KREVERSIHUMANPLAYER_H
#define KREVERSIHUMANPLAYER_H

#include "kreversiplayer.h"

/**
 * Represents human player. Implements KReversiPlayer.
 *
 * @see KReversiPlayer
 */
class KReversiHumanPlayer : public KReversiPlayer
{
    Q_OBJECT
public:
    /**
     * Constructs Human player with specified @p color and @p name
     */
    explicit KReversiHumanPlayer(ChipColor color, QString name);

    /**
     *  Overridden slots from @see KReversiPlayer
     */
    void prepare(KReversiGame *game);
    void takeTurn();
    void skipTurn();
    void gameOver();

signals:

public slots:
    /**
     *  Using it to get information from UI.
     *  @param move Move that player has made.
     */
    void onUICellClick(KReversiPos move);

private:

};

#endif // KREVERSIHUMANPLAYER_H
