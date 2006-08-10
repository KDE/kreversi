#include "mainwindow.h"
#include "kreversigame.h"
#include "kreversiscene.h"
#include "kreversiview.h"

#include <kaction.h>
#include <kstdaction.h>

#include <QGraphicsView>

KReversiMainWindow::KReversiMainWindow(QWidget* parent)
    : KMainWindow(parent), m_game(0)
{
    setupActions();
    m_scene = new KReversiScene;
    KReversiView *view = new KReversiView(m_scene, this);
    view->show();
    setCentralWidget(view);
    newGame();
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
