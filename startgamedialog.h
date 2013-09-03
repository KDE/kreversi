#ifndef STARTGAMEDIALOG_H
#define STARTGAMEDIALOG_H

#include <KDialog>
#include "gamestartinformation.h"
#include "commondefs.h"
#include <KUser>

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

private slots:
    void slotUpdateBlack(int clickedId);
    void slotUpdateWhite(int clickedId);

private:
    void slotButtonClicked(int button);
    Ui::StartGameDialog *ui;
    QWidget *m_contents;
    KUser m_user;
};

#endif // STARTGAMEDIALOG_H
