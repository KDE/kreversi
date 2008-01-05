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

#include <kggzgames/kggzseatsdialog.h>
#include <kggzmod/module.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <ktoggleaction.h>
#include <kdebug.h>
#include <kexthighscore.h>
#include <kicon.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <kstandardaction.h>
#include <kstandardgameaction.h>
#include <kselectaction.h>
#include <ktoolinvocation.h>

#include <QApplication>
#include <QListWidget>
#include <QGridLayout>
#include <QLabel>
#include <QDesktopWidget>

static const int PLAYER_STATUSBAR_ID = 1;
static const int COMP_STATUSBAR_ID = 2;

static QString moveToString( const KReversiPos& move )
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

KReversiMainWindow::KReversiMainWindow(QWidget* parent, bool startDemo )
    : KXmlGuiWindow(parent), m_scene(0), m_game(0),
      m_historyLabel(0), m_historyView(0),
      m_firstShow( true ), m_startInDemoMode( startDemo ),
      m_undoAct(0), m_hintAct(0), m_demoAct(0)
{
    statusBar()->insertItem( i18n("Your turn."), 0 );
    statusBar()->insertItem( i18n("You: %1", 2), PLAYER_STATUSBAR_ID );
    statusBar()->insertItem( i18n("%1: %2", opponentName(), 2), COMP_STATUSBAR_ID );

    slotNewGame();
    // m_scene is created in slotNewGame();

    QWidget *mainWid = new QWidget;
    QGridLayout *lay = new QGridLayout(mainWid);
    lay->setColumnStretch( 0, 1 );
    lay->setMargin(1);

    m_view = new KReversiView(m_scene, mainWid);
    lay->addWidget(m_view, 0, 0, 2, 1);

    m_historyLabel = new QLabel( i18n("Move history"), mainWid );
    lay->addWidget( m_historyLabel, 0, 1, Qt::AlignCenter );
    m_historyView = new QListWidget( mainWid );
    m_historyView->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Expanding );
    lay->addWidget(m_historyView, 1, 1);

    m_historyLabel->hide();
    m_historyView->hide();

    setupActions();
    loadSettings();

    setCentralWidget(mainWid);

    setupGUI(qApp->desktop()->availableGeometry().size()*0.7);
}

void KReversiMainWindow::setupActions()
{
    // Game
    KStandardGameAction::gameNew(this, SLOT(slotNewGame()), actionCollection());
    KStandardGameAction::highscores(this, SLOT(slotHighscores()), actionCollection());
    KStandardGameAction::quit(this, SLOT(close()), actionCollection());

    // Move
    m_undoAct = KStandardGameAction::undo(this, SLOT(slotUndo()), actionCollection());
    m_undoAct->setEnabled( false ); // nothing to undo at the start of the game
    m_hintAct = KStandardGameAction::hint(m_scene, SLOT(slotHint()), actionCollection());
    m_demoAct = KStandardGameAction::demo(this, SLOT(slotToggleDemoMode()), actionCollection());

    m_seatsAct = actionCollection()->addAction( "game_seats" );
    m_seatsAct->setIcon( KIcon("roll") );
    m_seatsAct->setText( i18n("Players and Seats") );
    m_seatsAct->setShortcut( Qt::Key_S );
    connect(m_seatsAct, SIGNAL(triggered(bool)), SLOT(slotSeats()) );

    // View
    KToggleAction *showLast = new KToggleAction(KIcon("lastmoves"), i18n("Show Last Move"), this);
    actionCollection()->addAction("show_last_move", showLast);
    connect( showLast, SIGNAL(triggered(bool)), m_scene, SLOT(setShowLastMove(bool)) );

    KToggleAction *showLegal = new KToggleAction(KIcon("legalmoves"), i18n("Show Legal Moves"), this);
    actionCollection()->addAction("show_legal_moves", showLegal);
    connect( showLegal, SIGNAL(triggered(bool)), m_scene, SLOT(setShowLegalMoves(bool)) );

    m_animSpeedAct = new KSelectAction(i18n("Animation Speed"), this);
    actionCollection()->addAction("anim_speed", m_animSpeedAct);

    QStringList acts;
    acts << i18n("Slow") << i18n("Normal") << i18n("Fast");
    m_animSpeedAct->setItems(acts);
    connect( m_animSpeedAct, SIGNAL(triggered(int)), SLOT(slotAnimSpeedChanged(int)) );

    m_skillAct = new KSelectAction(i18n("Computer Skill"), actionCollection());
    actionCollection()->addAction("skill", m_skillAct);

    acts.clear();
    // FIXME dimsuz: give them good names
    acts << i18n("Very Easy") << i18n("Easy") << i18n("Normal");
    acts << i18n("Hard") << i18n("Very Hard") << i18n("Unbeatable") << i18n("Champion");
    m_skillAct->setItems(acts);
    connect(m_skillAct, SIGNAL(triggered(int)), SLOT(slotSkillChanged(int)) );

    m_coloredChipsAct = new KToggleAction( i18n("Use Colored Chips"), this );
    actionCollection()->addAction( "use_colored_chips", m_coloredChipsAct );
    connect( m_coloredChipsAct, SIGNAL(triggered(bool)), SLOT(slotUseColoredChips(bool)) );

    // NOTE: read/write this from/to config file? Or not necessary?
    KToggleAction *showMovesAct = new KToggleAction( KIcon("view-history"), i18n("Show Move History"), this );
    actionCollection()->addAction("show_moves", showMovesAct);
    connect( showMovesAct, SIGNAL(triggered(bool)), SLOT(slotShowMovesHistory(bool)) );

    addAction(m_seatsAct); // FIXME (josef): is this needed?

    if(!KGGZMod::Module::isGGZ())
    {
        // disable multiplayer actions
        m_seatsAct->setEnabled(false);
    }
    else
    {
        // disable singleplayer actions
        actionCollection()->action( "game_new" )->setEnabled(false);
        m_hintAct->setEnabled(false);
        m_demoAct->setEnabled(false);
        m_skillAct->setEnabled(false);
        m_undoAct->setEnabled(false);
    }
}

void KReversiMainWindow::loadSettings()
{
    int skill = Preferences::skill();
    m_skillAct->setCurrentItem( skill - 1 );

    m_animSpeedAct->setCurrentItem( Preferences::animationSpeed() );

    m_coloredChipsAct->setChecked( Preferences::useColoredChips() );
}

void KReversiMainWindow::slotSkillChanged(int skill)
{
    // m_game takes it from 1 to 7
    m_game->setComputerSkill( skill+1 );
    Preferences::setSkill( skill+1 );
    Preferences::self()->writeConfig();
}

void KReversiMainWindow::slotAnimSpeedChanged(int speed)
{
    m_scene->setAnimationSpeed(speed);
    Preferences::setAnimationSpeed(speed);
    Preferences::self()->writeConfig();
}

void KReversiMainWindow::slotUseColoredChips(bool toggled)
{
    QString chipsPrefix = m_coloredChipsAct->isChecked() ? "chip_color_%1" : "chip_bw_%1";
    m_scene->setChipsPrefix( chipsPrefix );
    Preferences::setUseColoredChips(toggled);
    Preferences::self()->writeConfig();
}

void KReversiMainWindow::slotShowMovesHistory(bool toggled)
{
    m_historyLabel->setVisible(toggled);
    m_historyView->setVisible(toggled);

    m_scene->setShowBoardLabels( toggled );
    m_view->resetCachedContent();
    m_view->update();
}

void KReversiMainWindow::slotToggleDemoMode()
{
    bool toggled = false;
    if( m_scene->isInDemoMode() )
    {
        toggled = false;
        m_demoAct->setIcon( KIcon("media-playback-start") );
        m_demoAct->setChecked( false );
    }
    else
    {
        toggled = true;
        m_demoAct->setIcon( KIcon("media-playback-pause") );
        m_demoAct->setChecked( true );
    }

    m_scene->toggleDemoMode(toggled);

    m_undoAct->setEnabled( !toggled );
    m_hintAct->setEnabled( !toggled );
}

void KReversiMainWindow::slotNewGame()
{
    if(KGGZMod::Module::isGGZ())
    {
        setCaption(i18n("Online game"));
    }
    else
    {
        if(m_hintAct)
            m_hintAct->setEnabled( true );
    }

    if(m_demoAct)
    {
        m_demoAct->setChecked( false );
        m_demoAct->setIcon( KIcon("media-playback-start") );
    }
    if(m_undoAct)
        m_undoAct->setEnabled( false );

    if(m_historyView)
        m_historyView->clear();

    m_game = new KReversiGame;
    m_game->setComputerSkill( Preferences::skill() );
    connect( m_game, SIGNAL(gameOver()), SLOT(slotGameOver()) );

    if(m_scene == 0) // if called first time
    {
        QString chipsPrefix = Preferences::useColoredChips() ? "chip_color_%1" : "chip_bw_%1";
        m_scene = new KReversiScene(m_game, chipsPrefix);
        m_scene->setAnimationSpeed( Preferences::animationSpeed() );
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
    m_undoAct->setEnabled(true);

    if ( m_scene->isInDemoMode() )
    {
        // let's loop! :-)
        slotToggleDemoMode();// turn off
        slotNewGame();
        slotToggleDemoMode();// turn on
        return;
    }

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
    res += i18n("\n%1: %2", opponentName(), whiteScore);

    KMessageBox::information( this, res, i18n("Game over") );
    // FIXME dimsuz: don't submit if in demo mode!
    KExtHighscore::submitScore(score, this);
}

void KReversiMainWindow::slotMoveFinished()
{
    if( !KGGZMod::Module::isGGZ())
    {
        if( !m_demoAct->isChecked() )
            m_undoAct->setEnabled( m_game->canUndo() );
    }

    // add last move to history list
    KReversiPos move = m_game->getLastMove();
    QString numStr = QString::number( m_historyView->count()+1 ) + ". ";
    m_historyView->addItem( numStr + moveToString(move) );
    QListWidgetItem *last = m_historyView->item( m_historyView->count() - 1 );
    m_historyView->setCurrentItem( last );
    m_historyView->scrollToItem( last );

    statusBar()->changeItem( m_game->isComputersTurn() ? opponentName() : i18n("Your turn."), 0 );

    statusBar()->changeItem( i18n("You: %1", m_game->playerScore(Black) ), PLAYER_STATUSBAR_ID);
    statusBar()->changeItem( i18n("%1: %2", opponentName(), m_game->playerScore(White) ), COMP_STATUSBAR_ID);
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

        statusBar()->changeItem( i18n("You: %1", m_game->playerScore(Black) ), PLAYER_STATUSBAR_ID);
        statusBar()->changeItem( i18n("%1: %2", opponentName(), m_game->playerScore(White) ), COMP_STATUSBAR_ID);

        m_undoAct->setEnabled( m_game->canUndo() );
        // if the user hits undo after game is over
        // let's give him a chance to ask for a hint ;)
        m_hintAct->setEnabled( true );
    }
}

void KReversiMainWindow::slotHighscores()
{
    if(!KGGZMod::Module::isGGZ())
    {
        KExtHighscore::show(this);
    }
    else
    {
        // FIXME (josef): GGZ needs KExtHighscore integration
        KToolInvocation::invokeBrowser("http://www.ggzcommunity.org/db/games?lookup=Reversi");
    }
}

void KReversiMainWindow::slotSeats()
{
    KGGZSeatsDialog *dlg = new KGGZSeatsDialog();
    Q_UNUSED(dlg);
    // FIXME (josef): make this a real non-modal dialog?
    // FIXME (josef): player might want to use it alongside game window
}

void KReversiMainWindow::showEvent( QShowEvent* )
{
    if ( m_firstShow && m_startInDemoMode )
    {
        kDebug() << "starting demo...";
        slotToggleDemoMode();
    }
    m_firstShow = false;
}

QString KReversiMainWindow::opponentName()
{
    if(KGGZMod::Module::isGGZ())
    {
        return i18n("Opponent");
    }
    else
    {
        return i18n("Computer");
    }
}

#include "mainwindow.moc"
