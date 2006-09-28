/*******************************************************************
 *
 * Copyright 2006 Dmitry Suzdalev <dimsuz@gmail.com>
 *
 * This file is part of the KDE project "KReversi"
 *
 * KReversi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * KReversi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KReversi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 ********************************************************************/
#include "mainwindow.h"
#include "kreversigame.h"
#include "kreversiscene.h"
#include "kreversiview.h"
#include "preferences.h"

#include <kaction.h>
#include <ktoggleaction.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kexthighscore.h>
#include <kicon.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <kstdaction.h>
#include <kstdgameaction.h>
#include <kselectaction.h>

#include <QGraphicsView>
#include <QListWidget>
#include <QGridLayout>

static const int PLAYER_STATUSBAR_ID = 1;
static const int COMP_STATUSBAR_ID = 2;

static QString moveToString( const KReversiMove& move )
{
    QString moveString;
    if( Preferences::useColoredChips() )
        moveString = (move.color == Black ? i18n("Blue") : i18n("Red") );
    else
        moveString = (move.color == Black ? i18n("Black") : i18n("White") );

    const char labelsHor[] = "ABCDEFGH";
    const char labelsVer[] = "12345678";

    moveString += ' ';
    moveString += labelsHor[move.col];
    moveString += labelsVer[move.row];

    return moveString;
}

KReversiMainWindow::KReversiMainWindow(QWidget* parent)
    : KMainWindow(parent), m_scene(0), m_game(0),
    m_historyLabel(0), m_historyView(0), m_undoAct(0),
    m_hintAct(0), m_demoAct(0)
{
    statusBar()->insertItem( i18n("Your turn."), 0 );
    statusBar()->insertItem( i18n("You: %1", 2), PLAYER_STATUSBAR_ID );
    statusBar()->insertItem( i18n("Computer: %1", 2), COMP_STATUSBAR_ID );

    slotNewGame();
    // m_scene is created in slotNewGame();

    QWidget *mainWid = new QWidget;
    QGridLayout *lay = new QGridLayout(mainWid);
    lay->setMargin(1);

    m_view = new KReversiView(m_scene, mainWid);
    m_view->show();
    lay->addWidget(m_view, 0, 0, 2, 1);

    m_historyLabel = new QLabel( i18n("Moves history"), mainWid );
    lay->addWidget( m_historyLabel, 0, 1, Qt::AlignCenter );
    m_historyView = new QListWidget( mainWid );
    lay->addWidget(m_historyView, 1, 1);

    m_historyLabel->hide();
    m_historyView->hide();

    m_scene->setBackground( KStandardDirs::locate("appdata", "pics/default_board.svgz"),
            KStandardDirs::locate("appdata", "pics/board_numbers.svgz") );

    setupActions();
    loadSettings();

    setCentralWidget(mainWid);

    setupGUI();
}

void KReversiMainWindow::setupActions()
{
    KAction *newGameAct = KStdAction::openNew(this, SLOT(slotNewGame()), actionCollection(), "new_game");
    KAction *quitAct = KStdAction::quit(this, SLOT(close()), actionCollection(), "quit");
    m_undoAct = KStdAction::undo( this, SLOT(slotUndo()), actionCollection(), "undo" );
    m_undoAct->setEnabled( false ); // nothing to undo at the start of the game
    m_hintAct = new KAction( KIcon("wizard"), i18n("Hint"), actionCollection(), "hint" );
    m_hintAct->setShortcut( Qt::Key_H );
    connect( m_hintAct, SIGNAL(triggered(bool)), m_scene, SLOT(slotHint()) );

    m_demoAct = new KToggleAction( KIcon("1rightarrow"), i18n("Demo"), actionCollection(), "demo" );
    m_demoAct->setShortcut( Qt::Key_D );
    connect(m_demoAct, SIGNAL(triggered(bool)), SLOT(slotDemoMode(bool)) );

    KToggleAction *showLast = new KToggleAction(KIcon("lastmoves"), i18n("Show last move"), actionCollection(), "show_last_move");
    connect( showLast, SIGNAL(triggered(bool)), m_scene, SLOT(setShowLastMove(bool)) );

    KToggleAction *showLegal = new KToggleAction(KIcon("legalmoves"), i18n("Show legal moves"), actionCollection(), "show_legal_moves" );
    connect( showLegal, SIGNAL(triggered(bool)), m_scene, SLOT(setShowLegalMoves(bool)) );

    m_animSpeedAct = new KSelectAction(i18n("Animation speed"), actionCollection(), "anim_speed");
    QStringList acts;
    acts << i18n("Slow") << i18n("Normal") << i18n("Fast");
    m_animSpeedAct->setItems(acts);
    connect( m_animSpeedAct, SIGNAL(triggered(int)), SLOT(slotAnimSpeedChanged(int)) );

    m_skillAct = new KSelectAction(i18n("Computer skill"), actionCollection(), "skill" );
    acts.clear();
    // FIXME dimsuz: give them good names
    acts << i18n("Very easy") << i18n("Easy") << i18n("Normal");
    acts << i18n("Hard") << i18n("Very Hard") << i18n("Unbeatable") << i18n("Champion");
    m_skillAct->setItems(acts);
    connect(m_skillAct, SIGNAL(triggered(int)), SLOT(slotSkillChanged(int)) );

    m_coloredChipsAct = new KToggleAction( i18n("Use colored chips"), actionCollection(), "use_colored_chips" );
    connect( m_coloredChipsAct, SIGNAL(triggered(bool)), SLOT(slotUseColoredChips(bool)) );

    // NOTE: read/write this from/to config file? Or not necessary?
    KToggleAction *showMovesAct = new KToggleAction( i18n("Show moves history"), actionCollection(), "show_moves" );
    connect( showMovesAct, SIGNAL(triggered(bool)), SLOT(slotShowMovesHistory(bool)) );

    KStdGameAction::highscores(this, SLOT(slotHighscores()), actionCollection());

    addAction(newGameAct);
    addAction(quitAct);
    addAction(m_undoAct);
    addAction(m_hintAct);
    addAction(m_demoAct);
}

void KReversiMainWindow::loadSettings()
{
    int skill = Preferences::skill();
    m_skillAct->setCurrentItem( skill - 1 );
    slotSkillChanged( skill - 1 );

    m_animSpeedAct->setCurrentItem( Preferences::animationSpeed() );
    slotAnimSpeedChanged( Preferences::animationSpeed() );

    m_coloredChipsAct->setChecked( Preferences::useColoredChips() );
    slotUseColoredChips( Preferences::useColoredChips() );
}

void KReversiMainWindow::slotSkillChanged(int skill)
{
    // m_game takes it from 1 to 7
    m_game->setComputerSkill( skill+1 );
    Preferences::setSkill( skill+1 );
    Preferences::writeConfig();
}

void KReversiMainWindow::slotAnimSpeedChanged(int speed)
{
    m_scene->setAnimationSpeed(speed);
    Preferences::setAnimationSpeed(speed);
    Preferences::writeConfig();
}

void KReversiMainWindow::slotUseColoredChips(bool toggled)
{
    QString chipsPath = m_coloredChipsAct->isChecked() ? "pics/chips.svgz" : "pics/chips_mono.svgz";
    m_scene->setChipsPixmap( KStandardDirs::locate("appdata", chipsPath) );
    Preferences::setUseColoredChips(toggled);
    Preferences::writeConfig();
}

void KReversiMainWindow::slotShowMovesHistory(bool toggled)
{
    m_historyLabel->setVisible(toggled);
    m_historyView->setVisible(toggled);
}

void KReversiMainWindow::slotDemoMode(bool toggled)
{
    m_scene->toggleDemoMode(toggled);

    m_undoAct->setEnabled( !toggled );
    m_hintAct->setEnabled( !toggled );
}

void KReversiMainWindow::slotNewGame()
{
    delete m_game;
    m_game = new KReversiGame;
    connect( m_game, SIGNAL(gameOver()), SLOT(slotGameOver()) );
    connect( m_game, SIGNAL(computerCantMove()), SLOT(slotComputerCantMove()) );

    if(m_hintAct)
        m_hintAct->setEnabled( true );
    if(m_demoAct)
        m_demoAct->setChecked( false );
    if(m_undoAct)
        m_undoAct->setEnabled( false );

    if(m_historyView)
        m_historyView->clear();

    if(m_scene == 0) // if called first time
    {
        // FIXME dimsuz: if chips[_mono].png not found give error end exit
        m_scene = new KReversiScene(m_game, KStandardDirs::locate("appdata", "pics/chips_mono.svgz"));
        connect( m_scene, SIGNAL(moveFinished()), SLOT(slotMoveFinished()) );
    }
    else
    {
        m_scene->setGame( m_game );
    }

    statusBar()->changeItem( i18n("Your turn."), 0 );
}

void KReversiMainWindow::slotGameOver()
{
    m_hintAct->setEnabled(false);
    m_demoAct->setChecked(false);
    m_undoAct->setEnabled(true);

    statusBar()->changeItem( i18n("GAME OVER."), 0 );

    int blackScore = m_game->playerScore(Black);
    int whiteScore = m_game->playerScore(White);

    // FIXME dimsuz: use lowest skill that was during ONE game
    KExtHighscore::setGameType( Preferences::skill()-1 );
    KExtHighscore::Score score;
    score.setScore(blackScore);

    QString res;
    if( blackScore == whiteScore )
    {
        res = i18n("Game is drawn!");
        score.setType( KExtHighscore::Draw );
    }
    else if( blackScore > whiteScore )
    {
        res = i18n("You win!");
        score.setType( KExtHighscore::Won );
    }
    else
    {
        res = i18n("You have lost!");
        score.setType( KExtHighscore::Lost );
    }

    res += i18n("\nYou: %1", blackScore);
    res += i18n("\nComputer: %1", whiteScore);

    KMessageBox::information( this, res, i18n("Game over") );
    // FIXME dimsuz: don't submit if in demo mode!
    KExtHighscore::submitScore(score, this);
}

void KReversiMainWindow::slotMoveFinished()
{
    if( !m_demoAct->isChecked() )
        m_undoAct->setEnabled( m_game->canUndo() );    

    // add last move to history list
    KReversiMove move = m_game->getLastMove();
    QString numStr = QString::number( m_historyView->count()+1 ) + ". ";
    m_historyView->addItem( numStr + moveToString(move) );
    QListWidgetItem *last = m_historyView->item( m_historyView->count() - 1 );
    m_historyView->setCurrentItem( last );
    m_historyView->scrollToItem( last );

    statusBar()->changeItem( m_game->isComputersTurn() ? i18n("Computer turn.") : i18n("Your turn."), 0 );
    statusBar()->changeItem( i18n("You: %1", m_game->playerScore(Black) ), PLAYER_STATUSBAR_ID);
    statusBar()->changeItem( i18n("Computer: %1", m_game->playerScore(White) ), COMP_STATUSBAR_ID);
}

void KReversiMainWindow::slotComputerCantMove()
{
    statusBar()->changeItem( i18n("Computer can not make move. Your turn."), 0 );
}

void KReversiMainWindow::slotUndo()
{
    if( !m_scene->isBusy() )
    {
        // scene will automatically notice that it needs to update
        int numUndone = m_game->undo();
        // remove last numUndone items from historyView
        for(int i=0;i<numUndone; ++i)
            delete m_historyView->takeItem( m_historyView->count()-1 );

        QListWidgetItem *last = m_historyView->item( m_historyView->count() - 1 );
        m_historyView->setCurrentItem( last );
        m_historyView->scrollToItem( last );

        m_undoAct->setEnabled( m_game->canUndo() );    
        // if the user hits undo after game is over
        // let's give him a chance to ask for a hint ;)
        m_hintAct->setEnabled( true );
    }
}

void KReversiMainWindow::slotHighscores()
{
    KExtHighscore::show(this);
}

#include "mainwindow.moc"
