/*
    SPDX-FileCopyrightText: 2006 Dmitry Suzdalev <dimsuz@gmail.com>
    SPDX-FileCopyrightText: 2013 Denis Kuplyakov <dener.kup@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KREVERSI_MAIN_WINDOW_H
#define KREVERSI_MAIN_WINDOW_H


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
    explicit KReversiMainWindow(QWidget* parent = nullptr,  bool startDemo = false);
    ~KReversiMainWindow();
public Q_SLOTS:
    void slotNewGame();
    void levelChanged();
    void slotAnimSpeedChanged(int);
    void slotUndo();
    void slotMoveFinished();
    void slotGameOver();
    void slotUseColoredChips(bool);
    void slotToggleBoardLabels(bool);
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
    void receivedGameStartInformation(const GameStartInformation &info);
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

    enum { common = 1, black, white };
    QLabel *m_statusBarLabel[4];
};
#endif
