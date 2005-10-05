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
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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
}


bool QReversiGame::doMove(Move move)
{
  bool  retval = Game::doMove(move);
  if (!retval)
      return false;

  emit sig_move(m_moveNumber, move);

  if (!Game::moveIsAtAllPossible())
    emit sig_gameOver();

  return retval;
}


bool QReversiGame::undoMove()
{
  bool  retval = Game::undoMove();

  // Update all views.
  emit sig_update();

  return retval;
}


#include "qreversigame.moc"

