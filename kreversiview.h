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

#include <KgDeclarativeView>
#include "kreversigame.h"

#include <KGameRenderer>
#include <KStandardDirs>
#include <KgThemeProvider>
#include <QTimer>

#include <kdebug.h>

class KReversiView : public KgDeclarativeView
{
    Q_OBJECT
public:
    KReversiView(KReversiGame* game, QWidget *parent = 0);
    /**
     *  Sets the game object which this view will visualize/use.
     */
    void setGame(KReversiGame* game);
    /**
     *  Sets the chips pixmap to be one found in chipsPrefix
     */
    void setChipsPrefix(const QString& chipsPrefix);
    /**
     *  Sets whether to show board labels.
     */
    void setShowBoardLabels(bool show);
    /**
     *  @return whether the view is in demo-mode
     */
    bool isDemoModeToggled() const {
        return m_demoMode;
    }
    /**
     *  Sets the animation speed (0 - slow, 1 - normal, 2 - fast)
     */
    void setAnimationSpeed(int speed);
public slots:
    void onPlayerMove(int row, int col);
    /**
     *  Synchronizes graphical board with m_game's board
     */
    void updateBoard();
    /**
     *  This will make view visually mark the last made move
     */
    void setShowLastMove(bool show);
    /**
     *  This will make view visually mark squares with possible moves
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
     */
    void setDemoMode(bool state);
    /**
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
    void moveFinished();
private:
    static const int ANIMATION_SPEED_SLOW = 40 * 12;
    static const int ANIMATION_SPEED_NORMAL = 25 * 12;
    static const int ANIMATION_SPEED_FAST = 15 * 12;
    QObject *m_qml_root;
    KgThemeProvider *m_provider;
    QTimer m_delayTimer;
    KReversiPos m_hint;
    int m_delay;
    /**
     *  The Game object
     */
    KReversiGame *m_game;
    /**
     * The SVG element prefix for the current chip set
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
};
#endif
