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

  void    showLegalMoves(MoveList moves)
                                    { m_boardView->showLegalMoves(moves);     }
  void    quitShowLegalMoves()      { m_boardView->quitShowLegalMoves();      }
  void    setMarks(bool show)       { m_boardView->setMarks(show);            }
  bool    marks()        const      { return m_boardView->marks();            }
  void    setShowLastMove(bool show){ m_boardView->setShowLastMove(show);     }
  bool    showLastMove() const      { return m_boardView->showLastMove();     }

  // FIXME: These should not be called from the outside (used signals instead).
  void    updateBoard(bool force = FALSE) { m_boardView->updateBoard(force);  }
  void    animateChanged(Move move)       { m_boardView->animateChanged(move);}

  void    setAnimationSpeed(uint speed){m_boardView->setAnimationSpeed(speed);}

  // To get the pixmap for the status view
  QPixmap   chipPixmap(Color color, uint size)  const
                               { return m_boardView->chipPixmap(color, size); }

  // Proxy methods for the movelist
  // FIXME: Not all of these need to be externally reachable
  //        example: ensureCurrentVisible().
  void insertMove(QString moveString) { m_movesView->insertItem(moveString); }
  void removeMove(int moveNum) { m_movesView->removeItem(moveNum); }
  void setCurrentMove(int moveNum) { m_movesView->setCurrentItem(moveNum); }
  void ensureCurrentMoveVisible()  { m_movesView->ensureCurrentVisible(); }

  // The status widgets.
  void  setHumanColor(Color color);
  void  setStatusPixmap(Color color, QPixmap pixmap);

  void  setStatusScore(int index, int score)
    {
      if (index == 0) 
	m_blackStatus->setScore(score);
      else
	m_whiteStatus->setScore(score);
    }


  // starts all: emits some signal, so it can't be called from
  // constructor
  void  start() { m_boardView->start(); }

  // Used by the outer KZoomMainWindow class.
  void  adjustSize() { m_boardView->adjustSize(); }

  void  loadSettings() { m_boardView->loadSettings(); }


public slots:
  void  slotNewGame();
  void  moveMade(uint moveNum, Move &move);

  void  updateView();		// Update the entire view.
  void  updateStatus();		// Update the status widgets (score)


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

