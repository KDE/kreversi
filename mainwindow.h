/*******************************************************************
 *
 * Copyright 2006 Dmitry Suzdalev <dimsuz@gmail.com>
 * Copyright 2013 Denis Kuplyakov <dener.kup@gmail.com>
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
#ifndef KREVERSI_MAIN_WINDOW_H
#define KREVERSI_MAIN_WINDOW_H


#include <QApplication>
#include <QDockWidget>
#include <QListWidget>

#include <KSelectAction>
#include <KToggleAction>
#include <KXmlGuiWindow>

#include "preferences.h"
#include "startgamedialog.h"

#include "kreversigame.h"
#include "kreversiview.h"

#include <QLabel>

class KReversiGame;
class KReversiView;
class QAction;

class KReversiMainWindow : public KXmlGuiWindow
{
    Q_OBJECT
public:
    explicit KReversiMainWindow(QWidget* parent = 0,  bool startDemo = false);
    ~KReversiMainWindow();
public slots:
    void slotNewGame();
    void levelChanged();
    void slotAnimSpeedChanged(int);
    void slotUndo();
    void slotMoveFinished();
    void slotGameOver();
    void slotUseColoredChips(bool);
    void slotHighscores();
    void slotDialogReady();
private:
    void showEvent(QShowEvent*) override;
    void setupActionsInit();
    void setupActionsStart();
    void setupActionsGame();
    void loadSettings();
    void updateStatusBar();
    void updateHistory();
    void startDemo();
    void clearPlayers();
    void receivedGameStartInformation(GameStartInformation info);
    KReversiPlayer *m_player[2];

    StartGameDialog *m_startDialog;
    GameStartInformation m_nowPlayingInfo;


    KReversiView  *m_view;
    KReversiGame  *m_game;
    QDockWidget   *m_historyDock;
    QListWidget   *m_historyView;

    bool m_firstShow;
    bool m_startInDemoMode;

    KgThemeProvider *m_provider;

    QAction *m_undoAct;
    QAction *m_hintAct;
    KToggleAction *m_showLast;
    KToggleAction *m_showLegal;
    QAction *m_showMovesAct;
    KSelectAction *m_animSpeedAct;
    KToggleAction *m_coloredChipsAct;

    QPointer<QLabel> black = new QLabel;
    QPointer<QLabel> white = new QLabel;
    QPointer<QLabel> common = new QLabel;
};
#endif
