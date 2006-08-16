#ifndef KREVERSI_SCENE_H
#define KREVERSI_SCENE_H

#include <QGraphicsScene>
#include <QPixmap>

#include "commondefs.h"

class KReversiGame;
class KReversiChipFrameSet;
class KReversiChip;
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
    /**
     *  Shows hint for player
     */
    void slotHint();
private:
    /**
     *  Draws a background with 8x8 cell matrix.
     *  Reimplemented from QGraphicsScene.
     */
    virtual void drawBackground( QPainter *p, const QRectF& rect );
    virtual void mousePressEvent( QGraphicsSceneMouseEvent* );
    /**
     *  Checks if players can move and if they can:
     *  if it's time for computer to move or user is locked and can't move,
     *  this function tells m_game to perform computer move.
     *  Else it just sits and wait for user's mouse input (= his turn)
     */
    void beginNextTurn();
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
     *  Holds true if the scene is showing hint to the player
     */
    bool m_showingHint;
    /**
     *  This is our "hint-chip" - used to show hints
     */
    KReversiChip* m_hintChip;
    /**
     *  This list will hold a changed chips
     *  after each turn. It is received from the game object.
     *  Used to animate corresponding chips.
     */
    QList<KReversiMove> m_changedChips;
};
#endif
