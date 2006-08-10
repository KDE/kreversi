#include "mainwindow.h"

#include <kstdaction.h>

KReversiMainWindow::KReversiMainWindow(QWidget* parent)
    : KMainWindow(parent)
{
    setupActions();
    setupGUI();
}

void KReversiMainWindow::setupActions()
{
    KStdAction::quit(this, SLOT(close()), actionCollection());
}
