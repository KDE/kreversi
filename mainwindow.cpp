#include "mainwindow.h"
#include "kreversigame.h"
#include "kreversiscene.h"
#include "kreversiview.h"

#include <kaction.h>
#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kstdaction.h>
#include <kselectaction.h>
#include <kapplication.h>

#include <QGraphicsView>

KReversiMainWindow::KReversiMainWindow(QWidget* parent)
    : KMainWindow(parent)
{
    m_game = new KReversiGame;

    // FIXME dimsuz: if chips.png not found give error end exit
    m_scene = new KReversiScene(m_game, KStandardDirs::locate("appdata", "pics/chips.png"));

    m_view = new KReversiView(m_scene, this);
    m_view->show();

    setupActions();
    setCentralWidget(m_view);
    setupGUI();
}

void KReversiMainWindow::setupActions()
{
    KAction *quitAct = KStdAction::quit(this, SLOT(close()), actionCollection(), "quit");
    KAction *newGameAct = KStdAction::openNew(this, SLOT(newGame()), actionCollection(), "new_game");

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

    // FIXME dimsuz: this should come from KConfig!
    bkgndAct->setCurrentAction( "Hexagon" );
    slotBackgroundChanged("Hexagon");

    addAction(newGameAct);
    addAction(quitAct);
}

void KReversiMainWindow::slotBackgroundChanged( const QString& text )
{
    // FIXME dimsuz: I'm removing "&" from text here, because
    // there's a problem in KSelectAction ATM - text will contain a menu accell-ampersands
    // remove that .remove, after it is fixed
    QString file = text + ".png";
    file.remove('&');
    QPixmap pix( KStandardDirs::locate("appdata", QString("pics/background/") + file ) );
    if(!pix.isNull())
    {
        m_view->resetCachedContent();
        m_scene->setBackgroundPixmap( pix );
    }
}

void KReversiMainWindow::newGame()
{
    delete m_game;
    m_game = new KReversiGame;
    m_scene->setGame( m_game );
}

#include "mainwindow.moc"
