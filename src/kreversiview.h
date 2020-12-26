/*
    SPDX-FileCopyrightText: 2006 Dmitry Suzdalev <dimsuz@gmail.com>
    SPDX-FileCopyrightText: 2010 Brian Croom <brian.s.croom@gmail.com>
    SPDX-FileCopyrightText: 2013 Denis Kuplyakov <dener.kup@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KREVERSI_VIEW_H
#define KREVERSI_VIEW_H


#include <KgDeclarativeView>
#include <KgThemeProvider>

#include "commondefs.h"
#include "kreversigame.h"

/**
 *  This class provides graphical representation of KReversiGame
 *  using QML for graphics display.
 *  It displays the reversi board in its current state,
 *  receives a mouse events, translates them with signals,
 *  receives board-changed notifications, nicely animates them.
 */
class KReversiView : public KgDeclarativeView
{
    Q_OBJECT
public:
    explicit KReversiView(KReversiGame* game, QWidget *parent, KgThemeProvider *provider);
    /**
     *  Destructor used to delete game object owned by class
     */
    ~KReversiView();

    /**
     *  Sets the game object which this view will visualize/use
     *
     *  @param game pointer to game object for visualization. View takes
     *              ownership over game object and will delete it
     */
    void setGame(KReversiGame* game);

    /**
     *  Sets the chips prefix to @p chipsPrefix
     */
    void setChipsPrefix(ChipsPrefix chipsPrefix);

    /**
     *  Sets whether to show board labels.
     *
     *  @param show @c true to show labels
     *              @c false to hide labels
     */
    void setShowBoardLabels(bool show);

    /**
     *  Sets the animation speed
     *
     *  @param speed 0 - slow, 1 - normal, 2 - fast
     *
     *  @return time for animation in milliseconds to pass it to KReversiGame
     */
    void setAnimationSpeed(int speed);

public Q_SLOTS:
    /**
    *   This will make view visually mark the last made move
    *
    *   @param show @c true to show last move
    *               @c false to don't show last move
     */
    void setShowLastMove(bool show);

    /**
     *  This will make view visually mark squares with possible moves
     *
     *  @param show @c true to show legal moves
     *              @c false to don't show legal moves
     */
    void setShowLegalMoves(bool show);

    /**
     *  Shows hint for player
     */
    void slotHint();

private Q_SLOTS:
    /**
     *  Triggered on user click on board, connected to QML signal
     *
     *  @param row index of the clicked cell row (starting from 0)
     *  @param col index of the clicked cell column (starting from 0)
     */
    void onPlayerMove(int row, int col);
    /**
     *  Synchronizes graphical board with m_game's board
     */
    void updateBoard();
    void gameMoveFinished();
    void gameOver();
    void whitePlayerCantMove();
    void blackPlayerCantMove();
Q_SIGNALS:
    void userMove(KReversiPos);

private:
    /**
     *  40 ms time per frame for animation
     */
    static const int ANIMATION_SPEED_SLOW = 40 * 12;

    /**
     *  25 ms time per frame for animation
     */
    static const int ANIMATION_SPEED_NORMAL = 25 * 12;

    /**
     *  15 ms time per frame for animation
     */
    static const int ANIMATION_SPEED_FAST = 15 * 12;

    /**
     *  Used to provide access to QML-implemented board
     */
    QObject *m_qml_root;

    /**
     *  Used to access theme engine from QML
     */
    KgThemeProvider *m_provider;

    /**
     *  Position of calculated hint. It is not valid if there is no hint
     */
    KReversiMove m_hint;

    /**
     *  Current animation time
     */
    int m_delay;

    /**
     *  Pointer to game object
     */
    KReversiGame *m_game;

    /**
     *  The SVG element prefix for the current chip set
     */
    ChipsPrefix m_ColouredChips;

    /**
     *  If true, then last made turn will be shown to the player
     */
    bool m_showLastMove;

    /**
     *  If true, then all possible moves will be shown to the player
     */
    bool m_showLegalMoves;

    /**
     *  If true board labels will be rendered
     */
    bool m_showLabels;
    
    /**
     *  Used to handle animation duration due to sequental turning of chips
     */
    int m_maxDelay;
};
#endif
