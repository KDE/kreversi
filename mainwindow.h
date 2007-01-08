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
#ifndef KREVERSI_MAIN_WINDOW_H
#define KREVERSI_MAIN_WINDOW_H

#include <kmainwindow.h>

class KReversiScene;
class KReversiGame;
class KReversiView;
class QAction;
class KSelectAction;
class KToggleAction;
class QListWidget;
class QLabel;

class KReversiMainWindow : public KMainWindow
{
    Q_OBJECT
public:
    KReversiMainWindow(QWidget* parent=0);
public slots:
    void slotNewGame();
    void slotSkillChanged(int);
    void slotAnimSpeedChanged(int);
    void slotUndo();
    void slotMoveFinished();
    void slotGameOver();
    void slotToggleDemoMode();
    void slotUseColoredChips(bool);
    void slotShowMovesHistory(bool);
    void slotComputerCantMove();
    void slotHighscores();
private:
    void setupActions();
    void loadSettings();

    KReversiScene *m_scene;
    KReversiView  *m_view;
    KReversiGame  *m_game;
    QLabel        *m_historyLabel;
    QListWidget   *m_historyView;

    QAction* m_undoAct;
    QAction* m_hintAct;
    QAction* m_demoAct;
    KSelectAction* m_animSpeedAct;
    KSelectAction* m_skillAct;
    KToggleAction* m_coloredChipsAct;
};
#endif
