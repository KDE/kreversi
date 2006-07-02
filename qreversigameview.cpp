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


#include <qlayout.h>
#include <qwidget.h>
#include <qlabel.h>

#include <klocale.h>
#include <kdialog.h>

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
#include "Engine.h"
#endif
#include "prefs.h"

#include "qreversigame.h"
#include "qreversigameview.h"


// ================================================================
//                     class StatusWidget


StatusWidget::StatusWidget(const QString &text, QWidget *parent)
  : QWidget(parent, "status_widget")
{
  QHBoxLayout  *hbox  = new QHBoxLayout(this, 0, KDialog::spacingHint());
  QLabel       *label;

  m_textLabel = new QLabel(text, this);
  hbox->addWidget(m_textLabel);

  m_pixLabel = new QLabel(this);
  hbox->addWidget(m_pixLabel);

  label = new QLabel(":", this);
  hbox->addWidget(label);

  m_scoreLabel = new QLabel(this);
  hbox->addWidget(m_scoreLabel);
}


// Set the text label
//

void StatusWidget::setText(const QString &string)
{
  m_textLabel->setText(string);
}


// Set the pixel label - used to show the color.
//

void StatusWidget::setPixmap(const QPixmap &pixmap)
{
  m_pixLabel->setPixmap(pixmap);
}


// Set the score label - used to write the number of pieces.
//

void StatusWidget::setScore(uint s)
{
  m_scoreLabel->setText(QString::number(s));
}


// ================================================================
//                  class QReversiGameView


QReversiGameView::QReversiGameView(QWidget *parent, QReversiGame *game)
  : QWidget(parent, "gameview")
{
  // Store a pointer to the game.
  m_game = game;

  // The widget stuff
  createView();

  // Other initializations.
  m_humanColor = Nobody;

  // Connect the game to the view.
  connect(m_game, SIGNAL(sig_newGame()), this, SLOT(newGame()));
  connect(m_game, SIGNAL(sig_move(uint, Move&)),
	  this,   SLOT(moveMade(uint, Move&)));
  connect(m_game, SIGNAL(sig_update()),  this, SLOT(updateView()));
  // The sig_gameOver signal is not used by the view.

  // Reemit the signal from the board.
  connect(m_boardView, SIGNAL(signalSquareClicked(int, int)),
	  this,        SLOT(squareClicked(int, int)));
}


QReversiGameView::~QReversiGameView()
{
}


// Create the entire view.  Only called once from the constructor.

void QReversiGameView::createView()
{
  QGridLayout  *layout = new QGridLayout(this, 4, 2);

  // The board
  m_boardView = new QReversiBoardView(this, m_game);
  m_boardView->loadSettings();	// Load the pixmaps used in the status widgets.
  layout->addMultiCellWidget(m_boardView, 0, 3, 0, 0);

  // The status widgets
  m_blackStatus = new StatusWidget(QString::null, this);
  m_blackStatus->setPixmap(m_boardView->chipPixmap(Black, 20));
  layout->addWidget(m_blackStatus, 0, 1);
  m_whiteStatus = new StatusWidget(QString::null, this);
  m_whiteStatus->setPixmap(m_boardView->chipPixmap(White, 20));
  layout->addWidget(m_whiteStatus, 1, 1);

  // The "Moves" label
  QLabel  *movesLabel = new QLabel( i18n("Moves"), this);
  movesLabel->setAlignment(AlignCenter);
  layout->addWidget(movesLabel, 2, 1);

  // The list of moves.
  m_movesView = new QListBox(this, "moves");
  m_movesView->setMinimumWidth(150);
  layout->addWidget(m_movesView, 3, 1);
}


// ----------------------------------------------------------------
//                              Slots


// Recieves the sig_newGame signal from the game.

void QReversiGameView::newGame()
{
  m_boardView->updateBoard(true);
  m_movesView->clear();
  updateStatus();
}


// Recieves the sig_move signal from the game.

void QReversiGameView::moveMade(uint moveNum, Move &move)
{
  //FIXME: Error checks.
  QString colorsWB[] = {
    i18n("White"),
    i18n("Black")
  };
  QString colorsRB[] = {
    i18n("Red"),
    i18n("Blue")
  };

  // Insert the new move in the listbox and mark it as the current one.
  m_movesView->insertItem(QString("%1. %2 %3")
			  .arg(moveNum)
			  .arg(Prefs::grayscale() ? colorsWB[move.color()]
			       : colorsRB[move.color()])
			  .arg(move.asString()));
  m_movesView->setCurrentItem(moveNum - 1);
  m_movesView->ensureCurrentVisible();

  // Animate all changed pieces.
  m_boardView->animateChanged(move);
  m_boardView->updateBoard();

  // Update the score.
  updateStatus();
}


// Recieves the sig_update signal from the game, and can be called
// whenever a total update of the view is required.

void QReversiGameView::updateView()
{
  m_boardView->updateBoard(true);
  updateMovelist();
  updateStatus();
}


// Only updates the status widgets (score).

void QReversiGameView::updateStatus()
{
  m_blackStatus->setScore(m_game->score(Black));
  m_whiteStatus->setScore(m_game->score(White));
}


// Only updates the status board.

void QReversiGameView::updateBoard(bool force)
{
  m_boardView->updateBoard(force);
}


// Only updates the movelist.  This method regenerates the list from
// scratch.

void QReversiGameView::updateMovelist()
{
  // FIXME: NYI
}


// This special slot is just because the external program doesn't have
// access to the internal board view.
//

void QReversiGameView::squareClicked(int row, int col)
{
  emit signalSquareClicked(row, col);
}


// ----------------------------------------------------------------
//                   Other public methods.


void QReversiGameView::setHumanColor(Color color)
{
  m_humanColor = color;

  if (color == Black) {
    m_blackStatus->setText(i18n("You"));
    m_whiteStatus->setText("");
  }
  else {
    m_blackStatus->setText("");
    m_whiteStatus->setText(i18n("You"));
  }
}


#include "qreversigameview.moc"

