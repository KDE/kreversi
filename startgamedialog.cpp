#include "startgamedialog.h"
#include "ui_startgamedialog.h"

#include <QMessageBox>
#include <QCloseEvent>

StartGameDialog::StartGameDialog(QWidget *parent) :
    KDialog(parent),
    ui(new Ui::StartGameDialog)
{
    setModal(true);

    setFixedSize(width(), height());

    setButtons(Ok | Close);
    setButtonText(Ok, "Start game");
    setButtonToolTip(Ok, "Let's start playing!");
    setButtonText(Close, "Quit");
    setButtonToolTip(Close, "Quit KReversi");

    m_contents = new QWidget(this);
    setMainWidget(m_contents);
    ui->setupUi(m_contents);

    ui->whiteTypeGroup->setId(ui->whiteHuman, GameStartInformation::Human);
    ui->whiteTypeGroup->setId(ui->whiteAI, GameStartInformation::AI);

    ui->blackTypeGroup->setId(ui->blackHuman, GameStartInformation::Human);
    ui->blackTypeGroup->setId(ui->blackAI, GameStartInformation::AI);

    connect(ui->blackTypeGroup, SIGNAL(buttonClicked(int)), this, SLOT(slotUpdateBlack(int)));
    connect(ui->whiteTypeGroup, SIGNAL(buttonClicked(int)), this, SLOT(slotUpdateWhite(int)));

    slotUpdateBlack(GameStartInformation::Human);
    slotUpdateWhite(GameStartInformation::AI);
}

StartGameDialog::~StartGameDialog()
{
    delete ui;
}

void StartGameDialog::slotButtonClicked(int button)
{
    if (button == KDialog::Ok)
        emit startGame();
    KDialog::slotButtonClicked(button);
}

GameStartInformation StartGameDialog::createGameStartInformation() const
{
    GameStartInformation info;
    info.name[Black] = ui->blackName->text();
    info.name[White] = ui->whiteName->text();
    info.type[Black] = (GameStartInformation::PlayerType)ui->blackTypeGroup->checkedId();
    info.type[White] = (GameStartInformation::PlayerType)ui->whiteTypeGroup->checkedId();
    info.skill[Black] = ui->blackSkill->currentIndex();
    info.skill[White] = ui->whiteSkill->currentIndex();

    return info;
}


void StartGameDialog::slotUpdateBlack(int clickedId)
{
    ui->blackSkill->setEnabled(clickedId == GameStartInformation::AI);
    ui->blackName->setEnabled(clickedId == GameStartInformation::Human);
    if (clickedId == GameStartInformation::Human)
        ui->blackName->setText(m_user.loginName());
    else
        ui->blackName->setText("Computer");
}

void StartGameDialog::slotUpdateWhite(int clickedId)
{
    ui->whiteSkill->setEnabled(clickedId == GameStartInformation::AI);
    ui->whiteName->setEnabled(clickedId == GameStartInformation::Human);
    if (clickedId == GameStartInformation::Human)
        ui->whiteName->setText(m_user.loginName());
    else
        ui->whiteName->setText("Computer");
}
