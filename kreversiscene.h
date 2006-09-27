/*******************************************************************
 *
 * Copyright 2006 Dmitry Suzdalev <dimsuz@gmail.com>
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
    KReversiScene( KReversiGame* game, const QString& chipsPath );
    ~KReversiScene();

    void setGame( KReversiGame* game );
    void setBackgroundPixmap( const QPixmap& pix );
    void setChipsPixmap( const QString& chipsPath );
    void resizeScene( int width, int height );
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
    /**
     *  Sets the animation speed (0 - slow, 1 - normal, 2 - fast)
     */
    void setAnimationSpeed(int speed);
public slots:
    void updateBoard();
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
private slots:
    void slotGameMoveFinished();
    void slotAnimationStep();
    void slotGameOver();
signals:
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
     *  Current size of chip
     */
    int m_curChipSize;
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
     *  Delay between animation frames
     */
    int m_timerDelay;
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