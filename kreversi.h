/* Yo Emacs, this -*- C++ -*-
 *******************************************************************
 *******************************************************************
 *
 *
 * KREVERSI
 *
 *
 *******************************************************************
 *
 * A Reversi (or sometimes called Othello) game
 *
 *******************************************************************
 *
 * created 1997 by Mario Weilguni <mweilguni@sime.com>
 *
 *******************************************************************
 *
 * This file is part of the KDE project "KREVERSI"
 *
 * KREVERSI is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * KREVERSI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KREVERSI; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *******************************************************************
 */

#ifndef KREVERSI_H
#define KREVERSI_H

#include <kmainwindow.h>

#include "Score.h"

class QLabel;
class Board;
class KAction;
class KToggleAction;

class StatusWidget : public QWidget
{
  Q_OBJECT
public:
  StatusWidget(const QString &text, QWidget *parent);

  void setPixmap(const QPixmap &pixmap);
  void setScore(uint score);

private:
  QLabel *_pixLabel, *_label;
};

class KReversi : public KMainWindow {
  Q_OBJECT

public:
  KReversi();

private:
  void createKActions();
  void createStatusBar();
  QString getPlayerName();
  bool eventFilter(QObject *, QEvent *e);
  void updateColors();

  virtual void saveProperties(KConfig *);
  virtual void readProperties(KConfig *);

private slots:
  void slotScore();
  void slotGameEnded(Player);
  void slotTurn(Player);
  void slotStatusChange(int);
  void slotIllegalMove();
  void switchSides();

  void newGame();
  void openGame();
  void save();
  void configureKeyBindings();
  void zoomIn();
  void zoomOut();

  void showHighScoreDialog();
  void showSettings();
  void loadSettings();

private:
  KAction *undoAction, *zoomInAction, *zoomOutAction;
  KAction *stopAction, *continueAction;
  KToggleAction *soundAction;
  StatusWidget *_humanStatus, *_computerStatus;

  Board *board;
  bool gameOver;

};

#endif

