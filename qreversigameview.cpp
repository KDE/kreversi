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
#include "prefs.h"
#include "Engine.h"
#endif

#include "qreversigame.h"
#include "qreversigameview.h"


// ================================================================
//                     class StatusWidget


StatusWidget::StatusWidget(const QString &text, QWidget *parent)
  : QWidget(parent, "status_widget")
{
  QHBoxLayout  *hbox  = new QHBoxLayout(this, 0, KDialog::spacingHint());
  QLabel       *label;

  label = new QLabel(text, this);
  hbox->addWidget(label);

  m_pixLabel = new QLabel(this);
  hbox->addWidget(m_pixLabel);

  label = new QLabel(":", this);
  hbox->addWidget(label);

  m_label = new QLabel(this);
  hbox->addWidget(m_label);
}


// Set the text label - used to write the number of pieces.
//

void StatusWidget::setScore(uint s)
{
  m_label->setText(QString::number(s));
}


// Set the pixel label - used to show the color.
//

void StatusWidget::setPixmap(const QPixmap &pixmap)
{
  m_pixLabel->setPixmap(pixmap);
}


// ================================================================
//                class KReversiBoardView


QReversiGameView::QReversiGameView(QWidget *parent, QReversiGame *game)
  : QWidget(parent, "gameview")
{
  QGridLayout *layout;

  // Store a pointer to the game.
  m_qrgame = game;

  // The widget stuff
  layout      = new QGridLayout(this, 4, 2);
  m_boardView = new QReversiBoardView(this, game);
  layout->addMultiCellWidget(m_boardView, 0, 3, 0, 0);

  // The status widgets
  m_humanStatus = new StatusWidget(i18n("You"), this);
  layout->addWidget(m_humanStatus, 0, 1);
  m_computerStatus = new StatusWidget(QString::null, this);
  layout->addWidget(m_computerStatus, 1, 1);

  // The "Moves" label
  QLabel  *movesLabel = new QLabel( "Moves", this);
  movesLabel->setAlignment(AlignCenter);
  layout->addWidget(movesLabel, 2, 1);

  // The list of moves.
  m_movesView = new QListBox(this, "moves");
  m_movesView->setMinimumWidth(150);
  layout->addWidget(m_movesView, 3, 1);

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

