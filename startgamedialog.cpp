/*******************************************************************
 *
 * Copyright 2013 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * This file is part of the KDE project "KReversi"
 *
 * KReversi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * KReversi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KReversi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 ********************************************************************/

#include <startgamedialog.h>
#include <ui_startgamedialog.h>

#include <QMessageBox>
#include <QCloseEvent>
#include <KgDifficulty>

#include <QSvgRenderer>
#include <KLocalizedString>
#include <QPainter>

#include <QGraphicsDropShadowEffect>

StartGameDialog::StartGameDialog(QWidget *parent, KgThemeProvider *provider) :
    KDialog(parent),
    ui(new Ui::StartGameDialog), m_provider(provider), m_chipsPrefix(BlackWhite)
{
    setModal(true);

    setFixedSize(width(), height());

    setCaption(i18n("New game"));

    setButtons(Ok | Close);
    setButtonText(Ok, i18n("Start game"));
    setButtonToolTip(Ok, i18n("Let's start playing!"));
    setButtonText(Close, i18n("Quit"));
    setButtonToolTip(Close, i18n("Quit KReversi"));

    m_contents = new QWidget(this);
    setMainWidget(m_contents);
    ui->setupUi(m_contents);

    loadChipImages();

    ui->whiteTypeGroup->setId(ui->whiteHuman, GameStartInformation::Human);
    ui->whiteTypeGroup->setId(ui->whiteAI, GameStartInformation::AI);
    ui->whiteAI->setIcon(QIcon::fromTheme("computer"));
    ui->whiteHuman->setIcon(QIcon::fromTheme("user-identity"));

    ui->blackTypeGroup->setId(ui->blackHuman, GameStartInformation::Human);
    ui->blackTypeGroup->setId(ui->blackAI, GameStartInformation::AI);
    ui->blackAI->setIcon(QIcon::fromTheme("computer"));
    ui->blackHuman->setIcon(QIcon::fromTheme("user-identity"));

    QList< const KgDifficultyLevel * > diffList = Kg::difficulty()->levels();
    const QIcon icon = QIcon::fromTheme("games-difficult");

    for (int i = 0; i < diffList.size(); i++) {
        ui->blackSkill->addItem(icon, diffList.at(i)->title());
        ui->whiteSkill->addItem(icon, diffList.at(i)->title());
        if (diffList.at(i)->isDefault()) {
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
    svgRenderer.render(painter, prefix + "_1");
    delete painter;

    painter = new QPainter(&whiteChip);
    // TODO: get 12 from some global constant that is shared with QML
    svgRenderer.render(painter, prefix + "_12");
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

void StartGameDialog::setChipsPrefix(ChipsPrefix prefix)
{
    m_chipsPrefix = prefix;
    loadChipImages();
}


void StartGameDialog::slotUpdateBlack(int clickedId)
{
    ui->blackSkill->setEnabled(clickedId == GameStartInformation::AI);
    ui->blackName->setEnabled(clickedId == GameStartInformation::Human);
    if (clickedId == GameStartInformation::Human)
        ui->blackName->setText(m_user.loginName());
    else
        ui->blackName->setText(i18n("Computer"));
}

void StartGameDialog::slotUpdateWhite(int clickedId)
{
    ui->whiteSkill->setEnabled(clickedId == GameStartInformation::AI);
    ui->whiteName->setEnabled(clickedId == GameStartInformation::Human);
    if (clickedId == GameStartInformation::Human)
        ui->whiteName->setText(m_user.loginName());
    else
        ui->whiteName->setText(i18n("Computer"));
}
