#ifndef KREVERSI_MAIN_WINDOW_H
#define KREVERSI_MAIN_WINDOW_H

#include <kmainwindow.h>

class KReversiScene;
class KReversiGame;
class KReversiView;
class KAction;

class KReversiMainWindow : public KMainWindow
{
    Q_OBJECT
public:
    KReversiMainWindow(QWidget* parent=0);
public slots:
    void slotNewGame();
    void slotBackgroundChanged(const QString& text);
    void slotUndo();
    void slotMoveFinished();
    void slotGameOver();
private:
    void setupActions();

    KReversiScene *m_scene;
    KReversiView  *m_view;
    KReversiGame *m_game;

    KAction* m_undoAct;
    KAction* m_hintAct;
    KAction* m_demoAct;
};
#endif
