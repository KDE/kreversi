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

#include <kreversiview.h>

#include <KLocalizedString>
#include <KStandardDirs>

#include <colorscheme.h>

KReversiView::KReversiView(KReversiGame* game, QWidget *parent, KgThemeProvider *provider) :
    KgDeclarativeView(parent), m_delay(ANIMATION_SPEED_NORMAL), m_game(0),
    m_showLastMove(false), m_showLegalMoves(false),
    m_showLabels(false), m_provider(provider)
{
    m_provider->setDeclarativeEngine("themeProvider", engine());

    qmlRegisterType<ColorScheme>("ColorScheme", 1, 0, "ColorScheme");
    qmlRegisterUncreatableType<KColorScheme>("KColorScheme", 1, 0, "KColorScheme",
                                             "KColorScheme class is uncreatable. Use for enums only");

    QString path =
        KStandardDirs::locate("appdata", QLatin1String("qml/Table.qml"));
    setSource(QUrl::fromLocalFile(path));

    m_qml_root = (QObject*) rootObject();
    rootContext()->setContextProperty("container", this);

    connect(m_qml_root, SIGNAL(cellClicked(int,int)),
            this, SLOT(onPlayerMove(int,int)));
    setGame(game);
}

void KReversiView::setGame(KReversiGame *game)
{
    // disconnect signals from previous game if they exist,
    // we are not interested in them anymore
    if (m_game) {
        disconnect(m_game, SIGNAL(boardChanged()), this, SLOT(updateBoard()));
        disconnect(m_game, SIGNAL(moveFinished()), this, SLOT(gameMoveFinished()));
        disconnect(m_game, SIGNAL(gameOver()), this, SLOT(gameOver()));
        disconnect(m_game, SIGNAL(whitePlayerCantMove()), this, SLOT(whitePlayerCantMove()));
        disconnect(m_game, SIGNAL(blackPlayerCantMove()), this, SLOT(blackPlayerCantMove()));
        delete m_game;
    }

    m_game = game;

    if (m_game) {
        connect(m_game, SIGNAL(boardChanged()), this, SLOT(updateBoard()));
        connect(m_game, SIGNAL(moveFinished()), this, SLOT(gameMoveFinished()));
        connect(m_game, SIGNAL(gameOver()), this, SLOT(gameOver()));
        connect(m_game, SIGNAL(whitePlayerCantMove()), this, SLOT(whitePlayerCantMove()));
        connect(m_game, SIGNAL(blackPlayerCantMove()), this, SLOT(blackPlayerCantMove()));

        m_game->setDelay(m_delay);
    }

    m_hint = KReversiMove();

    updateBoard();
}

void KReversiView::setChipsPrefix(ChipsPrefix chipsPrefix)
{
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
    setGame(0);
}

void KReversiView::updateBoard()
{
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            QString new_state = "";
            if (m_game) // showing empty board if has no game
                switch (m_game->chipColorAt(KReversiMove(NoColor, i, j))) {
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
                                  Q_ARG(QVariant, "Black"));
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

    emit userMove(KReversiPos(row, col));
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
    /// TODO: use Computer, You and Opponent instead in message
    QMetaObject::invokeMethod(m_qml_root, "showPopup",
                              Q_ARG(QVariant,
                                    i18n("White player can not perform any move. It is black turn again.")));
    updateBoard();
}

void KReversiView::blackPlayerCantMove()
{
    /// TODO: use Computer, You and Opponent instead in message
    QMetaObject::invokeMethod(m_qml_root, "showPopup",
                              Q_ARG(QVariant,
                                    i18n("Black player can not perform any move. It is white turn again.")));
    updateBoard();
}
