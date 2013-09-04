#ifndef STARTGAMEDIALOG_H
#define STARTGAMEDIALOG_H

#include <KDialog>
#include "gamestartinformation.h"
#include "commondefs.h"
#include <KUser>
#include <KgThemeProvider>

namespace Ui {
class StartGameDialog;
}

class StartGameDialog : public KDialog
{
    Q_OBJECT

public:
    explicit StartGameDialog(QWidget *parent, KgThemeProvider *provider);
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
    KgThemeProvider *m_provider;
};

#endif // STARTGAMEDIALOG_H
