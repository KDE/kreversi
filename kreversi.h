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

#include "kzoommainwindow.h"

#include "board.h"

class QLabel;

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

class KAction;

class KReversi : public KZoomMainWindow {
  Q_OBJECT

public:
  KReversi();
  
  bool isPlaying() const;

private:
  void createKActions();
  void createStatusBar();
  QString getPlayerName();
  void updateColors();
  virtual void writeZoomSetting(uint zoom);
  virtual uint readZoomSetting() const;
  virtual void writeMenubarVisibleSetting(bool visible);
  virtual bool menubarVisibleSetting() const;

  virtual void saveProperties(KConfig *);
  virtual void readProperties(KConfig *);
  virtual bool queryExit();

private slots:
  void slotScore();
  void slotGameEnded(Color);
  void slotTurn(Color);
  void slotStatusChange(Board::State);
  void switchSides();

  void newGame();
  void openGame();
  void save();
  void configureNotifications();

  void showHighScoreDialog();
  void showSettings();
  void loadSettings();

private:
  KAction *undoAction, *stopAction, *continueAction;
  StatusWidget *_humanStatus, *_computerStatus;

  Board *board;
  bool gameOver, cheating;
};

#endif

