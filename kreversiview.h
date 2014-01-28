/*
    Copyright 2006 Dmitry Suzdalev <dimsuz@gmail.com>
    Copyright 2010 Brian Croom <brian.s.croom@gmail.com>
    Copyright 2013 Denis Kuplyakov <dener.kup@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KREVERSI_VIEW_H
#define KREVERSI_VIEW_H

#include <KLocalizedString>
#include <KgDeclarativeView>
#include <KStandardDirs>
#include <KgThemeProvider>
#include <QTimer>

#include "colorscheme.h"
#include "kreversigame.h"

/**
 *  This class provides graphical representation of KReversiGame
 *  using QML for graphics display.
 *  It displays the reversi board in its current state,
 *  receives a mouse events, translates them so KReversiGame can understand,
 *  sends them to it,
 *  receives board-changed notifications, nicely animates them.
 *  It also drives the gameflow, i.e. it tells KReversiGame when to make
 *  the next move.
 */
class KReversiView : public KgDeclarativeView
{
    Q_OBJECT
public:
    explicit KReversiView(KReversiGame* game, QWidget *parent = 0);

    /**
     *  Sets the game object which this view will visualize/use
     *
     *  @param game pointer to game object for visualization. View takes
     *              ownership over game object and will delete it
     */
    void setGame(KReversiGame* game);

    /**
     *  Sets the chips pixmap to be one found in chipsPrefix
     *
     *  @param chipsPrefix Use @c chip_color for colored chips
     *                    and @c chip_bw for black-white chips
     */
    void setChipsPrefix(const QString& chipsPrefix);

    /**
     *  Sets whether to show board labels.
     *
     *  @param show @c true to show labels
     *              @c false to hide labels
     */
    void setShowBoardLabels(bool show);

    /**
     *  Checks whether the view is in demo-mode
     *
     *  @return whether the view is in demo-mode
     */
    bool isDemoModeToggled() const {
        return m_demoMode;
    }

    /**
     *  Sets the animation speed
     *
     *  @param speed 0 - slow, 1 - normal, 2 - fast
     */
    void setAnimationSpeed(int speed);

    /**
     *  Destructor used to delete game object owned by class
     */
    ~KReversiView();
public slots:
    /**
     *  Triggered on user click on board, connected to QML signal
     *
     *  @param row index of the clicked cell row (starting from zero)
     *  @param col index of the clicked cell column (starting from zero)
     */
    void onPlayerMove(int row, int col);

    /**
     *  Synchronizes graphical board with m_game's board
     */
    void updateBoard();

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

    /**
     *  Sets Demo Mode.
     *  In this mode KReversiScene would not wait for user
     *  clicks to produce his turn. It will let computer
     *  play for user
     *  @see m_demoMode
     *
     *  @param state @c true to enable demo mode
     *               @c false to disable demo mode
     */
    void setDemoMode(bool state);

    /**
     *  Checks whether view is in demo mode
     *
     *  @return whether game is in demo mode
     */
    bool isInDemoMode() const {
        return m_demoMode;
    }
private slots:
    void slotGameMoveFinished();
    void slotGameOver();
    void slotComputerCantMove();
    void slotPlayerCantMove();
    void slotOnDelay();
signals:
    /**
     * The signal used to notify about end of the move
     */
    void moveFinished();
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
     *  Used to handle end of animation
     */
    QTimer m_delayTimer;

    /**
     *  Position of calculated hint. It is not valid if there is no hint
     */
    KReversiPos m_hint;

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
    QString m_chipsPrefix;

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
