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
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
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

#include "prefs.h"
#include "Score.h"
#include "kreversi.h"
#include "kreversi.moc"
#include "board.h"
#include "settings.h"


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
  m_krgame     = new QReversiGame();
  m_cheating   = false;
  m_gameOver   = false;
  m_humanColor = Black;

  w = new QWidget(this);
  setCentralWidget(w);

  top = new QGridLayout(w, 2, 2);

  // The reversi board view.
  m_boardView = new KReversiBoardView(w, m_krgame);
  top->addMultiCellWidget(m_boardView, 0, 1, 0, 0);

  // The "Moves" label
  QLabel  *movesLabel = new QLabel( "Moves", w);
  movesLabel->setAlignment(AlignCenter);
  top->addWidget(movesLabel, 0, 1);

  // The list of moves.
  m_movesView = new QListBox(w, "moves");
  m_movesView->setMinimumWidth(150);
  top->addWidget(m_movesView, 1, 1);


  // The Engine
  m_engine = new Engine();
  setStrength(1);
  
  // Populate the GUI.
  createStatusBar();
  createKActions();
  addWidget(m_boardView);

  // Connect some signals on the board with slots of the application
  connect(m_krgame, SIGNAL(sig_move(uint, Move&)), 
	  this,     SLOT(showMove(uint, Move&)));
  connect(m_krgame, SIGNAL(sig_score()),    this, SLOT(showScore()));
  connect(m_krgame, SIGNAL(gameOver()),     this, SLOT(slotGameOver()));
  connect(m_krgame, SIGNAL(turn(Color)),    this, SLOT(showTurn(Color)));

  // Internal signal
  connect(this, SIGNAL(stateChange(State)),
          this, SLOT(slotStateChange(State)));

  // Signal that is sent when the user clicks on the board.
  connect(m_boardView, SIGNAL(signalSquareClicked(int, int)),
	  this,        SLOT(slotSquareClicked(int, int)));

  loadSettings();

  setupGUI();
  init("popup");
  m_boardView->start();
  showScore();

  // Show legal moves for black.
  if (showLegalMovesAction->isChecked()) {
    MoveList  moves = m_krgame->position().generateMoves(Black);
    m_boardView->showLegalMoves(moves);
  }
}


KReversi::~KReversi()
{
  delete m_krgame;
  delete m_engine;
}



// Create the status bar at the lower edge of the main window.
//

void KReversi::createStatusBar()
{
  m_humanStatus = new StatusWidget(i18n("You"), this);
  statusBar()->addWidget(m_humanStatus, 0, true);

  m_computerStatus = new StatusWidget(QString::null, this);
  statusBar()->addWidget(m_computerStatus, 0, true);
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
  showLegalMovesAction = new KToggleAction(i18n("Show legal moves"), 0, 
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
		       i18n("Abort current game?"),
		       i18n("Abort the old game"),
		       i18n("Continue the old game")) == KMessageBox::No)
      return;

    KExtHighscore::submitScore(KExtHighscore::Lost, this);
  }

  m_gameOver = false;
  m_cheating = false;

  m_krgame->newGame();
  m_competitiveGame = Prefs::competitiveGameChoice();
  m_lowestStrength  = strength();
  //kdDebug() << "Competitive: " << m_competitiveGame << endl;

  // Show some data
  setState(Ready);
  showTurn(Black);
  m_boardView->updateBoard(TRUE);
  m_movesView->clear();
  showScore();

  if (showLegalMovesAction->isChecked()) {
    MoveList  moves = m_krgame->position().generateMoves(Black);
    m_boardView->showLegalMoves(moves);
  }

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
  showColors();
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
  move = m_engine->computeMove(m_krgame, m_competitiveGame);

  setState(Hint);
  m_boardView->showHint(move);

  setState(Ready);
}


// Takes back last set of moves
//

void KReversi::slotUndo()
{
  if (state() != Ready)
    return;

  // Can't undo anything if no moves are made.
  if (m_krgame->moveNumber() == 0)
    return;

  // Undo all moves of the same color as the last one.
  Color  last_color = m_krgame->lastMove().color();
  while (m_krgame->moveNumber() != 0
	 && last_color == m_krgame->lastMove().color()) {
    m_movesView->removeItem(m_krgame->moveNumber() - 1);
    m_krgame->undoMove();
  }

  // Take back one more move.
  if (m_krgame->moveNumber() > 0) {
    m_movesView->removeItem(m_krgame->moveNumber() - 1);
    m_krgame->undoMove();

    m_movesView->setCurrentItem(m_krgame->moveNumber() - 1);
    m_movesView->ensureCurrentVisible();
  }

  if (m_krgame->toMove() == computerColor()) {
    // Must repaint so that the new move is not shown before the old
    // one is removed on the screen.
    m_boardView->repaint();
    computerMakeMove();
  }
  else
    m_boardView->update();

  // Show legal moves.
  if (showLegalMovesAction->isChecked()) {
    MoveList  moves = m_krgame->position().generateMoves(Black);
    m_boardView->showLegalMoves(moves);
  }
}


// Interrupt thinking of game engine.
//

void KReversi::slotInterrupt()
{
  m_engine->setInterrupt(TRUE);

  // At this point the engine has not returned yet and State has not
  // yet been set to Ready, so the interrupted() test does not work
  // here.
  showTurn(m_krgame->toMove());
}


// Continues a move if it was interrupted earlier.
//

void KReversi::slotContinue()
{
  if (interrupted())
    computerMakeMove();
}


// Turn on or off showing of legal moves in the board view.

void KReversi::slotShowLegalMoves() 
{
  if (showLegalMovesAction->isChecked()) {
    Color     toMove = m_krgame->position().toMove();
    MoveList  moves  = m_krgame->position().generateMoves(toMove);
    m_boardView->showLegalMoves(moves);
  }
  else
    m_boardView->quitShowLegalMoves();
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
  if (m_krgame->moveNumber() != 0) {
    int res = KMessageBox::warningContinueCancel(this,
						 i18n("If you switch side, your score will not be added to the highscores."),
						 QString::null, QString::null, "switch_side_warning");
    if ( res==KMessageBox::Cancel ) 
      return;

    m_cheating = true;
  }

  m_humanColor = opponent(m_humanColor);

  // Show the new state of things in the window.
  showScore();
  showColors();
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
    m_boardView->quitHint();
    setState(Ready);
  }
  else
    illegalMove();
}


// ----------------------------------------------------------------
//                   Slots for the game view


void KReversi::showColors()
{
  m_humanStatus   ->setPixmap(m_boardView->chipPixmap(humanColor(),    20));
  m_computerStatus->setPixmap(m_boardView->chipPixmap(computerColor(), 20));
}


// Show the move in the move view.

void  KReversi::showMove(uint moveno, Move &move)
{
  //FIXME: Error checks.
  QString colors[] = {
    i18n("White"),
    i18n("Black")
  };

  m_movesView->insertItem(QString("%1. %2 %3")
			  .arg(moveno)
			  .arg(colors[move.color()]).arg(move.asString()));

  // Mark the current move in the listbox.
  m_movesView->setCurrentItem(moveno - 1);
  m_movesView->ensureCurrentVisible();
}


// Show the number of pieces for each side.
//

void KReversi::showScore()
{
  m_humanStatus   ->setScore(m_krgame->score(humanColor()));
  m_computerStatus->setScore(m_krgame->score(computerColor()));
}


// A slot that is called when it is time to show whose turn it is.
//

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
  uint  black = m_krgame->score(Black);
  uint  white = m_krgame->score(White);

  if (black > white)
    showGameOver(Black);
  else if (black < white)
    showGameOver(White);
  else
    showGameOver(Nobody);

  showTurn(Nobody);
  m_boardView->quitShowLegalMoves();
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
  uint human    = m_krgame->score(humanColor());
  uint computer = m_krgame->score(computerColor());

  KExtHighscore::Score score;
  score.setScore(m_krgame->score(humanColor()));
  
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
  if ( !m_cheating && m_competitiveGame) {
    KExtHighscore::submitScore(score, this);
  }
  m_gameOver = true;
}


// A slot that is called when the state of the program changes.
//

void KReversi::slotStateChange(State state)
{
  if (state == Thinking){
    kapp->setOverrideCursor(waitCursor);
    stopAction->setEnabled(true);
  }
  else {
    kapp->restoreOverrideCursor();
    stopAction->setEnabled(false);
  }

  continueAction->setEnabled(interrupted());
}


// ----------------------------------------------------------------
//                        Private methods


// Handle the humans move.
//

void KReversi::humanMakeMove(int row, int col)
{
  if (state() != Ready) 
    return;

  Color color = m_krgame->toMove();

  // Create a move from the mouse click and see if it is legal.
  // If it is, then make a human move.
  Move  move(color, col + 1, row + 1);
  if (m_krgame->moveIsLegal(move)) {
    m_krgame->doMove(move);
    m_boardView->animateChanged(move);

    if (!m_krgame->moveIsAtAllPossible()) {
      m_boardView->updateBoard();
      showScore();
      setState(Ready);
      slotGameOver();
      return;
    }

    m_boardView->updateBoard();
    showScore();

    if (color != m_krgame->toMove())
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
  Color color    = m_krgame->toMove();
  Color opponent = ::opponent(color);

  showTurn(color);

  // Show legal moves for the computer.
  // FIXME: This shall be dependent on a KToggleAction later.
  //        Or perhaps never show legal moves for the computer.
  if (showLegalMovesAction->isChecked()) {
    moves = m_krgame->position().generateMoves(color);
    m_boardView->showLegalMoves(moves);
  }

  if (!m_krgame->moveIsPossible(color))
    return;
 
  // Make computer moves until the human can play or until the game is over.
  setState(Thinking);
  do {
    Move  move;

    if (!m_krgame->moveIsAtAllPossible()) {
      setState(Ready);
      slotGameOver();
      return;
    }

    move = m_engine->computeMove(m_krgame, m_competitiveGame);
    if (move.x() == -1) {
      setState(Ready);
      return;
    }
    usleep(300000); // Pretend we have to think hard.

    //playSound("click.wav");
    m_krgame->doMove(move);
    m_boardView->animateChanged(move);
    m_boardView->updateBoard();
    showScore();
  } while (!m_krgame->moveIsPossible(opponent));


  showTurn(opponent);
  setState(Ready);

  if (!m_krgame->moveIsAtAllPossible()) {
    slotGameOver();
    return;
  }

  // Show legal moves for the human.
  // FIXME: This shall be dependent on a KToggleAction later.
  if (showLegalMovesAction->isChecked()) {
    moves = m_krgame->position().generateMoves(m_humanColor);
    m_boardView->showLegalMoves(moves);
  }
}


// Handle an attempt to make an illegal move by the human.

void KReversi::illegalMove()
{
  KNotifyClient::event(winId(), "illegal_move", i18n("Illegal move"));
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
  config->writeEntry("NumberOfMoves", m_krgame->moveNumber());
  for (uint i = 0; i < m_krgame->moveNumber(); i++) {
    Move  move = m_krgame->move(i);

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
//

bool KReversi::loadGame(KConfig *config)
{
  slotInterrupt(); // stop thinking

  uint  nmoves = config->readNumEntry("NumberOfMoves", 0);
  if (nmoves==0) 
    return false;

  m_krgame->newGame();
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
    m_krgame->doMove(move);
  }

  m_humanColor      = (Color) config->readNumEntry("HumanColor");
  m_competitiveGame = (bool)  config->readNumEntry("Competitive");

  m_boardView->updateBoard(TRUE);
  showScore();
  setState(State(config->readNumEntry("State")));
  setStrength(config->readNumEntry("Strength", 1));
  //kdDebug() << "Competitive set to: " << m_competitiveGame << endl;

  if (interrupted())
    slotContinue();
  else {
    // Make the view show who is to move.
    showTurn(m_krgame->toMove());

    // Computer makes first move.
    if (m_humanColor != m_krgame->toMove())
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
  m_cheating = false;		// FIXME: Is this true?
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

  m_boardView->loadSettings();

  // Show the color of the human and the computer.
  showColors(); 
}


bool KReversi::isPlaying() const
{
  return ( m_krgame->moveNumber() != 0 && !m_gameOver );
}


void KReversi::setState(State newState)
{
  m_state = newState;
  emit stateChange(m_state);
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
