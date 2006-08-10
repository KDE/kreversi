#ifndef KREVERSI_SCENE_H
#define KREVERSI_SCENE_H

#include <QGraphicsScene>

class KReversiGame;

/**
 *  This class provides graphical representation of KReversiGame
 *  using QGraphicsScene for graphics display.
 *  It displays the reversi board in its current state,
 *  receives a mouse events, translates them so KReversiGame can understand,
 *  sends them to it, 
 *  receives board-changed notifications, nicely animates them.
 * FIXME dimsuz: make this description more descriptive :)
 */
class KReversiScene : public QGraphicsScene
{
public:
    KReversiScene( KReversiGame* game=0 );
    void setGame( KReversiGame* game );
private:
    KReversiGame *m_game;
};
#endif
