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


class Board : public QWidget {
  Q_OBJECT
public:

  enum { READY = 1, THINKING = 2, HINT = 3};

  Board(QWidget *parent = 0);
  ~Board();

  void newGame();
  Player humanPlayer() const { return human; }
  Player computerPlayer() const { return opponent(human); }
  void getScore(int&, int&) const;
  void setStrength(int);
  int  strength() const;
  bool interrupted() const;

  // starts all: emits some signal, so it can't be called from
  // constructor
  void start();

  // event stuff
  void paintEvent(QPaintEvent *);
  void mousePressEvent(QMouseEvent *);
  QSize sizeHint() const;

  /// stuff for zooming
  bool canZoomIn() const;
  bool canZoomOut() const;
  void zoomIn();
  void zoomOut();
  void setZoom(int);
  int  zoom() const { return _zoom; }

  int  state() const { return _status; }
  void setState(int);
  void setAnimationSpeed(int);
  int  animationSpeed() const { return anim_speed; }

  int  moveNumber() const;

  enum ChipType { Unloaded, Colored, Grayscale };
  void loadChips(ChipType);
  QPixmap chipPixmap(Player player) const;
  ChipType chipType() const { return chiptype; }

  void loadGame(KConfig *, bool noupdate = FALSE);
  void saveGame(KConfig *);
  bool canLoad(KConfig *);

  void setColor(const QColor &);
  QColor color() const { return bgColor; }
  void setPixmap(QPixmap &);

  enum SoundType { ClickSound = 0, DrawSound, WonSound, LostSound,
                   HallOfFameSound, IllegalMoveSound, Nb_Sounds };
  static const char * const SOUND[Nb_Sounds];
  void playSound(SoundType);

public slots:
  void undo();
  void hint();
  void interrupt();
  void doContinue();
  void switchSides();
  void loadSettings();

signals:
  void score();
  void gameWon(Player);
  void statusChange(int);
  void illegalMove();
  void strengthChanged(int);
  void sizeChange();
  void turn(Player);

private:
  void fieldClicked(int, int);
  void gameEnded();
  void computerMakeMove();

  void updateBoard(bool force = FALSE);

  void drawPiece(int row, int col, Player);
  void drawOnePiece(int row, int col, int i);
  void loadPixmaps();
  void adjustSize();
  void animateChanged(Move m);
  void animateChangedRow(int row, int col, int dy, int dx);
  void rotateChip(int row, int col);
  bool isField(int row, int col) const;

private:
  Engine *engine;
  Game *game;

  int  _status;
  int oldsizehint;
  int _zoom;
  int _zoomed_size;
  Player human;
  bool nopaint;

  QColor bgColor;
  QPixmap bg;

  // the chips
  ChipType chiptype;
  QPixmap allchips;
  QPixmap scaled_allchips;

  int anim_speed;
};

#endif

