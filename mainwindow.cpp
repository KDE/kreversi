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
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <kstdaction.h>
#include <kselectaction.h>

#include <QGraphicsView>

KReversiMainWindow::KReversiMainWindow(QWidget* parent)
    : KMainWindow(parent), m_scene(0), m_game(0), m_undoAct(0), m_hintAct(0), m_demoAct(0)
{
    slotNewGame();
    // m_scene is created in slotNewGame();

    m_view = new KReversiView(m_scene, this);
    m_view->show();

    setupActions();
    setCentralWidget(m_view);

    statusBar()->insertItem( i18n("Your turn."), 0 );
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

    KSelectAction *bkgndAct = new KSelectAction(i18n("Choose background"), actionCollection(), "choose_bkgnd");
    connect(bkgndAct, SIGNAL(triggered(const QString&)), SLOT(slotBackgroundChanged(const QString&)));

    QStringList pixList = kapp->dirs()->findAllResources( "appdata", "pics/background/*.png", false, true );
    // let's find a name of files w/o extensions
    // FIXME dimsuz: this wont work with Windows separators...
    // But let's fix problems as they come (maybe will be some generalized solution in future)
    foreach( QString str, pixList )
    {
        int idx1 = str.lastIndexOf('/');
        int idx2 = str.lastIndexOf('.');
        bkgndAct->addAction(str.mid(idx1+1,idx2-idx1-1));
    }


    bkgndAct->setCurrentAction( Preferences::backgroundImageName() );
    slotBackgroundChanged( Preferences::backgroundImageName() );

    KToggleAction *showLast = new KToggleAction(KIcon("lastmoves"), i18n("Show last move"), actionCollection(), "show_last_move");
    connect( showLast, SIGNAL(triggered(bool)), m_scene, SLOT(setShowLastMove(bool)) );

    KToggleAction *showLegal = new KToggleAction(KIcon("legalmoves"), i18n("Show legal moves"), actionCollection(), "show_legal_moves" );
    connect( showLegal, SIGNAL(triggered(bool)), m_scene, SLOT(setShowLegalMoves(bool)) );

    addAction(newGameAct);
    addAction(quitAct);
    addAction(m_undoAct);
    addAction(m_hintAct);
    addAction(m_demoAct);
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

void KReversiMainWindow::slotDemoMode(bool toggled)
{
    kDebug() << k_funcinfo << endl;
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

    // FIXME dimsuz: make this nicer
    QString res = i18n("GAME OVER. ");
    res += m_game->playerScore(Black) > m_game->playerScore(White) ? i18n("You won.") : i18n("Computer won.");
    statusBar()->changeItem( res, 0 );
}

void KReversiMainWindow::slotMoveFinished()
{
    if( !m_demoAct->isChecked() )
        m_undoAct->setEnabled( m_game->canUndo() );    

    statusBar()->changeItem( m_game->isComputersTurn() ? i18n("Computer turn.") : i18n("Your turn."), 0 );
}

void KReversiMainWindow::slotUndo()
{
    if( !m_scene->isBusy() )
    {
        // scene will automatically notice that it needs to update
        m_game->undo();
        m_undoAct->setEnabled( m_game->canUndo() );    
        // if the user hits undo after game is over
        // let's give him a chance to ask for a hint ;)
        m_hintAct->setEnabled( true );
    }
}

#include "mainwindow.moc"
