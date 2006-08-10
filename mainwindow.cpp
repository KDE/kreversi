#include "mainwindow.h"
#include "kreversigame.h"
#include "kreversiscene.h"

#include <kaction.h>
#include <kstdaction.h>

#include <QGraphicsView>

KReversiMainWindow::KReversiMainWindow(QWidget* parent)
    : KMainWindow(parent)
{
    setupActions();
    m_scene = new KReversiScene;
    QGraphicsView *view = new QGraphicsView(m_scene, this);
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
    KReversiGame *game = new KReversiGame(10);
    m_scene->setGame( game );
}

#include "mainwindow.moc"
