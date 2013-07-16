#ifndef KREVERSIHUMANPLAYER_H
#define KREVERSIHUMANPLAYER_H

#include "kreversigame.h"
#include "kreversiplayer.h"

class KReversiHumanPlayer : public KReversiPlayer
{
    Q_OBJECT
public:
    explicit KReversiHumanPlayer(ChipColor color);

    /**
     *  Overriden slots from @see KReversiPlayer
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
