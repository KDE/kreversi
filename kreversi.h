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

#include "board.h"

class QLabel;


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


class KAction;


class KReversi : public KZoomMainWindow {
  Q_OBJECT

public:

  enum State { Ready, Thinking, Hint};

  KReversi();
  ~KReversi();
  
  bool isPlaying() const;

  // Methods that deal with the game
  Color  toMove() const           { return m_game->toMove();       }
  Color  humanColor() const       { return m_humanColor;           }
  Color  computerColor() const    { return opponent(m_humanColor); }
  uint   score(Color color) const { return m_game->score(color);   }
  uint   moveNumber() const       { return m_game->moveNumber();   }

  // Methods that deal with the engine.
  void   setStrength(uint);
  uint   strength() const         { return m_engine->strength();   }
  void   interrupt()              { m_engine->setInterrupt(TRUE);  }
  bool   interrupted() const      { return (m_game->toMove() == opponent(m_humanColor)
					    && m_state == Ready);  }

  // State of the program (Hint, Ready, Thinking, etc).
  State  state() const { return m_state; }
  void   setState(State);

private:
  void     createStatusBar();
  void     createKActions();
  QString  getPlayerName();
  void     updateColors();

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
  void slotSquareClicked(int, int);


  void  configureNotifications();

  // Some dialogs and other misc stuff.
  void  showHighScoreDialog();
  void  slotEditSettings();
  void  loadSettings();

  // Slots for the view.
  void  slotScore();
  void  slotGameEnded(Color);
  void  slotTurn(Color);
  void  slotStateChange(State);

private:

  // Private methods
  void  humanMakeMove(int row, int col);
  void  computerMakeMove();
  void  illegalMove();

  void  saveGame(KConfig *);
  bool  loadGame(KConfig *, bool noupdate = FALSE);


private:
  // Member fields
  KAction  *stopAction;
  KAction  *continueAction;

  State          m_state;	// Ready, Thinking, Hint
  Engine        *m_engine;      // The AI that creates the computers moves.

  Game          *m_game;	// Stores the moves of the game
  Color          m_humanColor;	// The Color of the human player.
  bool           m_lowestStrength; // Lowest strength during the game.
  bool           m_competitiveGame;// True if the game has been
				// competitive during all moves so far.

  Board         *m_board;       // The board widget.


  // Some status values.
  bool           gameOver;
  bool           cheating;

  // Score widgets
  StatusWidget  *m_humanStatus;
  StatusWidget  *m_computerStatus;
};

#endif

