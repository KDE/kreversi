/*
    SPDX-FileCopyrightText: 2013 Denis Kuplyakov <dener.kup@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

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
    explicit KReversiHumanPlayer(ChipColor color, const QString &name);

    /**
     *  Overridden slots from @see KReversiPlayer
     */
    void prepare(KReversiGame *game) override;
    void takeTurn() override;
    void skipTurn() override;
    void gameOver() override;

Q_SIGNALS:

public Q_SLOTS:
    /**
     *  Using it to get information from UI.
     *  @param move Move that player has made.
     */
    void onUICellClick(KReversiPos move);

private:

};

#endif // KREVERSIHUMANPLAYER_H
