#include "startgamedialog.h"
#include "ui_startgamedialog.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <KgDifficulty>
#include <kdebug.h>
#include <QSvgRenderer>
#include <QPainter>
#include <KColorScheme>

StartGameDialog::StartGameDialog(QWidget *parent, KgThemeProvider *provider) :
    KDialog(parent),
    ui(new Ui::StartGameDialog), m_provider(provider)
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

    QSvgRenderer svgRenderer;
    svgRenderer.load(m_provider->currentTheme()->graphicsPath());

    QPixmap blackChip(QSize(46, 46));
    blackChip.fill(Qt::transparent);
    QPixmap whiteChip(QSize(46, 46));
    whiteChip.fill(Qt::transparent);

    QPainter *painter = new QPainter(&blackChip);
    svgRenderer.render(painter, "chip_bw_1");
    delete painter;

    painter = new QPainter(&whiteChip);
    svgRenderer.render(painter, "chip_bw_12");
    delete painter;

    ui->blackLabel->setPixmap(blackChip);
    ui->whiteLabel->setPixmap(whiteChip);

    QGraphicsDropShadowEffect *blackShadow = new QGraphicsDropShadowEffect(this);
    blackShadow->setBlurRadius(10.0);
    blackShadow->setColor(Qt::black);
    blackShadow->setOffset(0.0);

    QGraphicsDropShadowEffect *whiteShadow = new QGraphicsDropShadowEffect(this);
    whiteShadow->setBlurRadius(10.0);
    whiteShadow->setColor(Qt::black);
    whiteShadow->setOffset(0.0);

    ui->blackLabel->setGraphicsEffect(blackShadow);
    ui->whiteLabel->setGraphicsEffect(whiteShadow);

    ui->whiteTypeGroup->setId(ui->whiteHuman, GameStartInformation::Human);
    ui->whiteTypeGroup->setId(ui->whiteAI, GameStartInformation::AI);

    ui->blackTypeGroup->setId(ui->blackHuman, GameStartInformation::Human);
    ui->blackTypeGroup->setId(ui->blackAI, GameStartInformation::AI);

    QList< const KgDifficultyLevel * > diffList = Kg::difficulty()->levels();
    const KIcon icon("games-difficult");

    for (int i = 0; i < diffList.size(); i++) {
        ui->blackSkill->addItem(icon, diffList.at(i)->title());
        ui->whiteSkill->addItem(icon, diffList.at(i)->title());
        if (diffList.at(i)->isDefault())
        {
            ui->whiteSkill->setCurrentIndex(i);
            ui->blackSkill->setCurrentIndex(i);
        }
    }

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
