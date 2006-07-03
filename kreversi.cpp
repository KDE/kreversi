/* Yo Emacs, this is -*- C++ -*-
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


#include <unistd.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qlistbox.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kaction.h>
#include <kstdgameaction.h>
#include <kkeydialog.h>
#include <kconfigdialog.h>
#include <knotifyclient.h>
#include <knotifydialog.h>
#include <kexthighscore.h>

#include "Score.h"
#include "kreversi.h"

// Automatically generated headers
#include "prefs.h"
#include "settings.h"

#include "kreversi.moc"


// ================================================================
//                         class KReversi


#ifndef PICDATA
#define PICDATA(x)  \
    KGlobal::dirs()->findResource("appdata", QString("pics/") + x)
#endif


KReversi::KReversi()
  : KZoomMainWindow(10, 300, 5, "kreversi"), 
    m_gameOver(false)
{
  QWidget     *w;
  QGridLayout *top;

  KNotifyClient::startDaemon();

  // The game.
  m_game     = new QReversiGame();
  m_cheating   = false;
  m_gameOver   = false;
  m_humanColor = Black;

  // The Engine
  m_engine = new Engine();
  setStrength(1);
  
  // The visual stuff
  w = new QWidget(this);
  setCentralWidget(w);

  top = new QGridLayout(w, 2, 2);

  // The reversi game view.
  m_gameView = new QReversiGameView(w, m_game);
  top->addMultiCellWidget(m_gameView, 0, 1, 0, 0);

  // Populate the GUI.
  createKActions();
  addWidget(m_gameView);

  // Connect the signals from the game with slots of the view
  //
  // The only part of the view that is left in this class is the
  // indicator of whose turn it is in the status bar.  The rest is
  // in the game view.
  connect(m_game, SIGNAL(sig_newGame()),  this, SLOT(showTurn()));
  connect(m_game, SIGNAL(sig_move(uint, Move&)), 
	  this,   SLOT(handleMove(uint, Move&))); // Calls showTurn().
  connect(m_game, SIGNAL(sig_update()),   this, SLOT(showTurn()));
  connect(m_game, SIGNAL(sig_gameOver()), this, SLOT(slotGameOver()));

  // Signal that is sent when the user clicks on the board.
  connect(m_gameView, SIGNAL(signalSquareClicked(int, int)),
	  this,       SLOT(slotSquareClicked(int, int)));

  loadSettings();

  setupGUI();
  init("popup");
  m_gameView->start();

  slotNewGame();
}


KReversi::~KReversi()
{
  delete m_game;
  delete m_engine;
}



// Create all KActions used in KReversi.
//

void KReversi::createKActions()
{
  // Standard Game Actions.
  KStdGameAction::gameNew(this, SLOT(slotNewGame()),  actionCollection(), 
			  "game_new");
  KStdGameAction::load(this,    SLOT(slotOpenGame()), actionCollection());
  KStdGameAction::save(this,    SLOT(slotSave()),     actionCollection());
  KStdGameAction::quit(this,    SLOT(close()),        actionCollection());
  KStdGameAction::hint(this,    SLOT(slotHint()),     actionCollection(),
		       "game_hint");
  KStdGameAction::undo(this,    SLOT(slotUndo()),     actionCollection(),
		       "game_undo");

  // Non-standard Game Actions: Stop, Continue, Switch sides
  stopAction = new KAction(i18n("&Stop Thinking"), "game_stop", Qt::Key_Escape,
			   this, SLOT(slotInterrupt()), actionCollection(),
			   "game_stop");
  continueAction = new KAction(i18n("&Continue Thinking"), "reload", 0,
			       this, SLOT(slotContinue()), actionCollection(),
			       "game_continue");
  new KAction(i18n("S&witch Sides"), 0, 0,
	      this, SLOT(slotSwitchSides()), actionCollection(),
	      "game_switch_sides");

  // Some more standard game actions: Highscores, Settings.
  KStdGameAction::highscores(this, SLOT(showHighScoreDialog()), actionCollection());
  KStdAction::preferences(this, SLOT(slotEditSettings()), actionCollection());

  // Actions for the view(s).
  showLastMoveAction = new KToggleAction(i18n("Show Last Move"), "lastmoves", 0, 
					 this, SLOT(slotShowLastMove()),
					 actionCollection(),
					 "show_last_move");
  showLegalMovesAction = new KToggleAction(i18n("Show Legal Moves"), "legalmoves", 0, 
					   this, SLOT(slotShowLegalMoves()),
					   actionCollection(),
					   "show_legal_moves");
}


// ----------------------------------------------------------------
//                    Methods for the engine


// Set the strength for the engine.  We keep track of the lowest
// strength that was used during a game and if the user wins, this is
// the strength that we will store in the highscore file.

void KReversi::setStrength(uint strength)
{
  // FIXME: 7 should be MAXSTRENGTH or something similar.
  Q_ASSERT( 1 <= strength && strength <= 7 );

  strength = QMAX(QMIN(strength, 7), 1);
  m_engine->setStrength(strength);
  if (m_lowestStrength < strength)
    m_lowestStrength = strength;
  KExtHighscore::setGameType(m_lowestStrength-1);
}


// ----------------------------------------------------------------
//                        Slots for KActions


// A slot that is called when the user wants a new game.
//

void KReversi::slotNewGame()
{
  // If already playing, ask the player if he wants to abort the old game.
  if ( isPlaying() ) {
    if (KMessageBox
	::warningYesNo(0,
		       i18n("You are already running an unfinished game.  "
			    "If you abort the old game to start a new one, "
			    "the old game will be registered as a loss in "
			    "the highscore file.\n"
			    "What do you want to do?"),
		       i18n("Abort Current Game?"),
		       i18n("Abort Old Game"),
		       i18n("Continue Old Game")) == KMessageBox::No)
      return;

    KExtHighscore::submitScore(KExtHighscore::Lost, this);
  }

  m_gameOver = false;
  m_cheating = false;

  m_game->newGame();
  m_competitiveGame = Prefs::competitiveGameChoice();
  m_lowestStrength  = strength();
  //kdDebug() << "Competitive: " << m_competitiveGame << endl;

  // Set the state to waiting for the humans move.
  setState(Ready);

  // Black always makes first move.
  if (m_humanColor == White)
    computerMakeMove();
}


// Open an earlier saved game from the config file.
//
// FIXME: Should give a choice to load from an ordinary file (SGF?)
//

void KReversi::slotOpenGame()
{
  KConfig *config = kapp->config();
  config->setGroup("Savegame");

  if (loadGame(config))
    Prefs::setSkill(m_engine->strength());
  m_gameView->setHumanColor(humanColor());
}


// Save a game to the config file.
//
// FIXME: Should give a choice to save as an ordinary file (SGF?)
//

void KReversi::slotSave()
{
  KConfig *config = kapp->config();
  config->setGroup("Savegame");
  saveGame(config);

  KMessageBox::information(this, i18n("Game saved."));
}


void KReversi::slotHint()
{
  Move  move;

  if (state() != Ready) 
    return;

  setState(Thinking);
  move = m_engine->computeMove(m_game, m_competitiveGame);

  setState(Hint);
  m_gameView->showHint(move);

  setState(Ready);
}


// Takes back last set of moves
//

void KReversi::slotUndo()
{
  if (state() != Ready)
    return;

  // Can't undo anything if no moves are made.
  if (m_game->moveNumber() == 0)
    return;

  // Undo all moves of the same color as the last one.
  Color  last_color = m_game->lastMove().color();
  while (m_game->moveNumber() != 0
	 && last_color == m_game->lastMove().color()) {
    m_game->undoMove();
    m_gameView->removeMove(m_game->moveNumber());
  }

  // Take back one more move.
  if (m_game->moveNumber() > 0) {
    m_game->undoMove();
    m_gameView->removeMove(m_game->moveNumber());

    // FIXME: Call some method in m_gameView.
    m_gameView->setCurrentMove(m_game->moveNumber() - 1);
  }

  if (m_game->toMove() == computerColor()) {
    // Must repaint so that the new move is not shown before the old
    // one is removed on the screen.
    m_gameView->repaint();
    computerMakeMove();
  }
  else
    m_gameView->update();
}


// Interrupt thinking of game engine.
//

void KReversi::slotInterrupt()
{
  m_engine->setInterrupt(TRUE);

  // Indicate that the computer was interrupted.
  showTurn();
}


// Continues a move if it was interrupted earlier.
//

void KReversi::slotContinue()
{
  if (interrupted())
    computerMakeMove();
}


// Turn on or off showing of legal moves in the board view.

void KReversi::slotShowLastMove() 
{
  m_gameView->setShowLastMove(showLastMoveAction->isChecked());
}


// Turn on or off showing of legal moves in the board view.

void KReversi::slotShowLegalMoves() 
{
  m_gameView->setShowLegalMoves(showLegalMovesAction->isChecked());
}


void KReversi::slotSwitchSides()
{
  if (state() != Ready) 
    return;

  if (interrupted()) {
    KMessageBox::information(this, i18n("You cannot switch sides in the middle of the computer's move."),
			     i18n("Notice"));
    return;
  }

  // It's ok to change sides before the first move.
  if (m_game->moveNumber() != 0) {
    int res = KMessageBox::warningContinueCancel(this,
						 i18n("If you switch side, your score will not be added to the highscores."),
						 QString::null, QString::null, "switch_side_warning");
    if ( res==KMessageBox::Cancel ) 
      return;

    m_cheating = true;
  }

  m_humanColor = opponent(m_humanColor);

  // Update the human color in the window.
  m_gameView->setHumanColor(m_humanColor);

  kapp->processEvents();
  computerMakeMove();
}


// ----------------------------------------------------------------
//                   Slots for the game IO


// Handle mouse clicks.
//

void KReversi::slotSquareClicked(int row, int col)
{
  // Can't move when it is the computers turn.
  if ( interrupted() ) {
    illegalMove();
    return;
  }

  if (state() == Ready)
    humanMakeMove(row, col);
  else if (state() == Hint) {
    m_gameView->quitHint();
    setState(Ready);
  }
  else
    illegalMove();
}


// ----------------------------------------------------------------
//                   Slots for the game view


// Show the move in the move view.
// FIXME: Move this to the gameview.

void KReversi::handleMove(uint /*moveno*/, Move& /*move*/)
{
  showTurn();
}


// A slot that is called when it is time to show whose turn it is.
//

void KReversi::showTurn()
{
  showTurn(m_game->toMove());
}

void KReversi::showTurn(Color color)
{
  // If we are not playing, do nothing. 
  if (m_gameOver)
    return;

  if (color == humanColor())
    statusBar()->message(i18n("Your turn"));
  else if (color == computerColor()) {
    QString  message = i18n("Computer's turn");

    // We can't use the interrupted() test here since we might be in a
    // middle state when called from slotInterrupt().
    if (m_state == Thinking)
      message += i18n(" (interrupted)");
    statusBar()->message(message);
  }
  else
    statusBar()->clear();
}


// A slot that is called when the game ends.
//

void KReversi::slotGameOver()
{
  uint  black = m_game->score(Black);
  uint  white = m_game->score(White);

  setState(Ready);

  if (black > white)
    showGameOver(Black);
  else if (black < white)
    showGameOver(White);
  else
    showGameOver(Nobody);

  showTurn(Nobody);
}


// ----------------------------------------------------------------
//                        Private methods


// Handle the humans move.
//

void KReversi::humanMakeMove(int row, int col)
{
  if (state() != Ready) 
    return;

  Color color = m_game->toMove();

  // Create a move from the mouse click and see if it is legal.
  // If it is, then make a human move.
  Move  move(color, col + 1, row + 1);
  if (m_game->moveIsLegal(move)) {
    // Do the move. The view is automatically updated.
    m_game->doMove(move);

    if (!m_game->moveIsAtAllPossible()) {
      setState(Ready);
      slotGameOver();
      return;
    }

    if (color != m_game->toMove())
      computerMakeMove();
  } else
    illegalMove();
}


// Make a computer move.
//

void KReversi::computerMakeMove()
{
  MoveList  moves;

  // Check if the computer can move.
  Color color    = m_game->toMove();
  Color opponent = ::opponent(color);

  if (!m_game->moveIsPossible(color))
    return;
 
  // Make computer moves until the human can play or until the game is over.
  setState(Thinking);
  do {
    Move  move;

    if (!m_game->moveIsAtAllPossible()) {
      setState(Ready);
      slotGameOver();
      return;
    }

    move = m_engine->computeMove(m_game, m_competitiveGame);
    if (move.x() == -1) {
      setState(Ready);
      return;
    }
    usleep(300000); // Pretend we have to think hard.

    // Do the move on the board.  The view is automatically updated.
    //playSound("click.wav");
    m_game->doMove(move);
  } while (!m_game->moveIsPossible(opponent));

  setState(Ready);

  if (!m_game->moveIsAtAllPossible()) {
    slotGameOver();
    return;
  }
}


// Handle an attempt to make an illegal move by the human.

void KReversi::illegalMove()
{
  KNotifyClient::event(winId(), "illegal_move", i18n("Illegal move"));
}


// Show things when the game is over.
//

void KReversi::showGameOver(Color color) 
{
  // If the game already was over, do nothing.
  if (m_gameOver)
    return;

  statusBar()->message(i18n("End of game"));

  // Get the scores.
  uint human    = m_game->score(humanColor());
  uint computer = m_game->score(computerColor());

  KExtHighscore::Score score;
  score.setScore(m_game->score(humanColor()));
  
  // Show the winner in a messagebox.
  if ( color == Nobody ) {
    KNotifyClient::event(winId(), "draw", i18n("Draw!"));
    QString s = i18n("Game is drawn!\n\nYou     : %1\nComputer: %2")
                .arg(human).arg(computer);
    KMessageBox::information(this, s, i18n("Game Ended"));
    score.setType(KExtHighscore::Draw);
  }
  else if ( humanColor() == color ) {
    KNotifyClient::event(winId(), "won", i18n("Game won!"));
    QString s = i18n("Congratulations, you have won!\n\nYou     : %1\nComputer: %2")
                .arg(human).arg(computer);
    KMessageBox::information(this, s, i18n("Game Ended"));
    score.setType(KExtHighscore::Won);
  } 
  else {
    KNotifyClient::event(winId(), "lost", i18n("Game lost!"));
    QString s = i18n("You have lost the game!\n\nYou     : %1\nComputer: %2")
                .arg(human).arg(computer);
    KMessageBox::information(this, s, i18n("Game Ended"));
    score.setType(KExtHighscore::Lost);
  }
  
  // Store the result in the highscore file if no cheating was done,
  // and only if the game was competitive.
  if (!m_cheating && m_competitiveGame) {
    KExtHighscore::submitScore(score, this);
  }

  m_gameOver = true;
}


// Saves the game in the config file.  
//
// Only one game at a time can be saved.
//

void KReversi::saveGame(KConfig *config)
{
  // Stop thinking.
  slotInterrupt(); 

  // Write the data to the config file.
  config->writeEntry("State",         state());
  config->writeEntry("Strength",      strength());
  config->writeEntry("Competitive",   (int) m_competitiveGame);
  config->writeEntry("HumanColor",    (int) m_humanColor);

  // Write the moves of the game to the config object.  This object
  // saves itself all at once so we don't have to write the moves
  // to the file ourselves.
  config->writeEntry("NumberOfMoves", m_game->moveNumber());
  for (uint i = 0; i < m_game->moveNumber(); i++) {
    Move  move = m_game->move(i);

    QString  moveString;
    QString  idx;

    moveString.sprintf("%d %d %d", move.x(), move.y(), (int) move.color());
    idx.sprintf("Move_%d", i + 1);
    config->writeEntry(idx, moveString);
  }

  // Actually write the data to file.
  config->sync();

  // Continue with the move if applicable.
  slotContinue(); 
}


// Loads the game.  Only one game at a time can be saved.

bool KReversi::loadGame(KConfig *config)
{
  slotInterrupt(); // stop thinking

  uint  nmoves = config->readNumEntry("NumberOfMoves", 0);
  if (nmoves==0) 
    return false;

  m_game->newGame();
  uint movenumber = 1;
  while (nmoves--) {
    // Read one move.
    QString  idx;
    idx.sprintf("Move_%d", movenumber++);

    QStringList  s = config->readListEntry(idx, ' ');
    uint         x = (*s.at(0)).toUInt();
    uint         y = (*s.at(1)).toUInt();
    Color        color = (Color)(*s.at(2)).toInt();

    Move  move(color, x, y);
    m_game->doMove(move);
  }

  m_humanColor      = (Color) config->readNumEntry("HumanColor");
  m_competitiveGame = (bool)  config->readNumEntry("Competitive");

  m_gameView->updateBoard(TRUE);
  setState(State(config->readNumEntry("State")));
  setStrength(config->readNumEntry("Strength", 1));

  if (interrupted())
    slotContinue();
  else {
    // Computer makes first move.
    if (m_humanColor != m_game->toMove())
      computerMakeMove();
  }

  return true;
}


// ----------------------------------------------------------------


void KReversi::saveProperties(KConfig *c)
{
  saveGame(c);
}


void KReversi::readProperties(KConfig *config) {
  loadGame(config);
  m_gameOver = false;
  m_cheating = false;		// FIXME: Is this true?  It isn't saved.
}


void KReversi::showHighScoreDialog()
{
  KExtHighscore::show(this);
}


void KReversi::slotEditSettings()
{
  // If we are already editing the settings, then do nothing.
  if (KConfigDialog::showDialog("settings"))
    return;

  KConfigDialog *dialog  = new KConfigDialog(this, "settings", Prefs::self(), 
					     KDialogBase::Swallow);
  Settings      *general = new Settings(0, "General");

  dialog->addPage(general, i18n("General"), "package_settings");
  connect(dialog, SIGNAL(settingsChanged()), this, SLOT(loadSettings()));
  dialog->show();
}


void KReversi::configureNotifications()
{
    KNotifyDialog::configure(this);
}


void KReversi::loadSettings()
{
  m_humanColor = (Color) Prefs::humanColor();
  setStrength(Prefs::skill());

  // m_competitiveGame is set at the start of a game and can only be
  // downgraded during the game, never upgraded.
  if ( !Prefs::competitiveGameChoice() )
    m_competitiveGame = false;

  m_gameView->loadSettings();

  // Update the color of the human and the computer.
  m_gameView->setHumanColor(humanColor());
}


bool KReversi::isPlaying() const
{
  return ( m_game->moveNumber() != 0 && !m_gameOver );
}


void KReversi::setState(State newState)
{
  m_state = newState;

  if (m_state == Thinking){
    kapp->setOverrideCursor(waitCursor);
    stopAction->setEnabled(true);
  }
  else {
    kapp->restoreOverrideCursor();
    stopAction->setEnabled(false);
  }

  continueAction->setEnabled(interrupted());
}


bool KReversi::queryExit()
{
  if ( isPlaying() )
    KExtHighscore::submitScore(KExtHighscore::Lost, this);

  return KZoomMainWindow::queryExit();
}


void KReversi::writeZoomSetting(uint zoom)
{
  Prefs::setZoom(zoom);
  Prefs::writeConfig();
}


uint KReversi::readZoomSetting() const
{
  return Prefs::zoom();
}


void KReversi::writeMenubarVisibleSetting(bool visible)
{
  Prefs::setMenubarVisible(visible);
  Prefs::writeConfig();
}


bool KReversi::menubarVisibleSetting() const
{
  return Prefs::menubarVisible();
}
