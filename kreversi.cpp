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

//-----------------------------------------------------------------------------
StatusWidget::StatusWidget(const QString &text, QWidget *parent)
  : QWidget(parent, "status_widget")
{
  QHBoxLayout *hbox = new QHBoxLayout(this, 0, KDialog::spacingHint());
  QLabel *label = new QLabel(text, this);
  hbox->addWidget(label);
  _pixLabel = new QLabel(this);
  hbox->addWidget(_pixLabel);
  label = new QLabel(":", this);
  hbox->addWidget(label);
  _label = new QLabel(this);
  hbox->addWidget(_label);
}

void StatusWidget::setScore(uint s)
{
  _label->setText(QString::number(s));
}

void StatusWidget::setPixmap(const QPixmap &pixmap)
{
  _pixLabel->setPixmap(pixmap);
}

//-----------------------------------------------------------------------------
#define PICDATA(x) KGlobal::dirs()->findResource("appdata", QString("pics/")+ x)

KReversi::KReversi()
  : KZoomMainWindow(10, 300, 5, "kreversi"), gameOver(false), cheating(false)
{
  KNotifyClient::startDaemon();

  QWidget *w = new QWidget(this);
  setCentralWidget(w);
  QHBoxLayout *top = new QHBoxLayout(w);
  top->addStretch(1);
  board = new Board(w);
  top->addWidget(board);
  top->addStretch(1);
  
  createStatusBar();
  createKActions();
  addWidget(board);

  connect(board, SIGNAL(score()), this, SLOT(slotScore()));
  connect(board, SIGNAL(gameWon(Player)), this, SLOT(slotGameEnded(Player)));
  connect(board, SIGNAL(turn(Player)), this, SLOT(slotTurn(Player)));
  connect(board, SIGNAL(statusChange(Board::State)),
          this, SLOT(slotStatusChange(Board::State)));

  loadSettings();

  setupGUI();
  init("popup");
  board->start();
}

void KReversi::createKActions() {
  KStdGameAction::gameNew(this, SLOT(newGame()), actionCollection(), "game_new");
  KStdGameAction::load(this, SLOT(openGame()), actionCollection());
  KStdGameAction::save(this, SLOT(save()), actionCollection());
  KStdGameAction::quit(this, SLOT(close()), actionCollection());
  KStdGameAction::hint(board, SLOT(hint()), actionCollection(), "game_hint");
  undoAction = KStdGameAction::undo(board, SLOT(undo()), actionCollection(), "game_undo");

  stopAction = new KAction(i18n("&Stop Thinking"), "game_stop",
	Qt::Key_Escape, board, SLOT(interrupt()), actionCollection(), "game_stop");
  continueAction = new KAction(i18n("&Continue Thinking"), "reload",
	0, board, SLOT(doContinue()), actionCollection(), "game_continue");
  new KAction(i18n("S&witch Sides"), 0,
	0, this, SLOT(switchSides()), actionCollection(), "game_switch_sides");

  KStdGameAction::highscores(this, SLOT(showHighScoreDialog()), actionCollection());
  KStdAction::preferences(this, SLOT(showSettings()), actionCollection());
}

void KReversi::createStatusBar() {
  _humanStatus = new StatusWidget(i18n("You"), this);
  statusBar()->addWidget(_humanStatus, 0, true);
  _computerStatus = new StatusWidget(QString::null, this);
  statusBar()->addWidget(_computerStatus, 0, true);
}

void KReversi::newGame(){
  if ( isPlaying() )
    KExtHighscore::submitScore(KExtHighscore::Lost, this);
  gameOver = false;
  cheating = false;
  board->newGame();
}

void KReversi::openGame(){
  KConfig *config = kapp->config();
  config->setGroup("Savegame");
  if(board->loadGame(config))
    Prefs::setSkill(board->strength());
}

void KReversi::save(){
  KConfig *config = kapp->config();
  config->setGroup("Savegame");
  board->saveGame(config);
  KMessageBox::information(this, i18n("Game saved."));
}


void KReversi::slotScore() {
  _humanStatus->setScore(board->score(board->humanPlayer()));
  _computerStatus->setScore(board->score(board->computerPlayer()));
}

void KReversi::slotGameEnded(Player player) {
  if (gameOver) return;
  statusBar()->message(i18n("End of game"));

  // get the scores
  uint human = board->score(board->humanPlayer());
  uint computer = board->score(board->computerPlayer());

  KExtHighscore::Score score;
  score.setScore(board->score(board->humanPlayer()));
  
  if( player==Nobody ) {
    KNotifyClient::event(winId(), "draw", i18n("Draw!"));
    QString s = i18n("Game is drawn!\n\nYou     : %1\nComputer: %2").arg(human).arg(computer);
    KMessageBox::information(this, s, i18n("Game Ended"));
    score.setType(KExtHighscore::Draw);
  } else if( board->humanPlayer()==player ) {
    KNotifyClient::event(winId(), "won", i18n("Game won!"));
    QString s = i18n("Congratulations, you have won!\n\nYou     : %1\nComputer: %2")
                .arg(human).arg(computer);
    KMessageBox::information(this, s, i18n("Game Ended"));
    score.setType(KExtHighscore::Won);
  } else {
    KNotifyClient::event(winId(), "lost", i18n("Game lost!"));
    QString s = i18n("You have lost the game!\n\nYou     : %1\nComputer: %2")
                .arg(human).arg(computer);
    KMessageBox::information(this, s, i18n("Game Ended"));
    score.setType(KExtHighscore::Lost);
  }
  
  if ( !cheating ) KExtHighscore::submitScore(score, this);
  gameOver = true;
}

void KReversi::slotTurn(Player player) {
  if (gameOver)
    return;

  if(player == board->humanPlayer())
    statusBar()->message(i18n("Your turn"));
  else if(player == board->computerPlayer())
    statusBar()->message(i18n("Computer's turn"));
  else
    statusBar()->clear();
}

void KReversi::slotStatusChange(Board::State status) {
  if(status == Board::Thinking){
    kapp->setOverrideCursor(waitCursor);
    stopAction->setEnabled(true);
  }
  else{
    kapp->restoreOverrideCursor();
    stopAction->setEnabled(false);
  }
  continueAction->setEnabled(board->interrupted());
}

void KReversi::saveProperties(KConfig *c) {
  board->saveGame(c);
}

void KReversi::readProperties(KConfig *c) {
  board->loadGame(c);
  gameOver = false;
  cheating = false;
}

void KReversi::showHighScoreDialog() {
  KExtHighscore::show(this);
}

void KReversi::showSettings(){
  if(KConfigDialog::showDialog("settings"))
    return;

  KConfigDialog *dialog = new KConfigDialog(this, "settings", Prefs::self(), KDialogBase::Swallow);
  Settings *general = new Settings(0, "General");
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
  _humanStatus->setPixmap(board->chipPixmap(board->humanPlayer(), 20));
  _computerStatus->setPixmap(board->chipPixmap(board->computerPlayer(), 20));
}

void KReversi::loadSettings()
{
  board->loadSettings();
  updateColors(); 
}

void KReversi::switchSides()
{
  int res = KMessageBox::warningContinueCancel(this,
         i18n("If you switch side, your score will not be added to the highscores."),
         QString::null, QString::null, "switch_side_warning");
  if ( res==KMessageBox::Cancel ) return;
  cheating = true;
  board->switchSides();
  updateColors();
}

bool KReversi::isPlaying() const
{
  return ( board->moveNumber()!=0 && !gameOver );
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
