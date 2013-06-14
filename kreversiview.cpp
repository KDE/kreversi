/*
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
    KgDeclarativeView(parent), m_game(0)
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
    if (m_game)
        this->disconnect(m_game, 0, 0, 0);
    m_game = game;

    if (m_game) {
        connect(m_game, SIGNAL(boardChanged()), this, SLOT(onGameUpdate()));
        connect(m_game, SIGNAL(moveFinished()), this, SLOT(onGameUpdate()));
    }

    onGameUpdate();
}

void KReversiView::onGameUpdate()
{
    if (!m_game)
        return;

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            switch (m_game->chipColorAt(i, j)) {
            case Black:
                QMetaObject::invokeMethod(m_qml_root, "setChipState",
                                          Q_ARG(QVariant, i),
                                          Q_ARG(QVariant, j),
                                          Q_ARG(QVariant, "Black"));
                break;
            case White:
                QMetaObject::invokeMethod(m_qml_root, "setChipState",
                                          Q_ARG(QVariant, i),
                                          Q_ARG(QVariant, j),
                                          Q_ARG(QVariant, "White"));
                break;
            case NoColor:
                QMetaObject::invokeMethod(m_qml_root, "setChipState",
                                          Q_ARG(QVariant, i),
                                          Q_ARG(QVariant, j),
                                          Q_ARG(QVariant, ""));
                break;
            }
}

void KReversiView::onPlayerMove(int row, int col)
{
    if (!m_game)
        return;

    m_game->makePlayerMove(row, col, false);
    m_game->makeComputerMove();
}
