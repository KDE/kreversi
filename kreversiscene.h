#ifndef KREVERSI_SCENE_H
#define KREVERSI_SCENE_H

#include <QGraphicsScene>

class KReversiGame;
class QPainter;

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
    Q_OBJECT
public:
    KReversiScene( KReversiGame* game=0 );
    void setGame( KReversiGame* game );
public slots:
    void updateBoard();
private:
    /**
     *  Draws a background with 8x8 cell matrix.
     *  Reimplemented from QGraphicsScene.
     */
    virtual void drawBackground( QPainter *p, const QRectF& rect );
    /**
     *  Returns the center point of cell (row,col)
     */
    QPointF cellCenter( int row, int col ) const;
    KReversiGame *m_game;
};
#endif
