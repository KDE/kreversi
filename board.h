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


class Game;

class KConfig;


// The class Board is the visible Reversi Board widget.
//

class Board : public QWidget {
  Q_OBJECT

public:

  Board(QWidget *parent, Game *game);
  ~Board();

  // Used by the outer KZoomMainWindow class.
  virtual void adjustSize();

  // Helper functions for actions in the main program.
  void    showHint(Move move);
  void    quitHint();

  // starts all: emits some signal, so it can't be called from
  // constructor
  void  start();

  // event stuff
  void  paintEvent(QPaintEvent *);
  void  mousePressEvent(QMouseEvent *);

  void   setAnimationSpeed(uint);

  // Methods for handling images of pieces.
  enum      ChipType { Unloaded, Colored, Grayscale };
  void      loadChips(ChipType);
  ChipType  chipType()                          const { return chiptype; }
  QPixmap   chipPixmap(Color color, uint size)  const;
  QPixmap   chipPixmap(uint i, uint size)       const;

  void    setColor(const QColor &);
  QColor  color()                               const { return bgColor; }
  void    setPixmap(QPixmap &);

  // View methods called from the outside.
  void    updateBoard(bool force = FALSE);
  void    animateChanged(Move move);

  void    gameEnded();

  void    loadSettings();

signals:
  void  signalSquareClicked(int, int);

  void  score();
  void  gameWon(Color);
  void  sizeChange();
  void  turn(Color);

private:
  uint  zoomedSize() const;
  void  drawPiece(uint row, uint col, Color);
  void  drawOnePiece(uint row, uint col, int i);
  void  animateChangedRow(int row, int col, int dy, int dx);
  void  rotateChip(uint row, uint col);
  bool  isField(int row, int col) const;

private:
  Game    *m_game;		// Pointer to the game object (not owner).

  // The background of the board - a color and a pixmap.
  QColor    bgColor;
  QPixmap   bg;

  // the pieces
  ChipType  chiptype;
  QPixmap   allchips;
  uint      anim_speed;

  // Special stuff used only in smaller areas.
  bool      m_hintShowing;
};

#endif

