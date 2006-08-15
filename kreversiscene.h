#ifndef KREVERSI_SCENE_H
#define KREVERSI_SCENE_H

#include <QGraphicsScene>
#include <QPixmap>

#include "commondefs.h"

class KReversiGame;
class KReversiChipFrameSet;
class QPainter;
class QTimer;

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
    /**
     * @param chipsPixmap the pixmap with animation frames
     */
    KReversiScene( KReversiGame* game, const QPixmap& chipsPixmap );
    void setGame( KReversiGame* game );
    void setBackgroundPixmap( const QPixmap& pix );
public slots:
    void updateBoard();
    void slotMoveFinished();
    void slotAnimationStep();
private:
    /**
     *  Draws a background with 8x8 cell matrix.
     *  Reimplemented from QGraphicsScene.
     */
    virtual void drawBackground( QPainter *p, const QRectF& rect );
    virtual void mousePressEvent( QGraphicsSceneMouseEvent* );
    /**
     *  Returns the center point of cell (row,col)
     */
    QPointF cellCenter( int row, int col ) const;
    /**
     *  Returns the top-left point of cell (row,col)
     */
    QPointF cellTopLeft( int row, int col ) const;
    /**
     *  The Game object
     */
    KReversiGame *m_game;
    /**
     *  Bounding rect of the board grid
     */
    QRectF m_boardRect;
    /**
     *  Background pixmap
     */
    QPixmap m_bkgndPix;
    /**
     *  Animation frameset for chips
     */
    KReversiChipFrameSet *m_frameSet;
    /**
     *  Animation timer
     */
    QTimer* m_animTimer;
    /**
     *  This list will hold a changed chips
     *  after each turn. It is received from the game object.
     *  Used to animate corresponding chips.
     */
    QList<KReversiMove> m_changedChips;
};
#endif
