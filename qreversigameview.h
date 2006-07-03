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

#ifndef __QREVERSIGAMEVIEW__H__
#define __QREVERSIGAMEVIEW__H__


#include <qlistbox.h>

#include "Score.h"
#include "Move.h"
#include "board.h"


class KConfig;


class QLabel;

class QReversiGame;


class StatusWidget : public QWidget
{
  Q_OBJECT

public:
  StatusWidget(const QString &text, QWidget *parent);
  
  void  setText(const QString &string);
  void  setPixmap(const QPixmap &pixmap);
  void  setScore(uint score);
  
private:
  QLabel  *m_textLabel;
  QLabel  *m_pixLabel;
  QLabel  *m_scoreLabel;
};


// The main game view

class QReversiGameView : public QWidget {
  Q_OBJECT

public:

  QReversiGameView(QWidget *parent, QReversiGame *game);
  ~QReversiGameView();

  // Proxy methods for the board view
  void    showHint(Move move)       { m_boardView->showHint(move);            }
  void    quitHint()                { m_boardView->quitHint();                }

  void    setShowLegalMoves(bool show){ m_boardView->setShowLegalMoves(show); }
  void    setShowMarks(bool show)   { m_boardView->setShowMarks(show);        }
  void    setShowLastMove(bool show){ m_boardView->setShowLastMove(show);     }

  void    setAnimationSpeed(uint speed){m_boardView->setAnimationSpeed(speed);}

  // To get the pixmap for the status view
  QPixmap   chipPixmap(Color color, uint size)  const
                               { return m_boardView->chipPixmap(color, size); }

  // Proxy methods for the movelist
  // FIXME: Not all of these need to be externally reachable
  void insertMove(QString moveString) { m_movesView->insertItem(moveString); }
  void removeMove(int moveNum) { 
    m_movesView->removeItem(moveNum); 
    updateStatus();
  }
  void setCurrentMove(int moveNum) { 
    m_movesView->setCurrentItem(moveNum); 
    m_movesView->ensureCurrentVisible();
  }

  // The status widgets.
  void  setHumanColor(Color color);

  // Starts all: emits some signal, so it can't be called from
  // constructor
  void  start() { m_boardView->start(); }

  // Used by the outer KZoomMainWindow class.
  void  adjustSize()   { m_boardView->adjustSize(); }

  void  loadSettings() { m_boardView->loadSettings(); }


public slots:
  void  newGame();
  void  moveMade(uint moveNum, Move &move);

  void  updateView();		// Update the entire view.
  void  updateStatus();		// Update the status widgets (score)
  void  updateBoard(bool force = FALSE); // Update the board.
  void  updateMovelist();	// Update the move list.

signals:
  void  signalSquareClicked(int, int);

private slots:
  // Internal slot to reemit the boards signal.
  void  squareClicked(int, int);

private:
  void  createView(); 

private:

  // Pointer to the game we are displaying
  QReversiGame       *m_game; // Pointer to the game object (not owner).

  Color               m_humanColor;

  // Widgets in the view.
  QReversiBoardView  *m_boardView;
  QListBox           *m_movesView;
  StatusWidget       *m_blackStatus;
  StatusWidget       *m_whiteStatus;
};


#endif

