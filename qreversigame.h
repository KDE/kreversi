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

#ifndef __QREVERSIGAME__H__
#define __QREVERSIGAME__H__

#include <qwidget.h>
#include <qpixmap.h>

#include "Position.h"
#include "Game.h"
#include "Move.h"


class KConfig;



// The main document class in the reversi program.  The thing that
// makes this a QReversiGame instead of just a ReversiGame is that it
// emits signals that can be used to update a view.
//
// Signals:
//   updateBoard()
//   score()
//   turn(Color)
//   gameOver()
//

class QReversiGame : public QObject, public Game {
 Q_OBJECT

 public:
  QReversiGame(QObject *parent = 0);
  ~QReversiGame();

  // Methods dealing with the game
  void  newGame();
  bool  doMove(Move move);
  bool  undoMove();
  void  loadSettings();

  bool  loadGame(KConfig *, bool noupdate = FALSE);
  void  saveGame(KConfig *);

  const Position &position() const {return m_position; }

 signals:
  void  sig_move(uint, Move&);
  void  updateBoard();
  void  sig_score();
  void  turn(Color);
  void  gameOver();

  void  sizeChange();

private:
  // No members.
};


#endif

