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
 * created 2005 by Inge Wallin <inge@lysator.liu.se>
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
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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
#if 0
  void  loadSettings();

  bool  loadGame(KConfig *, bool noupdate = FALSE);
  void  saveGame(KConfig *);
#endif

 signals:
  void  sig_newGame();
  void  sig_move(uint, Move&);	// A move has just been done.
  void  sig_update();		// Some other change than a move has been done
                                // Example: loadFile(), undoMove();
  void  sig_gameOver();		// The game is over.

  // FIXME: To be removed:
  //void  updateBoard();
  //void  sig_score();
  //void  turn(Color);

private:
  // No members.
};


#endif

