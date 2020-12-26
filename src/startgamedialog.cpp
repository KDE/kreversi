/*
    SPDX-FileCopyrightText: 2013 Denis Kuplyakov <dener.kup@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "startgamedialog.h"
#include "ui_startgamedialog.h"

#include <QGraphicsDropShadowEffect>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QPainter>
#include <QSvgRenderer>
#include <QVBoxLayout>

#include <KgDifficulty>
#include <KLocalizedString>

StartGameDialog::StartGameDialog(QWidget *parent, KgThemeProvider *provider) :
    QDialog(parent),
    ui(new Ui::StartGameDialog), m_provider(provider), m_chipsPrefix(BlackWhite)
{
    setModal(true);

    setWindowTitle(i18n("New game"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Close);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &StartGameDialog::slotAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &StartGameDialog::reject);
    okButton->setText(i18n("Start game"));
    okButton->setToolTip(i18n("Let's start playing!"));
    buttonBox->button(QDialogButtonBox::Close)->setText(i18n("Quit"));
    buttonBox->button(QDialogButtonBox::Close)->setToolTip(i18n("Quit KReversi"));

    m_contents = new QWidget(this);
    mainLayout->addWidget(m_contents);
    mainLayout->addWidget(buttonBox);
    ui->setupUi(m_contents);

    loadChipImages();

    ui->whiteTypeGroup->setId(ui->whiteHuman, GameStartInformation::Human);
    ui->whiteTypeGroup->setId(ui->whiteAI, GameStartInformation::AI);
    ui->whiteAI->setIcon(QIcon::fromTheme(QStringLiteral("computer")));
    ui->whiteHuman->setIcon(QIcon::fromTheme(QStringLiteral("user-identity")));

    ui->blackTypeGroup->setId(ui->blackHuman, GameStartInformation::Human);
    ui->blackTypeGroup->setId(ui->blackAI, GameStartInformation::AI);
    ui->blackAI->setIcon(QIcon::fromTheme(QStringLiteral("computer")));
    ui->blackHuman->setIcon(QIcon::fromTheme(QStringLiteral("user-identity")));

    QList< const KgDifficultyLevel * > diffList = Kg::difficulty()->levels();
    const QIcon icon = QIcon::fromTheme(QStringLiteral("games-difficult"));

    for (int i = 0; i < diffList.size(); ++i) {
        ui->blackSkill->addItem(icon, diffList.at(i)->title());
        ui->whiteSkill->addItem(icon, diffList.at(i)->title());
        if (diffList.at(i)->isDefault()) {
            ui->whiteSkill->setCurrentIndex(i);
            ui->blackSkill->setCurrentIndex(i);
        }
    }

    connect(ui->blackTypeGroup, static_cast<void (QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), this, &StartGameDialog::slotUpdateBlack);
    connect(ui->whiteTypeGroup, static_cast<void (QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), this, &StartGameDialog::slotUpdateWhite);

    slotUpdateBlack(ui->blackTypeGroup->button(GameStartInformation::Human));
    slotUpdateWhite(ui->whiteTypeGroup->button(GameStartInformation::AI));
}

StartGameDialog::~StartGameDialog()
{
    delete ui;
}


void StartGameDialog::loadChipImages()
{
    QSvgRenderer svgRenderer;
    svgRenderer.load(m_provider->currentTheme()->graphicsPath());

    QPixmap blackChip(QSize(46, 46));
    blackChip.fill(Qt::transparent);
    QPixmap whiteChip(QSize(46, 46));
    whiteChip.fill(Qt::transparent);

    QPainter *painter = new QPainter(&blackChip);
    QString prefix = Utils::chipPrefixToString(m_chipsPrefix);
    svgRenderer.render(painter, prefix + QStringLiteral("_1"));
    delete painter;

    painter = new QPainter(&whiteChip);
    // TODO: get 12 from some global constant that is shared with QML
    svgRenderer.render(painter, prefix + QStringLiteral("_12"));
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
}

void StartGameDialog::slotAccepted()
{
    Q_EMIT startGame();
    accept();
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

void StartGameDialog::setChipsPrefix(ChipsPrefix prefix)
{
    m_chipsPrefix = prefix;
    loadChipImages();
}


void StartGameDialog::slotUpdateBlack(QAbstractButton *button)
{
    if (button) {
        const int clickedId = ui->blackTypeGroup->id(button);
        ui->blackSkill->setEnabled(clickedId == GameStartInformation::AI);
        ui->blackName->setEnabled(clickedId == GameStartInformation::Human);
        if (clickedId == GameStartInformation::Human)
            ui->blackName->setText(m_user.loginName());
        else
            ui->blackName->setText(i18n("Computer"));
    }
}

void StartGameDialog::slotUpdateWhite(QAbstractButton *button)
{
    if (button) {
        const int clickedId = ui->whiteTypeGroup->id(button);

        ui->whiteSkill->setEnabled(clickedId == GameStartInformation::AI);
        ui->whiteName->setEnabled(clickedId == GameStartInformation::Human);
        if (clickedId == GameStartInformation::Human)
            ui->whiteName->setText(m_user.loginName());
        else
            ui->whiteName->setText(i18n("Computer"));
    }
}
