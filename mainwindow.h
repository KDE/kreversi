#ifndef KREVERSI_MAIN_WINDOW_H
#define KREVERSI_MAIN_WINDOW_H

#include <kmainwindow.h>

class KReversiScene;
class KReversiGame;
class KReversiView;

class KReversiMainWindow : public KMainWindow
{
    Q_OBJECT
public:
    KReversiMainWindow(QWidget* parent=0);
public slots:
    void newGame();
    void slotBackgroundChanged(const QString& text);
private:
    void setupActions();

    KReversiScene *m_scene;
    KReversiView  *m_view;
    KReversiGame *m_game;
};
#endif
