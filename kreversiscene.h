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
 *  It also drives the gameflow, i.e. it tells KReversiGame when to make
 *  the next move.
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
    /**
     *  This function will tell you if the scene is currently performing
     *  some kind of "better-don't-interrupt-me" operation.
     *
     *  For example this call is used in KReversiMainWindow to decide
     *  whether it is feasible to perform undo action (which makes a little sence
     *  during, for example animation of computer move)
     */
    bool isBusy() const;
    /**
     *  @return whether the scene is in demo-mode
     */
    bool isDemoModeToggled() const { return m_demoMode; }
public slots:
    void updateBoard();
    void slotGameMoveFinished();
    void slotAnimationStep();
    /**
     *  This will make scene visually mark the last made move
     */
    void setShowLastMove( bool show );
    /**
     *  This will make scene visually mark squares with possible moves
     */
    void setShowLegalMoves( bool show );
    /**
     *  Shows hint for player
     */
    void slotHint();
    /**
     *  Sets Demo Mode.
     *  In this mode KReversiScene would not wait for user 
     *  clicks to produce his turn. It will let computer
     *  play for user
     *  @see m_demoMode
     */
    void toggleDemoMode(bool toggle);
signals:
    void gameOver();
    /**
     *  emitted when Scene finishes displaying last move
     */
    void moveFinished();
private:
    /**
     *  Draws a background with 8x8 cell matrix.
     *  Reimplemented from QGraphicsScene.
     */
    virtual void drawBackground( QPainter *p, const QRectF& rect );
    virtual void mousePressEvent( QGraphicsSceneMouseEvent* );
    /**
     *  Checks if players can move and if they can then:
     *  if it's time for computer to move or user is locked and can't move,
     *  this function tells m_game to perform computer move.
     *  Else it just sits and wait for user's mouse input (= his turn)
     */
    void beginNextTurn();
    /**
     *  Visually displays last move and possible moves
     *  (if the scene is set up to show them)
     */
    void displayLastAndPossibleMoves();
    /**
     *  If scene is showing hint animation this function will stop it
     */
    void stopHintAnimation();
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
     *  This list will hold a changed chips
     *  after each turn. It is received from the game object.
     *  Used to animate corresponding chips.
     */
    MoveList m_changedChips;
    /**
     *  Animation timer
     */
    QTimer* m_animTimer;
    /**
     *  This is our "hint-chip" - used to show hints
     */
    KReversiChip* m_hintChip;
    /**
     *  This will hold a pointer to chip which has "last-move" mark
     */
    KReversiChip* m_lastMoveChip;
    /**
     *  Holds true if the scene is showing hint to the player
     */
    bool m_showingHint;
    /**
     *  Specifies whether computer should play human moves
     */
    bool m_demoMode;
    /**
     *  If true, then last made turn will be shown to the player
     */
    bool m_showLastMove;
    /**
     *  If true, then all possible moves will be shown to the player
     */
    bool m_showPossibleMoves;
    // FIXME dimsuz: document
    QList<QGraphicsRectItem*> m_possibleMovesItems;
};
#endif
