#include "mainwindow.h"
#include "kreversigame.h"
#include "kreversiscene.h"
#include "kreversiview.h"

#include <kaction.h>
#include <kstdaction.h>

#include <QGraphicsView>

KReversiMainWindow::KReversiMainWindow(QWidget* parent)
    : KMainWindow(parent)
{
    setupActions();
    m_game = new KReversiGame;
    m_scene = new KReversiScene(m_game);
    KReversiView *view = new KReversiView(m_scene, this);
    view->show();
    setCentralWidget(view);
    setupGUI();
}

void KReversiMainWindow::setupActions()
{
    KAction *quitAct = KStdAction::quit(this, SLOT(close()), actionCollection(), "quit");
    KAction *newGameAct = KStdAction::openNew(this, SLOT(newGame()), actionCollection(), "new_game");
    addAction(newGameAct);
    addAction(quitAct);
}

void KReversiMainWindow::newGame()
{
    delete m_game;
    m_game = new KReversiGame;
    m_scene->setGame( m_game );
}

#include "mainwindow.moc"
