/*
    Copyright 2006-2007 Dmitry Suzdalev <dimsuz@gmail.com>
    Copyright 2010 Brian Croom <brian.s.croom@gmail.com>
    Copyright 2013 Denis Kuplyakov <dener.kup@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kreversiview.h"
#include "kreversiscene.h"

KReversiView::KReversiView(KReversiGame* game, QWidget *parent) :
    KgDeclarativeView(parent), m_delay(ANIMATION_SPEED_NORMAL), m_game(0),
    m_demoMode(false), m_showLastMove(false), m_showLegalMoves(false),
    m_showLabels(false)
{
    m_provider = new KgThemeProvider();
    m_provider->discoverThemes("appdata", QLatin1String("pics"));
    m_provider->setDeclarativeEngine("themeProvider", engine());

    QString path =
        KStandardDirs::locate("appdata", QLatin1String("qml/Table.qml"));
    setSource(QUrl::fromLocalFile(path));

    m_qml_root = (QObject*) rootObject();
    rootContext()->setContextProperty("container", this);

    connect(m_qml_root, SIGNAL(cellClicked(int, int)),
            this, SLOT(onPlayerMove(int, int)));
    setGame(game);
}

void KReversiView::setGame(KReversiGame *game)
{
    // disconnect signals from previous game if it exists,
    // we are not interested in them anymore
    if (m_game) {
        disconnect( m_game, SIGNAL(boardChanged()), this, SLOT(updateBoard()) );
        disconnect( m_game, SIGNAL(moveFinished()), this, SLOT(slotGameMoveFinished()) );
        disconnect( m_game, SIGNAL(gameOver()), this, SLOT(slotGameOver()) );
        disconnect( m_game, SIGNAL(computerCantMove()), this, SLOT(slotComputerCantMove()) );
        disconnect( m_game, SIGNAL(playerCantMove()), this, SLOT(slotPlayerCantMove()) );
        delete m_game;
    }

    m_game = game;

    connect(m_game, SIGNAL(boardChanged()), this, SLOT(updateBoard()));
    connect(m_game, SIGNAL(moveFinished()), this, SLOT(slotGameMoveFinished()));
    connect(m_game, SIGNAL(gameOver()), this, SLOT(slotGameOver()));
    connect(m_game, SIGNAL(computerCantMove()), this, SLOT(slotComputerCantMove()));
    connect(m_game, SIGNAL(playerCantMove()), this, SLOT(slotPlayerCantMove()));


    m_hint = KReversiPos();
    m_demoMode = false;

    updateBoard();
}

void KReversiView::setChipsPrefix(const QString &chipsPrefix)
{
    QMetaObject::invokeMethod(m_qml_root, "setChipsPrefix",
                              Q_ARG(QVariant, chipsPrefix));
}

void KReversiView::setShowBoardLabels(bool show)
{
    m_showLabels = show;
    updateBoard();
}

void KReversiView::setAnimationSpeed(int speed)
{
    int value = ANIMATION_SPEED_NORMAL;
    switch (speed) {
    case 0:
        value = ANIMATION_SPEED_SLOW;
        break;
    default:
    case 1:
        value = ANIMATION_SPEED_NORMAL;
        break;
    case 2:
        value = ANIMATION_SPEED_FAST;
        break;
    }

    m_delay = value;

    QMetaObject::invokeMethod(m_qml_root, "setAnimationTime",
                              Q_ARG(QVariant, value));
}

void KReversiView::updateBoard()
{
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            QString new_state = "";
            if (m_game) // showing empty board if has no game
                switch (m_game->chipColorAt(i, j)) {
                case Black:
                    new_state = "Black";
                    break;
                case White:
                    new_state = "White";
                    break;
                case NoColor:
                    new_state = "";
                    break;
                }

            QMetaObject::invokeMethod(m_qml_root, "setChipState",
                                      Q_ARG(QVariant, i),
                                      Q_ARG(QVariant, j),
                                      Q_ARG(QVariant, new_state));

            // clearing legal markers, hints and lastmove
            QMetaObject::invokeMethod(m_qml_root, "setLegal",
                                      Q_ARG(QVariant, i),
                                      Q_ARG(QVariant, j),
                                      Q_ARG(QVariant, false));
            QMetaObject::invokeMethod(m_qml_root, "setHint",
                                      Q_ARG(QVariant, i),
                                      Q_ARG(QVariant, j),
                                      Q_ARG(QVariant, false));
            QMetaObject::invokeMethod(m_qml_root, "setLastMove",
                                      Q_ARG(QVariant, i),
                                      Q_ARG(QVariant, j),
                                      Q_ARG(QVariant, false));
        }

    if (m_game && m_showLegalMoves) {
        PosList possible_moves = m_game->possibleMoves();
        for (int i = 0; i < possible_moves.size(); i++) {
            QMetaObject::invokeMethod(m_qml_root, "setLegal",
                                      Q_ARG(QVariant, possible_moves.at(i).row),
                                      Q_ARG(QVariant, possible_moves.at(i).col),
                                      Q_ARG(QVariant, true));
        }
    }

    QMetaObject::invokeMethod(m_qml_root, "setLabels",
                              Q_ARG(QVariant, m_showLabels));

    if (m_hint.isValid()) {
        QMetaObject::invokeMethod(m_qml_root, "setHint",
                                  Q_ARG(QVariant, m_hint.row),
                                  Q_ARG(QVariant, m_hint.col),
                                  Q_ARG(QVariant, true));
    }

    if (m_game && m_showLastMove) {
        KReversiPos lastmove = m_game->getLastMove();
        if (lastmove.isValid())
            QMetaObject::invokeMethod(m_qml_root, "setLastMove",
                                      Q_ARG(QVariant, lastmove.row),
                                      Q_ARG(QVariant, lastmove.col),
                                      Q_ARG(QVariant, true));
    }
}

void KReversiView::setShowLastMove(bool show)
{
    m_showLastMove = show;
    updateBoard();
}

void KReversiView::setShowLegalMoves(bool show)
{
    m_showLegalMoves = show;
    updateBoard();
}

void KReversiView::slotHint()
{
    if (!m_game)
    {
        m_hint = KReversiPos();
        return;
    }

    if (m_game->isComputersTurn())
        return;

    m_hint = m_game->getHint();
    updateBoard();
}

void KReversiView::setDemoMode(bool state)
{
    if (!m_game)
        return;

    if (m_game->isGameOver())
        return;

    m_demoMode = state;

    if (!m_game->isComputersTurn())
        m_game->startNextTurn(m_demoMode);
}

void KReversiView::onPlayerMove(int row, int col)
{
    if (!m_game)
        return;

    // user moves not allowed in demo mode
    if (m_demoMode)
        return;

    if (m_game->isComputersTurn())
        return;

    m_game->makePlayerMove(row, col, false);
}


void KReversiView::slotGameMoveFinished()
{
    m_hint = KReversiPos();
    updateBoard();
    emit moveFinished();
    m_delayTimer.singleShot(m_delay, this, SLOT(slotOnDelay()));
}

void KReversiView::slotGameOver()
{
    m_demoMode = false;
}

void KReversiView::slotComputerCantMove()
{
    QMetaObject::invokeMethod(m_qml_root, "showPopup",
        Q_ARG(QVariant,
        i18n("Computer can not move. It is your turn again.")));
}

void KReversiView::slotPlayerCantMove()
{

    QMetaObject::invokeMethod(m_qml_root, "showPopup",
        Q_ARG(QVariant,
        i18n("You can not perform any move. Computer takes next turn now.")));
}

void KReversiView::slotOnDelay()
{
    if (m_game)
        m_game->startNextTurn(m_demoMode);
}
