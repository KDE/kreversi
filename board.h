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

#ifndef __BOARD__H__
#define __BOARD__H__

#include "misc.h"
#include <qwidget.h>
#include <qpixmap.h>
#include "Engine.h"
#include "Score.h"
#include "Move.h"
#include "Position.h"
#include <kconfig.h>

class Board : public QWidget {
  Q_OBJECT
public:

  enum { READY = 1, THINKING = 2, HINT = 3};

  Board(QWidget *parent = 0);
  ~Board();

  void newGame();
  int humanIs();
  int computerIs();
  void getScore(int&, int&);
  void setStrength(int);
  int  getStrength();
  void interrupt();
  bool interrupted();
  void doContinue();
  void undo();
  void hint();
  
  // starts all: emits some signal, so it can't be called from 
  // constructor
  void start();

  // event stuff
  void paintEvent(QPaintEvent *);
  void mousePressEvent(QMouseEvent *);
  void timerEvent(QTimerEvent *);
  QSize sizeHint() const;  

  /// stuff for zooming
  bool canZoomIn();
  bool canZoomOut();
  void zoomIn();
  void zoomOut();
  void setZoom(int);
  int  getZoom() const;

  void switchSides();
  int  getState();
  void setState(int);
  void setAnimationSpeed(int);
  int  animationSpeed();

  int  getMoveNumber();
  void loadChips(const char *);
  QString chipsName();

  void loadGame(KConfig *, bool noupdate = FALSE);
  void saveGame(KConfig *);
  bool canLoad(KConfig *);

public slots:
  void setColor(const QColor &);
  void setPixmap(QPixmap &);

protected slots:
  void slotFieldClicked(int, int);  

signals:
  void signalFieldClicked(int, int);
  void score();
  void sizeChange();
  void gameWon(int);
  void statusChange(int);
  void illegalMove();
  void turn(int);
  void strengthChanged(int);

private:  
  void gameEnded();
  void computerMakeMove();
  
  void updateBoard(bool force = FALSE);
  void drawPiece(int row, int col, bool force = FALSE);
  void drawOnePiece(int row, int col, int color);
  void loadPixmaps();
  void adjustSize();
  void animateChanged(Move m);
  void animateChangedRow(int row, int col, int dy, int dx);
  void rotateChip(int row, int col);
  bool isField(int row, int col);
  void scaleOneChip(int i);

private:
  QString chipname;
  Engine e;
  Game   g;
  int  _status;
  int _size;
  int oldsizehint;
  int _zoom;
  int human;
  int initTimerID, scaleTimerID;
  bool nopaint;
  
  QPixmap bg;

  // the chips
  QPixmap *allchips, *chip[25];

  int anim_speed;
};

#endif
