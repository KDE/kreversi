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
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *******************************************************************
 */


#include <qlayout.h>
#include <qwidget.h>
#include <qlabel.h>

#if 0
#include <unistd.h>

#include <qpainter.h>
#include <qfont.h>

#include <kapplication.h>
#include <kstandarddirs.h>
#include <kconfig.h>
#include <knotifyclient.h>
#include <klocale.h>
#include <kexthighscore.h>
#include <kdebug.h>

#include "board.h"
#include "prefs.h"
#include "Engine.h"
#endif
#include "qreversigame.h"
#include "qreversigameview.h"


// ================================================================
//                class KReversiBoardView


QReversiGameView::QReversiGameView(QWidget *parent, QReversiGame *game)
  : QWidget(parent, "gameview")
{
  QGridLayout *layout;

  // Store a pointer to the game.
  m_qrgame = game;

  // The widget stuff
  layout      = new QGridLayout(this, 2, 2);
  m_boardView = new QReversiBoardView(this, game);
  layout->addMultiCellWidget(m_boardView, 0, 1, 0, 0);

  // The "Moves" label
  QLabel  *movesLabel = new QLabel( "Moves", this);
  movesLabel->setAlignment(AlignCenter);
  layout->addWidget(movesLabel, 0, 1);

  // The list of moves.
  m_movesView = new QListBox(this, "moves");
  m_movesView->setMinimumWidth(150);
  layout->addWidget(m_movesView, 1, 1);

  // FIXME: More view widgets will come here.

  // Reemit the signal from the board..
  connect(m_boardView, SIGNAL(signalSquareClicked(int, int)),
	  this,        SLOT(squareClicked(int, int)));
}


QReversiGameView::~QReversiGameView()
{
}


// ----------------------------------------------------------------


void QReversiGameView::squareClicked(int row, int col)
{
  emit signalSquareClicked(row, col);
}


// ----------------------------------------------------------------



#include "qreversigameview.moc"

