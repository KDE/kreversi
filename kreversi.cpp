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
  connect(board, SIGNAL(statusChange(Board::State)),
          this, SLOT(slotStatusChange(Board::State)));
  connect(board, SIGNAL(illegalMove()), this, SLOT(slotIllegalMove()));
  setAutoSaveSettings();

  loadSettings();
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
  _humanStatus = new StatusWidget(i18n("You"), this);
  statusBar()->addWidget(_humanStatus, 0, true);
  _computerStatus = new StatusWidget(QString::null, this);
  statusBar()->addWidget(_computerStatus, 0, true);
}

void KReversi::newGame(){
  gameOver = false;
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
  _humanStatus->setScore(board->score(board->humanPlayer()));
  _computerStatus->setScore(board->score(board->computerPlayer()));
}

void KReversi::slotGameEnded(Player player) {
  QString s;

  statusBar()->message(i18n("End of game"));

  // get the scores
  uint human = board->score(board->humanPlayer());
  uint computer = board->score(board->computerPlayer());

  if(player == Nobody) {
    board->playSound(Board::DrawSound);
    s = i18n("Game is drawn!\n\nYou     : %1\nComputer: %2").arg(human).arg(computer);
    KMessageBox::information(this, s, i18n("Game Ended"));
  } else if(board->humanPlayer() == player) {
    // calculate score
    int  st = board->strength();
    int sum = human + computer;
    // 8 is the highest level so it is the bases for the scale.
    float score= (float)human / sum * ((100/8)*st);

    board->playSound(Board::WonSound);
    s = i18n("Congratulations, you have won!\n\nYou     : %1\nComputer: %2\nYour rating %3%")
	      .arg(human).arg(computer).arg(score,1);
    KMessageBox::information(this, s, i18n("Game Ended"));

    if (!gameOver){
      KScoreDialog highscore(KScoreDialog::Name | KScoreDialog::Date, this);
      highscore.addField(KScoreDialog::Score, i18n("Rating"), "Rating");
      highscore.addField(KScoreDialog::Custom1, i18n("Score"), "NumChips");
      KScoreDialog::FieldInfo scoreInfo;

      scoreInfo.insert(KScoreDialog::Date, QDateTime::currentDateTime().toString());
      scoreInfo[KScoreDialog::Custom1].setNum(human);
      if (highscore.addScore((int)score, scoreInfo))
        highscore.exec();
    }
  } else {
    board->playSound(Board::LostSound);
    s = i18n("You have lost the game!\n\nYou     : %1\nComputer: %2")
	      .arg(human).arg(computer);
    KMessageBox::information(this, s, i18n("Game Ended"));
  }
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

void KReversi::showHighScoreDialog() {
  KScoreDialog highscore(KScoreDialog::Name | KScoreDialog::Date, this);
  highscore.addField(KScoreDialog::Score, i18n("Rating"), "Rating");
  highscore.addField(KScoreDialog::Custom1, i18n("Score"), "NumChips");
  highscore.exec();
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
  board->switchSides();
  updateColors();
}
