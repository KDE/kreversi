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


#ifndef KREVERSI_H
#define KREVERSI_H


#include "kzoommainwindow.h"

#include "Game.h"
#include "Engine.h"
#include "board.h"


class QLabel;

class KAction;


// A status widget that shows how many pieces one side has.

class StatusWidget : public QWidget
{
  Q_OBJECT

public:
  StatusWidget(const QString &text, QWidget *parent);
  
  void  setPixmap(const QPixmap &pixmap);
  void  setScore(uint score);
  
private:
  QLabel  *m_pixLabel;
  QLabel  *m_label;
};


class KReversi : public KZoomMainWindow {
  Q_OBJECT

public:

  enum State { Ready, Thinking, Hint};

  KReversi();
  ~KReversi();
  
  bool isPlaying() const;

  // Methods that deal with the game
  Color  toMove() const           { return m_krgame->toMove();       }
  Color  humanColor() const       { return m_humanColor;           }
  Color  computerColor() const    { return opponent(m_humanColor); }
  uint   score(Color color) const { return m_krgame->score(color);   }
  uint   moveNumber() const       { return m_krgame->moveNumber();   }

  // Methods that deal with the engine.
  void   setStrength(uint);
  uint   strength() const         { return m_engine->strength();   }
  void   interrupt()              { m_engine->setInterrupt(TRUE);  }
  bool   interrupted() const      { return (m_krgame->toMove() == computerColor()
					    && m_state == Ready);  }

  // State of the program (Hint, Ready, Thinking, etc).
  void     setState(State);
  State    state() const          { return m_state;                }

private:
  // Initialisation
  void     createStatusBar();
  void     createKActions();

  // View functions.
  QString  getPlayerName();

  virtual void  writeZoomSetting(uint zoom);
  virtual uint  readZoomSetting() const;
  virtual void  writeMenubarVisibleSetting(bool visible);
  virtual bool  menubarVisibleSetting() const;

  virtual void  saveProperties(KConfig *);
  virtual void  readProperties(KConfig *);
  virtual bool  queryExit();

 signals:
  void turn(Color);
  void score();
  void stateChange(State);

private slots:

  // Slots for KActions.
  void  slotNewGame();
  void  slotOpenGame();
  void  slotSave();
  void  slotHint();
  void  slotUndo();
  void  slotSwitchSides();

  // Interrupt and continue the engines thinking (also KActions).
  void  slotInterrupt();
  void  slotContinue();

  // Slots for game IO
  void  slotSquareClicked(int, int);

  // Misc slots.
  void  slotStateChange(State);
  void  configureNotifications();

  // Some dialogs and other misc stuff.
  void  showHighScoreDialog();
  void  slotEditSettings();
  void  loadSettings();

 public slots: 
  // Slots for the view.
  void  showColors();
  void  showScore();
  void  showTurn(Color);
  void  slotGameOver();
  void  showGameOver(Color);

private:

  // Private methods
  void  humanMakeMove(int row, int col);
  void  computerMakeMove();
  void  illegalMove();

  void  saveGame(KConfig *);
  bool  loadGame(KConfig *);


private:
  // Member fields
  KAction       *stopAction;
  KAction       *continueAction;

  // The game itself and game properties
  KReversiGame  *m_krgame;	   // Stores the moves of the game
  Color          m_humanColor;	   // The Color of the human player.
  bool           m_lowestStrength; // Lowest strength during the game.
  bool           m_competitiveGame;// True if the game has been
				   // competitive during all moves so far.

  State          m_state;	// Ready, Thinking, Hint
  Engine        *m_engine;      // The AI that creates the computers moves.

  // Some status values.
  bool           gameOver;
  bool           cheating;

  // Widgets
  KReversiBoardView  *m_boardView;          // The board widget.

  StatusWidget       *m_humanStatus;
  StatusWidget       *m_computerStatus;
};


#endif

