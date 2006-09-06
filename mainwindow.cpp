#include "mainwindow.h"
#include "kreversigame.h"
#include "kreversiscene.h"
#include "kreversiview.h"
#include "preferences.h"

#include <kaction.h>
#include <ktoggleaction.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kicon.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <kstdaction.h>
#include <kselectaction.h>

#include <QGraphicsView>
#include <QListWidget>
#include <QGridLayout>

static QString moveToString( const KReversiMove& move )
{
    QString moveString;
    if( Preferences::useColoredChips() )
        moveString = (move.color == Black ? i18n("Blue") : i18n("Red") );
    else
        moveString = (move.color == Black ? i18n("Black") : i18n("White") );

    const char labelsHor[] = "ABCDEFGH";
    const char labelsVer[] = "12345678";

    moveString += " ";
    moveString += labelsHor[move.col];
    moveString += labelsVer[move.row];

    return moveString;
}

KReversiMainWindow::KReversiMainWindow(QWidget* parent)
    : KMainWindow(parent), m_scene(0), m_game(0), m_undoAct(0), m_hintAct(0), m_demoAct(0)
{
    statusBar()->insertItem( i18n("Your turn."), 0 );

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

    m_bkgndAct = new KSelectAction(i18n("Choose background"), actionCollection(), "choose_bkgnd");
    connect(m_bkgndAct, SIGNAL(triggered(const QString&)), SLOT(slotBackgroundChanged(const QString&)));

    QStringList pixList = kapp->dirs()->findAllResources( "appdata", "pics/background/*.png", false, true );
    // let's find a name of files w/o extensions
    // FIXME dimsuz: this wont work with Windows separators...
    // But let's fix problems as they come (maybe will be some generalized solution in future)
    foreach( QString str, pixList )
    {
        int idx1 = str.lastIndexOf('/');
        int idx2 = str.lastIndexOf('.');
        m_bkgndAct->addAction(str.mid(idx1+1,idx2-idx1-1));
    }

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
    // FIXME dimsuz: this utilises 5 skills. although 7 is possible
    // also give them good names
    acts << i18n("Very easy") << i18n("Easy") << i18n("Normal");
    acts << i18n("Hard") << i18n("Beatable") << i18n("Unbeatable");
    m_skillAct->setItems(acts);
    connect(m_skillAct, SIGNAL(triggered(int)), SLOT(slotSkillChanged(int)) );

    m_coloredChipsAct = new KToggleAction( i18n("Use colored chips"), actionCollection(), "use_colored_chips" );
    connect( m_coloredChipsAct, SIGNAL(triggered(bool)), SLOT(slotUseColoredChips(bool)) );

    // FIXME dimsuz: read/write this from/to config file? Or not necessary?
    KToggleAction *showMovesAct = new KToggleAction( i18n("Show moves history"), actionCollection(), "show_moves" );
    connect( showMovesAct, SIGNAL(triggered(bool)), SLOT(slotShowMovesHistory(bool)) );

    addAction(newGameAct);
    addAction(quitAct);
    addAction(m_undoAct);
    addAction(m_hintAct);
    addAction(m_demoAct);
}

void KReversiMainWindow::loadSettings()
{
    m_bkgndAct->setCurrentAction( Preferences::backgroundImageName() );
    slotBackgroundChanged( Preferences::backgroundImageName() );

    int skill = Preferences::skill();
    m_skillAct->setCurrentItem( skill - 1 );
    slotSkillChanged( skill - 1 );

    m_animSpeedAct->setCurrentItem( Preferences::animationSpeed() );
    slotAnimSpeedChanged( Preferences::animationSpeed() );

    m_coloredChipsAct->setChecked( Preferences::useColoredChips() );
    slotUseColoredChips( Preferences::useColoredChips() );
}

void KReversiMainWindow::slotBackgroundChanged( const QString& text )
{
    // FIXME dimsuz: I'm removing "&" from text here, because
    // there's a problem in KSelectAction ATM - text will contain a menu accell-ampersands
    // remove that textMod and use text, after this issue will be fixed in kdelibs
    QString textMod = text;
    textMod.remove('&');
    QString file = textMod + ".png";
    QPixmap pix( KStandardDirs::locate("appdata", QString("pics/background/") + file ) );
    if(!pix.isNull())
    {
        m_view->resetCachedContent();
        m_scene->setBackgroundPixmap( pix );
    }
    Preferences::setBackgroundImageName( textMod );
    Preferences::writeConfig();
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
    QString chipsPngPath = m_coloredChipsAct->isChecked() ? "pics/chips.png" : "pics/chips_mono.png";
    m_scene->setChipsPixmap( KStandardDirs::locate("appdata", chipsPngPath) );
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

    if(m_hintAct)
        m_hintAct->setEnabled( true );
    if(m_demoAct)
        m_demoAct->setChecked( false );

    if(m_scene == 0) // if called first time
    {
        // FIXME dimsuz: if chips.png not found give error end exit
        m_scene = new KReversiScene(m_game, KStandardDirs::locate("appdata", "pics/chips.png"));
        connect( m_scene, SIGNAL(gameOver()), SLOT(slotGameOver()) );
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
    QString res =  blackScore > whiteScore ? i18n("You win!") : i18n("You have lost!");
    res += i18n("\nYou: %1", blackScore);
    res += i18n("\nComputer: %1", whiteScore);

    KMessageBox::information( this, res, i18n("Game over") );
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

#include "mainwindow.moc"
