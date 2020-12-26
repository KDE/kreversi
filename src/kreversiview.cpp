/*
    SPDX-FileCopyrightText: 2006-2007 Dmitry Suzdalev <dimsuz@gmail.com>
    SPDX-FileCopyrightText: 2010 Brian Croom <brian.s.croom@gmail.com>
    SPDX-FileCopyrightText: 2013 Denis Kuplyakov <dener.kup@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kreversiview.h"
#include "colorscheme.h"

#include <KLocalizedString>

#include <QStandardPaths>

KReversiView::KReversiView(KReversiGame* game, QWidget *parent, KgThemeProvider *provider)
    : KgDeclarativeView(parent),
    m_provider(provider),
    m_delay(ANIMATION_SPEED_NORMAL),
    m_game(nullptr),
    m_showLastMove(false),
    m_showLegalMoves(false),
    m_showLabels(false)
{
    m_provider->setDeclarativeEngine(QStringLiteral("themeProvider"), engine());

    qmlRegisterType<ColorScheme>("ColorScheme", 1, 0, "ColorScheme");

    QString path = QStandardPaths::locate(QStandardPaths::AppDataLocation, QStringLiteral("qml/Table.qml"));
    setSource(QUrl::fromLocalFile(path));

    m_qml_root = (QObject*) rootObject();
    rootContext()->setContextProperty(QStringLiteral("container"), this);

    connect(m_qml_root, SIGNAL(cellClicked(int,int)),
            this, SLOT(onPlayerMove(int,int)));

    setGame(game);
}

void KReversiView::setGame(KReversiGame *game)
{
    // disconnect signals from previous game if they exist,
    // we are not interested in them anymore
    if (m_game) {
        disconnect(m_game, &KReversiGame::boardChanged, this, &KReversiView::updateBoard);
        disconnect(m_game, &KReversiGame::moveFinished, this, &KReversiView::gameMoveFinished);
        disconnect(m_game, &KReversiGame::gameOver, this, &KReversiView::gameOver);
        disconnect(m_game, &KReversiGame::whitePlayerCantMove, this, &KReversiView::whitePlayerCantMove);
        disconnect(m_game, &KReversiGame::blackPlayerCantMove, this, &KReversiView::blackPlayerCantMove);
        delete m_game;
    }

    m_game = game;

    if (m_game) {
        connect(m_game, &KReversiGame::boardChanged, this, &KReversiView::updateBoard);
        connect(m_game, &KReversiGame::moveFinished, this, &KReversiView::gameMoveFinished);
        connect(m_game, &KReversiGame::gameOver, this, &KReversiView::gameOver);
        connect(m_game, &KReversiGame::whitePlayerCantMove, this, &KReversiView::whitePlayerCantMove);
        connect(m_game, &KReversiGame::blackPlayerCantMove, this, &KReversiView::blackPlayerCantMove);

        m_game->setDelay(m_delay);
    }

    m_hint = KReversiMove();

    updateBoard();
}

void KReversiView::setChipsPrefix(ChipsPrefix chipsPrefix)
{
    m_ColouredChips = chipsPrefix;
    m_qml_root->setProperty("chipsImagePrefix",
                            Utils::chipPrefixToString(chipsPrefix));
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

    if (m_game)
        m_game->setDelay(value);

    m_qml_root->setProperty("chipsAnimationTime", value);
}

KReversiView::~KReversiView()
{
    setGame(nullptr);
}

void KReversiView::updateBoard()
{
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            QMetaObject::invokeMethod(m_qml_root, "setPreAnimationTime",
                                      Q_ARG(QVariant, i),
                                      Q_ARG(QVariant, j),
                                      Q_ARG(QVariant, m_game ? m_game->getPreAnimationDelay(KReversiPos(i, j)) : 0));
        }
    }

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            QString new_state;
            if (m_game) // showing empty board if has no game
                switch (m_game->chipColorAt(KReversiMove(NoColor, i, j))) {
                case Black:
                    new_state = QStringLiteral("Black");
                    break;
                case White:
                    new_state = QStringLiteral("White");
                    break;
                case NoColor:
                    new_state = QString();
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
        MoveList possible_moves = m_game->possibleMoves();
        for (int i = 0; i < possible_moves.size(); i++) {
            QMetaObject::invokeMethod(m_qml_root, "setLegal",
                                      Q_ARG(QVariant, possible_moves.at(i).row),
                                      Q_ARG(QVariant, possible_moves.at(i).col),
                                      Q_ARG(QVariant, true));
        }
    }

    m_qml_root->setProperty("isBoardShowingLabels", m_showLabels);

    if (m_hint.isValid()) {
        QMetaObject::invokeMethod(m_qml_root, "setChipState",
                                  Q_ARG(QVariant, m_hint.row),
                                  Q_ARG(QVariant, m_hint.col),
                                  Q_ARG(QVariant, QLatin1String("Black")));
        QMetaObject::invokeMethod(m_qml_root, "setHint",
                                  Q_ARG(QVariant, m_hint.row),
                                  Q_ARG(QVariant, m_hint.col),
                                  Q_ARG(QVariant, true));
    }

    if (m_game && m_showLastMove) {
        KReversiMove lastmove = m_game->getLastMove();
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
    if (!m_game) {
        m_hint = KReversiMove();
        return;
    }

    m_hint = m_game->getHint();
    updateBoard();
}

void KReversiView::onPlayerMove(int row, int col)
{
    if (!m_game)
        return;

    Q_EMIT userMove(KReversiPos(row, col));
}


void KReversiView::gameMoveFinished()
{
    m_hint = KReversiMove();
    updateBoard();
}

void KReversiView::gameOver()
{
}

void KReversiView::whitePlayerCantMove()
{
    // TODO: use Computer, You and Opponent instead in message
    QMetaObject::invokeMethod(m_qml_root, "showPopup",
                              Q_ARG(QVariant, m_ColouredChips ?
                                    i18n("Red can not perform any move. It is blue turn again.") :
                                    i18n("White can not perform any move. It is black turn again.")));
    updateBoard();
}

void KReversiView::blackPlayerCantMove()
{
    // TODO: use Computer, You and Opponent instead in message
    QMetaObject::invokeMethod(m_qml_root, "showPopup",
                              Q_ARG(QVariant, m_ColouredChips ?
                                    i18n("Blue can not perform any move. It is red turn again.") :
                                    i18n("Black can not perform any move. It is white turn again.")));
    updateBoard();
}
