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


#include <qlayout.h>

#include <kapplication.h>
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
//                     class KReversi


#define PICDATA(x)  \
    KGlobal::dirs()->findResource("appdata", QString("pics/") + x)


KReversi::KReversi()
  : KZoomMainWindow(10, 300, 5, "kreversi"), 
     gameOver(false), 
     cheating(false)
{
  QWidget     *w;
  QHBoxLayout *top;

  KNotifyClient::startDaemon();

  w = new QWidget(this);
  setCentralWidget(w);

  top = new QHBoxLayout(w);
  top->addStretch(1);

  // The reversi board itself.
  m_board = new Board(w);
  top->addWidget(m_board);
  top->addStretch(1);
  
  // Populate the GUI.
  createStatusBar();
  createKActions();
  addWidget(m_board);

  // Connect some signals on the board with slots of the application
  connect(m_board, SIGNAL(score()), this, SLOT(slotScore()));
  connect(m_board, SIGNAL(gameWon(Color)), this, SLOT(slotGameEnded(Color)));
  connect(m_board, SIGNAL(turn(Color)), this, SLOT(slotTurn(Color)));
  connect(m_board, SIGNAL(statusChange(Board::State)),
          this, SLOT(slotStatusChange(Board::State)));

  loadSettings();

  setupGUI();
  init("popup");
  m_board->start();
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
  KStdGameAction::gameNew(this, SLOT(newGame()), actionCollection(), "game_new");
  KStdGameAction::load(this,  SLOT(openGame()), actionCollection());
  KStdGameAction::save(this,  SLOT(save()),     actionCollection());
  KStdGameAction::quit(this,  SLOT(close()),    actionCollection());
  KStdGameAction::hint(m_board, SLOT(hint()),     actionCollection(), "game_hint");
  undoAction = KStdGameAction::undo(m_board, SLOT(undo()), actionCollection(), "game_undo");

  // Non-standard Game Actions: Stop, Continue, Switch sides
  stopAction = new KAction(i18n("&Stop Thinking"), "game_stop",
			   Qt::Key_Escape, m_board, 
			   SLOT(interrupt()), actionCollection(), "game_stop");
  continueAction = new KAction(i18n("&Continue Thinking"), "reload",
	0, m_board, SLOT(doContinue()), actionCollection(), "game_continue");
  new KAction(i18n("S&witch Sides"), 0,
	0, this, SLOT(switchSides()), actionCollection(), "game_switch_sides");

  // Some more standard game actions: Highscores, Settings.
  KStdGameAction::highscores(this, SLOT(showHighScoreDialog()), actionCollection());
  KStdAction::preferences(this, SLOT(showSettings()), actionCollection());
}


// ----------------------------------------------------------------
//                              Slots


// A slot that is called when the user wants a new game.
//

void KReversi::newGame()
{
  // If we are already playing, treat this as a loss.
  if ( isPlaying() )
    KExtHighscore::submitScore(KExtHighscore::Lost, this);

  gameOver = false;
  cheating = false;

  m_board->newGame();
}


// Open an earlier saved game from the config file.
//
// FIXME: Should give a choice to load from an ordinary file (SGF?)
//

void KReversi::openGame()
{
  KConfig *config = kapp->config();
  config->setGroup("Savegame");

  if (m_board->loadGame(config))
    Prefs::setSkill(m_board->strength());
  updateColors();
}


// Save a game to the config file.
//
// FIXME: Should give a choice to save as an ordinary file (SGF?)
//

void KReversi::save()
{
  KConfig *config = kapp->config();
  config->setGroup("Savegame");
  m_board->saveGame(config);

  KMessageBox::information(this, i18n("Game saved."));
}


// Show the number of pieces for each side.
//

void KReversi::slotScore()
{
  m_humanStatus->setScore(m_board->score(m_board->humanColor()));
  m_computerStatus->setScore(m_board->score(m_board->computerColor()));
}


// A slot that is called when the game ends.
//

void KReversi::slotGameEnded(Color color) 
{
  // If the game already was over, do nothing.
  if (gameOver)
    return;

  statusBar()->message(i18n("End of game"));

  // Get the scores.
  uint human    = m_board->score(m_board->humanColor());
  uint computer = m_board->score(m_board->computerColor());

  KExtHighscore::Score score;
  score.setScore(m_board->score(m_board->humanColor()));
  
  // Show the winner in a messagebox.
  if ( color == Nobody ) {
    KNotifyClient::event(winId(), "draw", i18n("Draw!"));
    QString s = i18n("Game is drawn!\n\nYou     : %1\nComputer: %2")
                .arg(human).arg(computer);
    KMessageBox::information(this, s, i18n("Game Ended"));
    score.setType(KExtHighscore::Draw);
  }
  else if ( m_board->humanColor() == color ) {
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
  if ( !cheating && m_board->competitive()) {
    KExtHighscore::submitScore(score, this);
  }
  gameOver = true;
}


// A slot that is called when it is time to show whose turn it is.
//

void KReversi::slotTurn(Color color)
{
  // If we are not playing, do nothing. 
  if (gameOver)
    return;

  if (color == m_board->humanColor())
    statusBar()->message(i18n("Your turn"));
  else if (color == m_board->computerColor())
    statusBar()->message(i18n("Computer's turn"));
  else
    statusBar()->clear();
}


// A slot that is called when the state of the program changes.
//

void KReversi::slotStatusChange(Board::State status)
{
  if (status == Board::Thinking){
    kapp->setOverrideCursor(waitCursor);
    stopAction->setEnabled(true);
  }
  else {
    kapp->restoreOverrideCursor();
    stopAction->setEnabled(false);
  }

  continueAction->setEnabled(m_board->interrupted());
}


// ----------------------------------------------------------------


void KReversi::saveProperties(KConfig *c)
{
  m_board->saveGame(c);
}


void KReversi::readProperties(KConfig *c) {
  m_board->loadGame(c);
  gameOver = false;
  cheating = false;
}


void KReversi::showHighScoreDialog()
{
  KExtHighscore::show(this);
}


void KReversi::showSettings()
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


void KReversi::updateColors()
{
  m_humanStatus->setPixmap(m_board->chipPixmap(m_board->humanColor(), 20));
  m_computerStatus->setPixmap(m_board->chipPixmap(m_board->computerColor(), 20));
}


void KReversi::loadSettings()
{
  m_board->loadSettings();
  updateColors(); 
}


void KReversi::switchSides()
{
  if (m_board->moveNumber() != 0) {
    int res = KMessageBox::warningContinueCancel(this,
						 i18n("If you switch side, your score will not be added to the highscores."),
						 QString::null, QString::null, "switch_side_warning");
    if ( res==KMessageBox::Cancel ) 
      return;

    cheating = true;
  }

  m_board->switchSides();
  updateColors();
}

bool KReversi::isPlaying() const
{
  return ( m_board->moveNumber()!=0 && !gameOver );
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
