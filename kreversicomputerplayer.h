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

#ifndef KREVERSICOMPUTERPLAYER_H
#define KREVERSICOMPUTERPLAYER_H

#include "kreversigame.h"
#include "kreversiplayer.h"

class KReversiComputerPlayer : public KReversiPlayer
{
    Q_OBJECT
public:
    explicit KReversiComputerPlayer(ChipColor color, QString name);
    ~KReversiComputerPlayer();

    /**
     *  Overriden slots from @see KReversiPlayer
     */
    void prepare(KReversiGame *game);
    void takeTurn();
    void skipTurn();
    void gameOver();

    /**
     *  Sets computer skill
     *  @param skill Skill level from 1 to 7
     */
    void setSkill(int skill);

signals:

public slots:

private:
    Engine *m_engine;
};

#endif // KREVERSICOMPUTERPLAYER_H
