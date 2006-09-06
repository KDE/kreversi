#ifndef KREVERSI_MAIN_WINDOW_H
#define KREVERSI_MAIN_WINDOW_H

#include <kmainwindow.h>

class KReversiScene;
class KReversiGame;
class KReversiView;
class KAction;
class KSelectAction;
class KToggleAction;
class QListWidget;
class QLabel;

class KReversiMainWindow : public KMainWindow
{
    Q_OBJECT
public:
    KReversiMainWindow(QWidget* parent=0);
public slots:
    void slotNewGame();
    void slotBackgroundChanged(const QString& text);
    void slotSkillChanged(int);
    void slotAnimSpeedChanged(int);
    void slotUndo();
    void slotMoveFinished();
    void slotGameOver();
    void slotDemoMode(bool);
    void slotUseColoredChips(bool);
    void slotShowMovesHistory(bool);
private:
    void setupActions();
    void loadSettings();

    KReversiScene *m_scene;
    KReversiView  *m_view;
    KReversiGame  *m_game;
    QListWidget   *m_historyView;
    QLabel        *m_historyLabel;

    KAction* m_undoAct;
    KAction* m_hintAct;
    KAction* m_demoAct;
    KSelectAction* m_animSpeedAct;
    KSelectAction* m_bkgndAct;
    KSelectAction* m_skillAct;
    KToggleAction* m_coloredChipsAct;
};
#endif
