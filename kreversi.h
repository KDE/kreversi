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

class Board;
class KAction;
class KToggleAction;

class KReversi : public KMainWindow {
  Q_OBJECT

public:
  KReversi( QWidget* parent = 0, const char *name = 0 );

private:
  void createKActions();
  void createStatusBar();
  QString getPlayerName();

  virtual void saveProperties(KConfig *);
  virtual void readProperties(KConfig *);

private slots:
  void processEvent(int itemid);
  void slotScore();
  void slotGameEnded(int);
  void slotTurn(int);
  void slotStatusChange(int);
  void slotIllegalMove();

  void newGame();
  void openGame();
  void save();
  void configureKeyBindings();
  void zoomIn();
  void zoomOut();
  void sizeChanged();
  
  void showHighScoreDialog();
  void showSettings();

private:
  KAction *undoAction, *zoomInAction, *zoomOutAction;
  KAction *stopAction, *continueAction;
  KToggleAction *soundAction;
  
  Board *board;
  bool gameOver;

};

#endif

