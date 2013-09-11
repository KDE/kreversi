/*******************************************************************
 *
 * Copyright 2006 Dmitry Suzdalev <dimsuz@gmail.com>
 * Copyright 2010 Brian Croom <brian.s.croom@gmail.com>
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
#include "mainwindow.h"

static const int BLACK_STATUSBAR_ID = 1;
static const int WHITE_STATUSBAR_ID = 2;
static const int COMMON_STATUSBAR_ID = 0;



KReversiMainWindow::KReversiMainWindow(QWidget* parent, bool startDemo)
    : KXmlGuiWindow(parent), m_view(0), m_game(0),
      m_historyDock(0), m_historyView(0),
      m_firstShow(true), m_startInDemoMode(startDemo), /*m_lowestSkill(6),*/
      m_undoAct(0), m_hintAct(0), /*m_demoAct(0),*/ m_startDialog(0)
{
    memset(m_player, 0, sizeof(m_player));

    m_provider = new KgThemeProvider();
    m_provider->discoverThemes("appdata", QLatin1String("pics"));

    statusBar()->insertItem(i18n("Press start game!"), COMMON_STATUSBAR_ID);
    statusBar()->insertItem("", BLACK_STATUSBAR_ID);
    statusBar()->insertItem("", WHITE_STATUSBAR_ID);

    // initialize difficulty stuff
    Kg::difficulty()->addStandardLevelRange(
        KgDifficultyLevel::VeryEasy, KgDifficultyLevel::Impossible,
        KgDifficultyLevel::Easy //default
    );

    KgDifficultyGUI::init(this);
    connect(Kg::difficulty(), SIGNAL(currentLevelChanged(const KgDifficultyLevel*)), SLOT(levelChanged()));
    Kg::difficulty()->setEditable(false);

    // initialize history dock
    m_historyView = new QListWidget(this);
    m_historyView->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    m_historyDock = new QDockWidget(i18n("Move History"));
    m_historyDock->setWidget(m_historyView);
    m_historyDock->setObjectName("history_dock");

    m_historyDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_historyDock);

    // create main game view
    m_view = new KReversiView(m_game, this, m_provider);
    setCentralWidget(m_view);

    // initialise dialog handler
    m_startDialog = new StartGameDialog(this, m_provider);
    connect(m_startDialog, SIGNAL(startGame()), this, SLOT(slotDialogReady()));

    // initialise actions
    setupActionsInit();

    // load saved settings
    loadSettings();

    setupGUI(qApp->desktop()->availableGeometry().size() * 0.7);

    m_historyDock->hide();

    QTimer::singleShot(0, this, SLOT(slotNewGame()));
}

KReversiMainWindow::~KReversiMainWindow()
{
    delete m_provider;
}

void KReversiMainWindow::setupActionsInit()
{
    // Common actions
    KStandardGameAction::gameNew(this, SLOT(slotNewGame()), actionCollection());
    KStandardGameAction::highscores(this, SLOT(slotHighscores()), actionCollection());
    KStandardGameAction::quit(this, SLOT(close()), actionCollection());

    // Undo
    m_undoAct = KStandardGameAction::undo(this, SLOT(slotUndo()), actionCollection());
    m_undoAct->setEnabled(false);   // nothing to undo at the start of the game

    // Hint
    m_hintAct = KStandardGameAction::hint(m_view, SLOT(slotHint()), actionCollection());
    m_hintAct->setEnabled(false);
//    m_demoAct = KStandardGameAction::demo(this, SLOT(slotToggleDemoMode()), actionCollection());

    // Last move
    m_showLast = new KToggleAction(KIcon(QLatin1String("lastmoves")), i18n("Show Last Move"), this);
    actionCollection()->addAction(QLatin1String("show_last_move"), m_showLast);
    connect(m_showLast, SIGNAL(triggered(bool)), m_view, SLOT(setShowLastMove(bool)));

    // Legal moves
    m_showLegal = new KToggleAction(KIcon(QLatin1String("legalmoves")), i18n("Show Legal Moves"), this);
    actionCollection()->addAction(QLatin1String("show_legal_moves"), m_showLegal);
    connect(m_showLegal, SIGNAL(triggered(bool)), m_view, SLOT(setShowLegalMoves(bool)));

    // Animation speed
    m_animSpeedAct = new KSelectAction(i18n("Animation Speed"), this);
    actionCollection()->addAction(QLatin1String("anim_speed"), m_animSpeedAct);

    QStringList acts;
    acts << i18n("Slow") << i18n("Normal") << i18n("Fast");
    m_animSpeedAct->setItems(acts);
    connect(m_animSpeedAct, SIGNAL(triggered(int)), SLOT(slotAnimSpeedChanged(int)));

    // Chip's color
    m_coloredChipsAct = new KToggleAction(i18n("Use Colored Chips"), this);
    actionCollection()->addAction(QLatin1String("use_colored_chips"), m_coloredChipsAct);
    connect(m_coloredChipsAct, SIGNAL(triggered(bool)), SLOT(slotUseColoredChips(bool)));

    // Move history
    // NOTE: read/write this from/to config file? Or not necessary?
    m_showMovesAct = new KToggleAction(KIcon(QLatin1String("view-history")), i18n("Show Move History"), this);
    actionCollection()->addAction(QLatin1String("show_moves"), m_showMovesAct);
    connect(m_showMovesAct, SIGNAL(triggered(bool)), SLOT(slotShowMovesHistory(bool)));
}

void KReversiMainWindow::loadSettings()
{
    // Animation speed
    m_animSpeedAct->setCurrentItem(Preferences::animationSpeed());
    m_view->setAnimationSpeed(Preferences::animationSpeed());

    // Chip's color
    m_coloredChipsAct->setChecked(Preferences::useColoredChips());
    m_view->setChipsPrefix(Preferences::useColoredChips() ?
                KReversiView::Colored : KReversiView::BlackWhite);
    m_startDialog->setColoredChips(Preferences::useColoredChips());
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
    Preferences::self()->writeConfig();
}

void KReversiMainWindow::slotUseColoredChips(bool toggled)
{
    KReversiView::ChipsPrefix chipsPrefix = m_coloredChipsAct->isChecked() ?
                                            KReversiView::Colored :
                                            KReversiView::BlackWhite;
    m_view->setChipsPrefix(chipsPrefix);
    m_startDialog->setColoredChips(toggled);
    Preferences::setUseColoredChips(toggled);
    Preferences::self()->writeConfig();
}

void KReversiMainWindow::slotShowMovesHistory(bool toggled)
{
    m_historyDock->setVisible(toggled);
    m_view->setShowBoardLabels(toggled);
}

//void KReversiMainWindow::slotToggleDemoMode()
//{
//    bool toggled = false;
//    if (m_view->isInDemoMode()) {
//        toggled = false;
//        m_demoAct->setIcon(KIcon(QLatin1String("media-playback-start")));
//        m_demoAct->setChecked(false);
//    } else {
//        // if game is over when user launched Demo, start new game
//        // before Demo starts
//        if (m_game && m_game->isGameOver())
//            slotNewGame();

//        toggled = true
//    m_undoAct->setEnabled(m_game->canUndo());;
//        m_demoAct->setIcon(KIcon(QLatin1String("media-playback-pause")));
//        m_demoAct->setChecked(true);
//    }

//    m_view->setDemoMode(toggled);

//    m_undoAct->setEnabled(!toggled);
//    m_hintAct->setEnabled(!toggled);
//}

void KReversiMainWindow::slotNewGame()
{
//    if (m_demoAct) {
//        m_demoAct->setChecked(false);
//        m_demoAct->setIcon(KIcon(QLatin1String("media-playback-start")));
//    }

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

void KReversiMainWindow::updateHistory() {
    MoveList history = m_game->getHistory();
    m_historyView->clear();

    for (int i = 0; i < history.size(); i++) {
        QString numStr = QString::number(i + 1) + QLatin1String(". ");
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
        kDebug() << "starting demo...";
        startDemo();
    }
    else if (m_firstShow) {
        // TODO: showing start game dialog
    }
    m_firstShow = false;
}

void KReversiMainWindow::updateStatusBar()
{
    if (m_game->isGameOver()) {
        statusBar()->changeItem(i18n("GAME OVER"), COMMON_STATUSBAR_ID);
    }

    if (m_nowPlayingInfo.type[Black] == GameStartInformation::AI
            && m_nowPlayingInfo.type[White] == GameStartInformation::AI) { // using Black White names
        statusBar()->changeItem(i18n("%1: %2",
                                     Utils::colorToString(Black),
                                     m_game->playerScore(Black)), BLACK_STATUSBAR_ID);
        statusBar()->changeItem(i18n("%1: %2",
                                     Utils::colorToString(White),
                                     m_game->playerScore(White)), WHITE_STATUSBAR_ID);

        if (!m_game->isGameOver()) {
            statusBar()->changeItem(i18n("%1 turn",
                                    Utils::colorToString(m_game->currentPlayer())),
                                    COMMON_STATUSBAR_ID);
        }
    } else { // using player's names
        statusBar()->changeItem(i18n("%1: %2", m_nowPlayingInfo.name[Black],
                                     m_game->playerScore(Black)), BLACK_STATUSBAR_ID);
        statusBar()->changeItem(i18n("%1: %2", m_nowPlayingInfo.name[White],
                                     m_game->playerScore(White)), WHITE_STATUSBAR_ID);

        if (!m_game->isGameOver() && m_game->currentPlayer() != NoColor) {
            statusBar()->changeItem(i18n("%1's turn",
                                         m_nowPlayingInfo.name[m_game->currentPlayer()]), COMMON_STATUSBAR_ID);
        }
    }
}


// TODO: test it!!!
void KReversiMainWindow::startDemo() {
    GameStartInformation info;
    info.name[0] = info.name[1] = i18n("Computer");
    info.type[0] = info.type[1] = GameStartInformation::AI;
    info.skill[0] = info.skill[1] = Utils::difficultyLevelToInt();

    receivedGameStartInformation(info);
}

void KReversiMainWindow::clearPlayers() {
    for (int i = 0; i < 2; i++) // iterating through white to black
        if (m_player[i])
        {
            m_player[i]->disconnect();
            delete m_player[i];
            m_player[i] = 0;
        }
}

void KReversiMainWindow::receivedGameStartInformation(GameStartInformation info)
{
    clearPlayers();
    m_nowPlayingInfo = info;

    for (int i = 0; i < 2; i++) // iterating through black and white
        if (info.type[i] == GameStartInformation::AI)
        {
            m_player[i] = new KReversiComputerPlayer(ChipColor(i), info.name[i]);
            ((KReversiComputerPlayer *)(m_player[i]))->setSkill(info.skill[i]);
            levelChanged();
        }
        else
        {
            m_player[i] = new KReversiHumanPlayer(ChipColor(i), info.name[i]);
        }

    m_game = new KReversiGame(m_player[Black], m_player[White]);

    m_view->setGame(m_game);

    connect(m_game, SIGNAL(gameOver()), SLOT(slotGameOver()));
    connect(m_game, SIGNAL(moveFinished()), SLOT(slotMoveFinished()));

    for (int i = 0; i < 2; i++) // iterating white to black
        if (info.type[i] == GameStartInformation::Human)
            connect(m_view, SIGNAL(userMove(KReversiPos)),
                    (KReversiHumanPlayer *)(m_player[i]), SLOT(onUICellClick(KReversiPos)));

    updateStatusBar();
    updateHistory();

    if (info.type[White] == GameStartInformation::AI
         && info.type[Black] == GameStartInformation::Human) {
        Kg::difficulty()->setEditable(true);
        Kg::difficulty()->select(Utils::intToDifficultyLevel(info.skill[White]));
    }
    else if (info.type[White] == GameStartInformation::Human
         && info.type[Black] == GameStartInformation::AI) {
        Kg::difficulty()->setEditable(true);
        Kg::difficulty()->select(Utils::intToDifficultyLevel(info.skill[Black]));
    }
    else
        Kg::difficulty()->setEditable(false);

    m_hintAct->setEnabled(m_game->isHintAllowed());
    m_undoAct->setEnabled(m_game->canUndo());
}
