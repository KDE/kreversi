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

#include "config.h"

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
#include <kscoredialog.h>
#include <kurlrequester.h>

#include "prefs.h"
#include "Score.h"
#include "kreversi.h"
#include "kreversi.moc"
#include "board.h"
#include "settings.h"

const int SB_SCOREH	= 2;
const int SB_SCOREC	= 3;
const int SB_TURN       = 4;

#define PICDATA(x) KGlobal::dirs()->findResource("appdata", QString("pics/")+ x)

KReversi::KReversi()
    : KMainWindow(0, "kreversi")
{
  gameOver = false;

  // create reversi board
  QWidget *w = new QWidget(this);
  setCentralWidget(w);
  QHBoxLayout *top = new QHBoxLayout(w);
  top->addStretch(1);
  board = new Board(w);
  top->addWidget(board);
  top->addStretch(1);

  createKActions();
  createStatusBar();

  connect(board, SIGNAL(score()), this, SLOT(slotScore()));
  connect(board, SIGNAL(gameWon(Player)), this, SLOT(slotGameEnded(Player)));
  connect(board, SIGNAL(turn(Player)), this, SLOT(slotTurn(Player)));
  connect(board, SIGNAL(statusChange(int)), this, SLOT(slotStatusChange(int)));
  connect(board, SIGNAL(illegalMove()), this, SLOT(slotIllegalMove()));
  setAutoSaveSettings();

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
	0, board, SLOT(switchSides()), actionCollection(), "game_switch_sides");

  KStdGameAction::highscores(this, SLOT(showHighScoreDialog()), actionCollection());
  zoomInAction = KStdAction::zoomIn(this, SLOT(zoomIn()), actionCollection(), "zoomIn");
  zoomOutAction = KStdAction::zoomOut(this, SLOT(zoomOut()), actionCollection(), "zoomOut");
  #ifdef HAVE_MEDIATOOL
  soundAction = new KToggleAction(i18n("&Play Sounds"), 0, 0, 0, actionCollection(), "game_sound");
  #endif
  // Settings
  createStandardStatusBarAction();
  setStandardToolBarMenuEnabled(true);
  KStdAction::keyBindings(this, SLOT(configureKeyBindings()), actionCollection());
  KStdAction::preferences(this, SLOT(showSettings()), actionCollection());

  createGUI();
}

void KReversi::createStatusBar() {
  statusBar()->insertItem(i18n("XXXXX's turn"), SB_TURN,1);
  statusBar()->insertItem(i18n("You (XXXXX): 88"), SB_SCOREH,2);
  statusBar()->insertItem(i18n("Computer (XXXXX): 88"), SB_SCOREC,2);
}

void KReversi::newGame(){
  gameOver = false;
  board->newGame();
}

void KReversi::openGame(){
  KConfig *config = kapp->config();
  config->setGroup("Savegame");
  if(board->canLoad(config))
    board->loadGame(config);
}

void KReversi::save(){
  KConfig *config = kapp->config();
  config->setGroup("Savegame");
  board->saveGame(config);
  KMessageBox::information(this, i18n("Game saved."));
}

void KReversi::configureKeyBindings(){
  KKeyDialog::configure(actionCollection(), this);
}

bool KReversi::eventFilter(QObject *o, QEvent *e)
{
    if ( e->type()==QEvent::LayoutHint )
        setFixedSize(minimumSize()); // because QMainWindow and KMainWindow
                                     // do not manage fixed central widget
    return KMainWindow::eventFilter(o, e);
}

void KReversi::zoomIn(){
  board->zoomIn();
  Prefs::setZoom(board->zoom());
  Prefs::writeConfig();

  if(!board->canZoomIn())
    zoomInAction->setEnabled(false);
  if(board->canZoomOut())
    zoomOutAction->setEnabled(true);
}

void KReversi::zoomOut(){
  board->zoomOut();
  Prefs::setZoom(board->zoom());
  Prefs::writeConfig();

  if(board->canZoomIn())
    zoomInAction->setEnabled(true);
  if(!board->canZoomOut())
    zoomOutAction->setEnabled(false);
}

void KReversi::slotScore() {
  int black, white;
  QString s1, s2;

  board->getScore(black, white);

  if(board->chipType() == Board::Colored) {
    if(board->humanPlayer() == Black) {
      s1 = i18n("You (blue): %1").arg(black);
      s2 = i18n("Computer (red): %1").arg(white);
    } else {
      s2 = i18n("You (red): %1").arg(white);
      s1 = i18n("Computer (blue): %1").arg(black);
    }
  } else {
    if(board->humanPlayer() == Black) {
      s1 = i18n("You (black): %1").arg(black);
      s2 = i18n("Computer (white): %1").arg(white);
    } else {
      s2 = i18n("You (white): %1").arg(white);
      s1 = i18n("Computer (black): %1").arg(black);
    }
  }

  statusBar()->changeItem(s1, SB_SCOREH);
  statusBar()->changeItem(s2, SB_SCOREC);
}

void KReversi::slotGameEnded(Player player) {
  QString s;
  int winner, loser;

  statusBar()->changeItem(i18n("End of game"), SB_TURN);

  // get the score
  if(player == Black)
    board->getScore(winner, loser);
  else
    board->getScore(loser, winner);

  if(player == Nobody) {
    board->playSound(Board::DrawSound);
    s = i18n("Game is drawn!\n\nYou     : %1\nComputer: %2").arg(winner).arg(loser);
    KMessageBox::information(this, s, i18n("Game Ended"));
  } else if(board->humanPlayer() == player) {
    // calculate score
    int  st = board->strength();
    int sum = winner + loser;
    // 8 is the highest level so it is the bases for the scale.
    float score= (float)winner / sum * ((100/8)*st);

    board->playSound(Board::WonSound);
    s = i18n("Congratulations, you have won!\n\nYou     : %1\nComputer: %2\nYour rating %3%")
	      .arg(winner).arg(loser).arg(score,1);
    KMessageBox::information(this, s, i18n("Game Ended"));

    if (!gameOver){
      KScoreDialog highscore(KScoreDialog::Name, this);
      highscore.addField(KScoreDialog::Score, i18n("Rating"), "Rating");
      highscore.addField(KScoreDialog::Custom1, i18n("Score"), "NumChips");
      KScoreDialog::FieldInfo scoreInfo;

      scoreInfo[KScoreDialog::Custom1].setNum(winner);
      if (highscore.addScore((int)score, scoreInfo))
        highscore.exec();
    }
  } else {
    board->playSound(Board::LostSound);
    s = i18n("You have lost the game!\n\nYou     : %1\nComputer: %2")
	      .arg(loser).arg(winner);
    KMessageBox::information(this, s, i18n("Game Ended"));
  }
  gameOver = true;
}

void KReversi::slotTurn(Player player) {
  if (gameOver)
    return;

  QString s;

  if(player == board->humanPlayer())
      s = i18n("Your turn");
  else if(player == board->computerPlayer())
      s = i18n("Computer's turn");
  else
      s = "";
  statusBar()->changeItem(s, SB_TURN);
}

void KReversi::slotStatusChange(int status) {
  if(status == Board::THINKING){
    kapp->setOverrideCursor(waitCursor);
    stopAction->setEnabled(true);
  }
  else{
    kapp->restoreOverrideCursor();
    stopAction->setEnabled(false);
  }
  continueAction->setEnabled(board->interrupted());
}

void KReversi::slotIllegalMove() {
    board->playSound(Board::IllegalMoveSound);
}

void KReversi::saveProperties(KConfig *c) {
  board->saveGame(c);
}

void KReversi::readProperties(KConfig *c) {
  board->loadGame(c);
  gameOver = false;
}

/**
 * Bring up the standard kde high score dialog.
 */
void KReversi::showHighScoreDialog() {
  KScoreDialog highscore(KScoreDialog::Name, this);
  highscore.addField(KScoreDialog::Score, i18n("Rating"), "Rating");
  highscore.addField(KScoreDialog::Custom1, i18n("Score"), "NumChips");
  highscore.exec();
}

/**
 * Show Configure dialog.
 */
void KReversi::showSettings(){
  if(KConfigDialog::showDialog("settings"))
    return;

  KConfigDialog *dialog = new KConfigDialog(this, "settings", Prefs::self(), KDialogBase::Swallow);
  Settings *general = new Settings(0, "General");
  dialog->addPage(general, i18n("General"), "package_settings");
  connect(dialog, SIGNAL(settingsChanged()), board, SLOT(loadSettings()));
  dialog->show();
}

