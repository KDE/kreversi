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


#include "qreversigame.h"


// ================================================================
//                      class QReversiGame


QReversiGame::QReversiGame(QObject *parent)
  : QObject(parent), Game()
{
}


QReversiGame::~QReversiGame()
{
}


void QReversiGame::newGame()
{
  Game::newGame();

  emit sig_newGame();

  // FIXME: To be removed
  emit updateBoard();
  //emit sig_score();
  emit turn(Game::toMove());
}


bool QReversiGame::doMove(Move move)
{
  bool  retval = Game::doMove(move);
  if (!retval)
      return false;

  emit sig_move(m_moveNumber, move);

  // FIXME: to be removed
  emit updateBoard();
  //emit sig_score();
  emit turn(Game::toMove());

  if (!Game::moveIsAtAllPossible())
    emit sig_gameOver();

  return retval;
}


bool QReversiGame::undoMove()
{
  bool  retval = Game::undoMove();

  // Update all views.
  emit sig_update();

  // FIXME: to be removed
  emit updateBoard();
  //emit sig_score();
  emit turn(Game::toMove());

  return retval;
}


#include "qreversigame.moc"

