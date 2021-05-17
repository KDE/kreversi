/*
    SPDX-FileCopyrightText: 2006 Dmitry Suzdalev <dimsuz@gmail.com>
    SPDX-FileCopyrightText: 2010 Brian Croom <brian.s.croom@gmail.com>
    SPDX-FileCopyrightText: 2013 Denis Kuplyakov <dener.kup@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "mainwindow.h"

#include <QDebug>
#include <QDesktopWidget>
#include <QIcon>
#include <QStatusBar>
#include <QApplication>
#include <QScreen>
// KF
#include <kwidgetsaddons_version.h>
#include <KActionCollection>
#include <KLocalizedString>
#include <KMessageBox>
#include <KStandardGameAction>

#include "commondefs.h"
#include "kreversihumanplayer.h"
#include "kreversicomputerplayer.h"
#include "kexthighscore.h"

KReversiMainWindow::KReversiMainWindow(QWidget* parent, bool startDemo)
    : KXmlGuiWindow(parent),
    m_startDialog(nullptr),
    m_view(nullptr),
    m_game(nullptr),
    m_historyDock(nullptr),
    m_historyView(nullptr),
    m_firstShow(true),
    m_startInDemoMode(startDemo),
    m_undoAct(nullptr),
    m_hintAct(nullptr)
{
    memset(m_player, 0, sizeof(m_player));

    m_provider = new KgThemeProvider();
    m_provider->discoverThemes("appdata", QStringLiteral("pics"));

    for (auto &label : m_statusBarLabel) {
       label = new QLabel(this);
       label->setAlignment(Qt::AlignCenter);
       statusBar()->addWidget(label, 1);
    }
    m_statusBarLabel[common]->setText(i18n("Press start game!"));

    // initialize difficulty stuff
    Kg::difficulty()->addStandardLevelRange(
        KgDifficultyLevel::VeryEasy, KgDifficultyLevel::Impossible,
        KgDifficultyLevel::Easy //default
    );

    KgDifficultyGUI::init(this);
    connect(Kg::difficulty(), &KgDifficulty::currentLevelChanged, this, &KReversiMainWindow::levelChanged);
    Kg::difficulty()->setEditable(false);

    // initialize history dock
    m_historyView = new QListWidget(this);
    m_historyView->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    m_historyDock = new QDockWidget(i18n("Move History"));
    m_historyDock->setWidget(m_historyView);
    m_historyDock->setObjectName(QStringLiteral("history_dock"));

    m_historyDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_historyDock);

    // create main game view
    m_view = new KReversiView(m_game, this, m_provider);
    setCentralWidget(m_view);

    // initialise dialog handler
    m_startDialog = new StartGameDialog(this, m_provider);
    connect(m_startDialog, &StartGameDialog::startGame, this, &KReversiMainWindow::slotDialogReady);

    // initialise actions
    setupActionsInit();

    // load saved settings
    loadSettings();

    setupGUI(screen()->availableGeometry().size() * 0.7);
    m_historyDock->hide();
}

KReversiMainWindow::~KReversiMainWindow()
{
    clearPlayers();
    delete m_provider;
}

void KReversiMainWindow::setupActionsInit()
{
    // Common actions
    KStandardGameAction::gameNew(this, &KReversiMainWindow::slotNewGame, actionCollection());
    KStandardGameAction::highscores(this, &KReversiMainWindow::slotHighscores, actionCollection());
    KStandardGameAction::quit(this, &QWidget::close, actionCollection());

    // Undo
    m_undoAct = KStandardGameAction::undo(this, &KReversiMainWindow::slotUndo, actionCollection());
    m_undoAct->setEnabled(false);   // nothing to undo at the start of the game

    // Hint
    m_hintAct = KStandardGameAction::hint(m_view, &KReversiView::slotHint, actionCollection());
    m_hintAct->setEnabled(false);

    // Last move
    m_showLast = new KToggleAction(QIcon::fromTheme(QStringLiteral("lastmoves")), i18n("Show Last Move"), this);
    actionCollection()->addAction(QStringLiteral("show_last_move"), m_showLast);
    connect(m_showLast, &KToggleAction::triggered, m_view, &KReversiView::setShowLastMove);

    // Legal moves
    m_showLegal = new KToggleAction(QIcon::fromTheme(QStringLiteral("legalmoves")), i18n("Show Legal Moves"), this);
    actionCollection()->addAction(QStringLiteral("show_legal_moves"), m_showLegal);
    connect(m_showLegal, &KToggleAction::triggered, m_view, &KReversiView::setShowLegalMoves);

    // Animation speed
    m_animSpeedAct = new KSelectAction(i18n("Animation Speed"), this);
    actionCollection()->addAction(QStringLiteral("anim_speed"), m_animSpeedAct);

    QStringList acts;
    acts << i18n("Slow") << i18n("Normal") << i18n("Fast");
    m_animSpeedAct->setItems(acts);
    connect(m_animSpeedAct, &KSelectAction::indexTriggered, this, &KReversiMainWindow::slotAnimSpeedChanged);

    // Chip's color
    m_coloredChipsAct = new KToggleAction(i18n("Use Colored Chips"), this);
    actionCollection()->addAction(QStringLiteral("use_colored_chips"), m_coloredChipsAct);
    connect(m_coloredChipsAct, &KToggleAction::triggered, this, &KReversiMainWindow::slotUseColoredChips);

    // Move history
    // NOTE: read/write this from/to config file? Or not necessary?
    m_showMovesAct = m_historyDock->toggleViewAction();
    m_showMovesAct->setIcon(QIcon::fromTheme(QStringLiteral("view-history")));
    m_showMovesAct->setText(i18n("Show Move History"));
    actionCollection()->addAction(QStringLiteral("show_moves"), m_showMovesAct);
    connect(m_historyDock, &QDockWidget::visibilityChanged, this, &KReversiMainWindow::slotToggleBoardLabels);
}

void KReversiMainWindow::loadSettings()
{
    // Animation speed
    m_animSpeedAct->setCurrentItem(Preferences::animationSpeed());
    m_view->setAnimationSpeed(Preferences::animationSpeed());

    // Chip's color
    m_coloredChipsAct->setChecked(Preferences::useColoredChips());
    m_view->setChipsPrefix(Preferences::useColoredChips() ?
                           Colored : BlackWhite);
    m_startDialog->setChipsPrefix(Preferences::useColoredChips() ?
                                       Colored : BlackWhite);
}

void KReversiMainWindow::levelChanged()
{
    // we are assuming that level can be changed here only when it is
    // USER-AI or AI-USER match

    int skill = Utils::difficultyLevelToInt();

    if (m_nowPlayingInfo.type[White] == GameStartInformation::AI)
        ((KReversiComputerPlayer *)(m_player[White]))->setSkill(skill);
    else if (m_nowPlayingInfo.type[Black] == GameStartInformation::Human)
        ((KReversiComputerPlayer *)(m_player[Black]))->setSkill(skill);
}

void KReversiMainWindow::slotAnimSpeedChanged(int speed)
{
    m_view->setAnimationSpeed(speed);
    Preferences::setAnimationSpeed(speed);
    Preferences::self()->save();
}

void KReversiMainWindow::slotUseColoredChips(bool toggled)
{
    ChipsPrefix chipsPrefix = m_coloredChipsAct->isChecked() ?
                                            Colored :
                                            BlackWhite;
    m_view->setChipsPrefix(chipsPrefix);
    m_startDialog->setChipsPrefix(chipsPrefix);
    Preferences::setUseColoredChips(toggled);
    Preferences::self()->save();
}

void KReversiMainWindow::slotToggleBoardLabels(bool toggled)
{
    m_view->setShowBoardLabels(toggled);
}

void KReversiMainWindow::slotNewGame()
{
    m_startDialog->exec();
}

void KReversiMainWindow::slotGameOver()
{
    m_hintAct->setEnabled(false);
    m_undoAct->setEnabled(m_game->canUndo());

    int blackScore = m_game->playerScore(Black);
    int whiteScore = m_game->playerScore(White);

    bool storeScore = false;
    KExtHighscore::Score score;

    QString res;
    if (m_nowPlayingInfo.type[Black] == GameStartInformation::Human
            && m_nowPlayingInfo.type[White] == GameStartInformation::AI) { // we are playing black
        storeScore = true;
        KExtHighscore::setGameType(((KReversiComputerPlayer *)m_player[White])->lowestSkill());
        score.setScore(blackScore);
        if (blackScore == whiteScore) {
            res = i18n("Game is drawn!");
            score.setType(KExtHighscore::Draw);
        } else if (blackScore > whiteScore) {
            res = i18n("You win!");
            score.setType(KExtHighscore::Won);
        } else {
            res = i18n("You have lost!");
            score.setType(KExtHighscore::Lost);
        }
    } else if (m_nowPlayingInfo.type[White] == GameStartInformation::Human
               && m_nowPlayingInfo.type[Black] == GameStartInformation::AI) { // we are playing white
        storeScore = true;
        KExtHighscore::setGameType(((KReversiComputerPlayer *)m_player[Black])->lowestSkill());
        score.setScore(whiteScore);
        if (blackScore == whiteScore) {
            res = i18n("Game is drawn!");
            score.setType(KExtHighscore::Draw);
        } else if (blackScore < whiteScore) {
            res = i18n("You win!");
            score.setType(KExtHighscore::Won);
        } else {
            res = i18n("You have lost!");
            score.setType(KExtHighscore::Lost);
        }

    } else if (m_nowPlayingInfo.type[Black] == GameStartInformation::Human
               && m_nowPlayingInfo.type[White] == GameStartInformation::Human) { // friends match
        if (blackScore == whiteScore) {
            res = i18n("Game is drawn!");
        } else if (blackScore > whiteScore) {
            res = i18n("%1 has won!", m_nowPlayingInfo.name[Black]);
        } else {
            res = i18n("%1 has won!", m_nowPlayingInfo.name[White]);
        }
    } else { // using Black White names in other cases
        if (blackScore == whiteScore) {
            res = i18n("Game is drawn!");
        } else if (blackScore > whiteScore) {
            res = i18n("%1 has won!", Utils::colorToString(Black));
        } else {
            res = i18n("%1 has won!", Utils::colorToString(White));
        }
    }

    if (m_nowPlayingInfo.type[Black] == GameStartInformation::AI
            && m_nowPlayingInfo.type[White] == GameStartInformation::AI) {
        res += i18n("\n%1: %2", Utils::colorToString(Black), blackScore);
        res += i18n("\n%1: %2", Utils::colorToString(White), whiteScore);
    } else {
        res += i18n("\n%1: %2", m_nowPlayingInfo.name[Black], blackScore);
        res += i18n("\n%1: %2", m_nowPlayingInfo.name[White], whiteScore);
    }

    KMessageBox::information(this, res, i18n("Game over"));

    if (storeScore)
        KExtHighscore::submitScore(score, this);
}

void KReversiMainWindow::slotMoveFinished()
{
    updateHistory();
    updateStatusBar();

    m_hintAct->setEnabled(m_game->isHintAllowed());
    m_undoAct->setEnabled(m_game->canUndo());
}

void KReversiMainWindow::updateHistory()
{
    MoveList history = m_game->getHistory();
    m_historyView->clear();

    for (int i = 0; i < history.size(); i++) {
        QString numStr = QString::number(i + 1) + QStringLiteral(". ");
        m_historyView->addItem(numStr + Utils::moveToString(history.at(i)));
    }

    QListWidgetItem *last = m_historyView->item(m_historyView->count() - 1);
    m_historyView->setCurrentItem(last);
    m_historyView->scrollToItem(last);
}

void KReversiMainWindow::slotUndo()
{
    // scene will automatically notice that it needs to update
    m_game->undo();

    updateHistory();
    updateStatusBar();

    m_undoAct->setEnabled(m_game->canUndo());
    m_hintAct->setEnabled(m_game->isHintAllowed());
}

void KReversiMainWindow::slotHighscores()
{
    KExtHighscore::show(this);
}

void KReversiMainWindow::slotDialogReady()
{
    GameStartInformation info = m_startDialog->createGameStartInformation();
    receivedGameStartInformation(info);
}

void KReversiMainWindow::showEvent(QShowEvent*)
{
    if (m_firstShow && m_startInDemoMode) {
        qDebug() << "starting demo...";
        startDemo();
    } else if (m_firstShow) {
        QTimer::singleShot(0, this, &KReversiMainWindow::slotNewGame);
    }
    m_firstShow = false;
}

void KReversiMainWindow::updateStatusBar()
{
    if (m_game->isGameOver()) {
        m_statusBarLabel[common]->setText(i18n("GAME OVER"));
    }

    if (m_nowPlayingInfo.type[Black] == GameStartInformation::AI
            && m_nowPlayingInfo.type[White] == GameStartInformation::AI) { // using Black White names
        m_statusBarLabel[black]->setText(i18n("%1: %2", Utils::colorToString(Black), m_game->playerScore(Black)));
        m_statusBarLabel[white]->setText(i18n("%1: %2", Utils::colorToString(White), m_game->playerScore(White)));

        if (!m_game->isGameOver()) {
            m_statusBarLabel[common]->setText(i18n("%1 turn", Utils::colorToString(m_game->currentPlayer())));
        }
    } else { // using player's names
        m_statusBarLabel[black]->setText(i18n("%1: %2", m_nowPlayingInfo.name[Black], m_game->playerScore(Black)));
        m_statusBarLabel[white]->setText(i18n("%1: %2", m_nowPlayingInfo.name[White], m_game->playerScore(White)));

        if (!m_game->isGameOver() && m_game->currentPlayer() != NoColor) {
            m_statusBarLabel[common]->setText(i18n("%1's turn", m_nowPlayingInfo.name[m_game->currentPlayer()]));
        }
    }
}


// TODO: test it!!!
void KReversiMainWindow::startDemo()
{
    GameStartInformation info;
    info.name[0] = info.name[1] = i18n("Computer");
    info.type[0] = info.type[1] = GameStartInformation::AI;
    info.skill[0] = info.skill[1] = Utils::difficultyLevelToInt();

    receivedGameStartInformation(info);
}

void KReversiMainWindow::clearPlayers()
{
    for (int i = 0; i < 2; i++) // iterating through white to black
        if (m_player[i]) {
            m_player[i]->disconnect();
            delete m_player[i];
            m_player[i] = nullptr;
        }
}

void KReversiMainWindow::receivedGameStartInformation(const GameStartInformation &info)
{
    clearPlayers();
    m_nowPlayingInfo = info;

    for (int i = 0; i < 2; i++) // iterating through black and white
        if (info.type[i] == GameStartInformation::AI) {
            m_player[i] = new KReversiComputerPlayer(ChipColor(i), info.name[i]);
            ((KReversiComputerPlayer *)(m_player[i]))->setSkill(info.skill[i]);
            levelChanged();
        } else {
            m_player[i] = new KReversiHumanPlayer(ChipColor(i), info.name[i]);
        }

    m_game = new KReversiGame(m_player[Black], m_player[White]);

    m_view->setGame(m_game);

    connect(m_game, &KReversiGame::gameOver, this, &KReversiMainWindow::slotGameOver);
    connect(m_game, &KReversiGame::moveFinished, this, &KReversiMainWindow::slotMoveFinished);

    for (int i = 0; i < 2; i++) // iterating white to black
        if (info.type[i] == GameStartInformation::Human)
            connect(m_view, &KReversiView::userMove,
                    (KReversiHumanPlayer *)(m_player[i]), &KReversiHumanPlayer::onUICellClick);

    updateStatusBar();
    updateHistory();

    if (info.type[White] == GameStartInformation::AI
            && info.type[Black] == GameStartInformation::Human) {
        Kg::difficulty()->setEditable(true);
        Kg::difficulty()->select(Utils::intToDifficultyLevel(info.skill[White]));
    } else if (info.type[White] == GameStartInformation::Human
               && info.type[Black] == GameStartInformation::AI) {
        Kg::difficulty()->setEditable(true);
        Kg::difficulty()->select(Utils::intToDifficultyLevel(info.skill[Black]));
    } else
        Kg::difficulty()->setEditable(false);

    m_hintAct->setEnabled(m_game->isHintAllowed());
    m_undoAct->setEnabled(m_game->canUndo());
}
