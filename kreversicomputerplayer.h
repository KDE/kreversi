#ifndef KREVERSICOMPUTERPLAYER_H
#define KREVERSICOMPUTERPLAYER_H

#include "kreversigame.h"
#include "kreversiplayer.h"

class KReversiComputerPlayer : public KReversiPlayer
{
    Q_OBJECT
public:
    explicit KReversiComputerPlayer(ChipColor color);
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
