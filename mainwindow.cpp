/*******************************************************************
 *
 * Copyright 2006 Dmitry Suzdalev <dimsuz@gmail.com>
 * Copyright 2010 Brian Croom <brian.s.croom@gmail.com>
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
#include <KgDifficulty>

#include <QApplication>
#include <QListWidget>
#include <QDockWidget>
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

    moveString += QLatin1Char( ' ' );
    moveString += QLatin1Char( labelsHor[move.col] );
    moveString += QLatin1Char( labelsVer[move.row] );

    return moveString;
}

KReversiMainWindow::KReversiMainWindow(QWidget* parent, bool startDemo )
    : KXmlGuiWindow(parent), m_scene(0), m_game(0),
      m_historyDock(0), m_historyView(0),
      m_firstShow( true ), m_startInDemoMode( startDemo ), m_lowestSkill(6),
      m_undoAct(0), m_hintAct(0), m_demoAct(0)
{
    statusBar()->insertItem( i18n("Your turn."), 0 );
    statusBar()->insertItem( i18n("You: %1", 2), PLAYER_STATUSBAR_ID );
    statusBar()->insertItem( i18n("%1: %2", opponentName(), 2), COMP_STATUSBAR_ID );

    Kg::difficulty()->addStandardLevelRange(
        KgDifficultyLevel::VeryEasy, KgDifficultyLevel::Impossible,
        KgDifficultyLevel::Easy //default
    );
    KgDifficultyGUI::init(this);
    connect(Kg::difficulty(), SIGNAL(currentLevelChanged(const KgDifficultyLevel*)), SLOT(levelChanged()));

    slotNewGame();
    // m_scene is created in slotNewGame();

    m_view = new KReversiView(m_scene, this);
    setCentralWidget(m_view);

    m_historyView = new QListWidget( this );
    m_historyView->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Expanding );
    m_historyDock = new QDockWidget( i18n("Move History") );
    m_historyDock->setWidget(m_historyView);
    m_historyDock->setObjectName("history_dock");

    m_historyDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_historyDock);
    m_historyDock->hide();

    setupActions();
    loadSettings();

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

    // View
    KToggleAction *showLast = new KToggleAction(KIcon( QLatin1String( "lastmoves") ), i18n("Show Last Move" ), this);
    actionCollection()->addAction( QLatin1String( "show_last_move" ), showLast);
    connect( showLast, SIGNAL(triggered(bool)), m_scene, SLOT(setShowLastMove(bool)) );

    KToggleAction *showLegal = new KToggleAction(KIcon( QLatin1String( "legalmoves") ), i18n("Show Legal Moves" ), this);
    actionCollection()->addAction( QLatin1String( "show_legal_moves" ), showLegal);
    connect( showLegal, SIGNAL(triggered(bool)), m_scene, SLOT(setShowLegalMoves(bool)) );

    m_animSpeedAct = new KSelectAction(i18n("Animation Speed"), this);
    actionCollection()->addAction( QLatin1String( "anim_speed" ), m_animSpeedAct);

    QStringList acts;
    acts << i18n("Slow") << i18n("Normal") << i18n("Fast");
    m_animSpeedAct->setItems(acts);
    connect( m_animSpeedAct, SIGNAL(triggered(int)), SLOT(slotAnimSpeedChanged(int)) );

    m_coloredChipsAct = new KToggleAction( i18n("Use Colored Chips"), this );
    actionCollection()->addAction( QLatin1String(  "use_colored_chips" ), m_coloredChipsAct );
    connect( m_coloredChipsAct, SIGNAL(triggered(bool)), SLOT(slotUseColoredChips(bool)) );

    // NOTE: read/write this from/to config file? Or not necessary?
    KToggleAction *showMovesAct = new KToggleAction( KIcon( QLatin1String( "view-history") ), i18n("Show Move History" ), this );
    actionCollection()->addAction( QLatin1String( "show_moves" ), showMovesAct);
    connect( showMovesAct, SIGNAL(triggered(bool)), SLOT(slotShowMovesHistory(bool)) );
}

void KReversiMainWindow::loadSettings()
{
    m_animSpeedAct->setCurrentItem( Preferences::animationSpeed() );
    m_coloredChipsAct->setChecked( Preferences::useColoredChips() );
}

void KReversiMainWindow::levelChanged()
{
    int skill;

    switch(Kg::difficultyLevel()) {
        case KgDifficultyLevel::VeryEasy:
            skill=0;
            break;
        case KgDifficultyLevel::Easy:
        default:
            skill=1;
            break;
        case KgDifficultyLevel::Medium:
            skill=2;
            break;
        case KgDifficultyLevel::Hard:
            skill=3;
            break;
        case KgDifficultyLevel::VeryHard:
            skill=4;
            break;
        case KgDifficultyLevel::ExtremelyHard:
            skill=5;
            break;
        case KgDifficultyLevel::Impossible:
            skill=6;
            break;
    }
    m_lowestSkill = qMin(m_lowestSkill, skill);

    // m_game takes it from 1 to 7
    m_game->setComputerSkill( skill + 1 );
}

void KReversiMainWindow::slotAnimSpeedChanged(int speed)
{
    m_scene->setAnimationSpeed(speed);
    Preferences::setAnimationSpeed(speed);
    Preferences::self()->writeConfig();
}

void KReversiMainWindow::slotUseColoredChips(bool toggled)
{
    QString chipsPrefix = m_coloredChipsAct->isChecked() ? QLatin1String( "chip_color" ) : QLatin1String( "chip_bw" );
    m_scene->setChipsPrefix( chipsPrefix );
    Preferences::setUseColoredChips(toggled);
    Preferences::self()->writeConfig();
}

void KReversiMainWindow::slotShowMovesHistory(bool toggled)
{
    m_historyDock->setVisible(toggled);

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
        m_demoAct->setIcon( KIcon( QLatin1String( "media-playback-start" )) );
        m_demoAct->setChecked( false );
    }
    else
    {
        // if game is over when user launched Demo, start new game
        // before Demo starts
        if (m_game && m_game->isGameOver())
            slotNewGame();

        toggled = true;
        m_demoAct->setIcon( KIcon( QLatin1String( "media-playback-pause" )) );
        m_demoAct->setChecked( true );
    }

    m_scene->toggleDemoMode(toggled);

    m_undoAct->setEnabled( !toggled );
    m_hintAct->setEnabled( !toggled );
}

void KReversiMainWindow::slotNewGame()
{
    m_lowestSkill = 6;

    if(m_hintAct)
        m_hintAct->setEnabled( true );

    if(m_demoAct)
    {
        m_demoAct->setChecked( false );
        m_demoAct->setIcon( KIcon( QLatin1String( "media-playback-start" )) );
    }
    if(m_undoAct)
        m_undoAct->setEnabled( false );

    if(m_historyView)
        m_historyView->clear();

    m_game = new KReversiGame;
    levelChanged();
    connect( m_game, SIGNAL(gameOver()), SLOT(slotGameOver()) );

    if(m_scene == 0) // if called first time
    {
        QString chipsPrefix = Preferences::useColoredChips() ? QLatin1String( "chip_color" ) : QLatin1String( "chip_bw" );
        m_scene = new KReversiScene(m_game, chipsPrefix);
        m_scene->setAnimationSpeed( Preferences::animationSpeed() );
        connect( m_scene, SIGNAL(moveFinished()), SLOT(slotMoveFinished()) );
    }
    else
    {
        m_scene->setGame( m_game );
    }

    statusBar()->changeItem( i18n("Your turn."), 0 );
    updateScores();
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

    statusBar()->changeItem( i18n("GAME OVER"), 0 );

    int blackScore = m_game->playerScore(Black);
    int whiteScore = m_game->playerScore(White);

    KExtHighscore::setGameType( m_lowestSkill );
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
    if( !m_demoAct->isChecked() )
        m_undoAct->setEnabled( m_game->canUndo() );

    // add last move to history list
    KReversiPos move = m_game->getLastMove();
    QString numStr = QString::number( m_historyView->count()+1 ) + QLatin1String( ". " );
    m_historyView->addItem( numStr + moveToString(move) );
    QListWidgetItem *last = m_historyView->item( m_historyView->count() - 1 );
    m_historyView->setCurrentItem( last );
    m_historyView->scrollToItem( last );

    statusBar()->changeItem( m_game->isComputersTurn() ? opponentName() : i18n("Your turn."), 0 );

    updateScores();
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

        updateScores();

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

void KReversiMainWindow::showEvent( QShowEvent* )
{
    if ( m_firstShow && m_startInDemoMode )
    {
        kDebug() << "starting demo...";
        slotToggleDemoMode();
    }
    m_firstShow = false;
}

void KReversiMainWindow::updateScores()
{
    statusBar()->changeItem( i18n("You: %1", m_game->playerScore(Black) ), PLAYER_STATUSBAR_ID);
    statusBar()->changeItem( i18n("%1: %2", opponentName(), m_game->playerScore(White) ), COMP_STATUSBAR_ID);
}

QString KReversiMainWindow::opponentName() const
{
    return i18n("Computer");
}

#include "mainwindow.moc"
