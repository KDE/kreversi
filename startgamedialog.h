#ifndef STARTGAMEDIALOG_H
#define STARTGAMEDIALOG_H

#include <KDialog>
#include "gamestartinformation.h"
#include "commondefs.h"

namespace Ui {
class StartGameDialog;
}

class StartGameDialog : public KDialog
{
    Q_OBJECT

public:
    explicit StartGameDialog(QWidget *parent = 0);
    ~StartGameDialog();

    GameStartInformation createGameStartInformation() const;

signals:
    void startGame();

private:
    Ui::StartGameDialog *ui;
    QWidget *m_contents;
    void slotButtonClicked(int button);

};

#endif // STARTGAMEDIALOG_H
