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

#include <qwidget.h>
#include <qpixmap.h>

#include "Move.h"


class KConfig;
class Engine;
class Game;


// The class Board is the visible Reversi Board widget.
//

class Board : public QWidget {
  Q_OBJECT

public:

  enum State { Ready, Thinking, Hint};

  Board(QWidget *parent = 0);
  ~Board();

  void   newGame();
  Color  whoseTurn() const;
  Color  humanColor() const    { return human; }
  Color  computerColor() const { return opponent(human); }

  uint   score(Color) const;
  void   setStrength(uint);
  uint   strength() const;
  bool   interrupted() const;
  
  virtual void adjustSize();

  // starts all: emits some signal, so it can't be called from
  // constructor
  void  start();

  // event stuff
  void  paintEvent(QPaintEvent *);
  void  mousePressEvent(QMouseEvent *);

  State  state() const { return m_status; }
  void   setState(State);
  void   setAnimationSpeed(uint);

  uint   moveNumber() const;

  // Methods for handling images of pieces.
  enum      ChipType { Unloaded, Colored, Grayscale };
  void      loadChips(ChipType);
  ChipType  chipType() const { return chiptype; }
  QPixmap   chipPixmap(Color color, uint size) const;
  QPixmap   chipPixmap(uint i, uint size) const;

  bool  loadGame(KConfig *, bool noupdate = FALSE);
  void  saveGame(KConfig *);

  void    setColor(const QColor &);
  QColor  color() const { return bgColor; }
  void    setPixmap(QPixmap &);

public slots:
  void  undo();
  void  hint();
  void  interrupt();
  void  doContinue();
  void  switchSides();
  void  loadSettings();

signals:
  void  score();
  void  gameWon(Color);
  void  statusChange(Board::State);
  void  sizeChange();
  void  turn(Color);

private:
  void  fieldClicked(int, int);
  void  gameEnded();
  void  computerMakeMove();
  void  illegalMove();

  void  updateBoard(bool force = FALSE);

  uint  zoomedSize() const;
  void  drawPiece(uint row, uint col, Color);
  void  drawOnePiece(uint row, uint col, int i);
  void  animateChanged(Move m);
  void  animateChangedRow(int row, int col, int dy, int dx);
  void  rotateChip(uint row, uint col);
  bool  isField(int row, int col) const;

private:
  Engine  *engine;
  Game    *game;

  State    m_status;		// 
  Color    human;
  bool     nopaint;

  QColor   bgColor;
  QPixmap  bg;

  // the pieces
  ChipType  chiptype;
  QPixmap   allchips;
  uint      anim_speed;
};

#endif

