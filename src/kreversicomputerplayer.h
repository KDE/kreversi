/*
    SPDX-FileCopyrightText: 2013 Denis Kuplyakov <dener.kup@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KREVERSICOMPUTERPLAYER_H
#define KREVERSICOMPUTERPLAYER_H

#include "kreversiplayer.h"

/**
 * Represents computer or AI of this game. Implements KReversiPlayer.
 *
 * @see KReversiPlayer
 */
class KReversiComputerPlayer : public KReversiPlayer
{
    Q_OBJECT
public:
    /**
     * Constructs AI player of specified @p color and @p name
     */
    explicit KReversiComputerPlayer(ChipColor color, const QString &name);
    ~KReversiComputerPlayer();

    /**
     *  Overridden slots from @see KReversiPlayer
     */
    void prepare(KReversiGame *game) override;
    void takeTurn() override;
    void skipTurn() override;
    void gameOver() override;

    /**
     *  Sets computer skill
     *  @param skill Skill level from 1 to 7
     */
    void setSkill(int skill);

    /**
     *  @return lowest skill that has been set
     */
    int lowestSkill();

Q_SIGNALS:

public Q_SLOTS:

private:
    int m_lowestSkill;
    Engine *m_engine;
};

#endif // KREVERSICOMPUTERPLAYER_H
