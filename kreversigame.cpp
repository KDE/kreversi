/*******************************************************************
 *
 * Copyright 2006-2007 Dmitry Suzdalev <dimsuz@gmail.com>
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
#include "kreversigame.h"

const int KReversiGame::DX[KReversiGame::DIRECTIONS_COUNT] = {0, 0, 1, 1, 1, -1, -1, -1};
const int KReversiGame::DY[KReversiGame::DIRECTIONS_COUNT] = {1, -1, 1, 0, -1, 1, 0, -1};

KReversiGame::KReversiGame(KReversiPlayer *blackPlayer, KReversiPlayer *whitePlayer)
    : m_curPlayer(Black)
{
    m_isReady[White] = m_isReady[Black] = false;

    // reset board
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            m_cells[r][c] = NoColor;
    // initial pos
    m_cells[3][3] = m_cells[4][4] = White;
    m_cells[3][4] = m_cells[4][3] = Black;

    m_score[White] = m_score[Black] = 2;

    m_player[White] = whitePlayer;
    m_player[Black] = blackPlayer;

    connect(this, SIGNAL(blackPlayerCantMove()), blackPlayer, SLOT(skipTurn()));
    connect(this, SIGNAL(blackPlayerTurn()), blackPlayer, SLOT(takeTurn()));
    connect(this, SIGNAL(gameOver()), blackPlayer, SLOT(gameOver()));
    connect(blackPlayer, SIGNAL(makeMove(KReversiMove)), this, SLOT(blackPlayerMove(KReversiMove)));
    connect(blackPlayer, SIGNAL(ready()), this, SLOT(blackReady()));

    connect(this, SIGNAL(whitePlayerCantMove()), whitePlayer, SLOT(skipTurn()));
    connect(this, SIGNAL(whitePlayerTurn()), whitePlayer, SLOT(takeTurn()));
    connect(this, SIGNAL(gameOver()), whitePlayer, SLOT(gameOver()));
    connect(whitePlayer, SIGNAL(makeMove(KReversiMove)), this, SLOT(whitePlayerMove(KReversiMove)));
    connect(whitePlayer, SIGNAL(ready()), this, SLOT(whiteReady()));

    m_engine = new Engine(1);

    whitePlayer->prepare(this);
    blackPlayer->prepare(this);
}

KReversiGame::~KReversiGame()
{
    delete m_engine;
}

void KReversiGame::makeMove(const KReversiMove &move)
{
    if (!move.isValid()) {
        kickCurrentPlayer();
        return; // Move is invalid!
    }

    if (move.color != m_curPlayer)
        return; // It's not your turn now

    if (!isMovePossible(move)) {
        kickCurrentPlayer();
        return; // Unpossible move
    }

    m_lastPlayer = m_curPlayer;
    m_curPlayer = NoColor; // both players wait for animations
    m_delayTimer.singleShot(m_delay, this, SLOT(onDelayTimer()));

    turnChips(move);
}

void KReversiGame::startNextTurn()
{
    m_curPlayer = opponentColorFor(m_lastPlayer);

    emit moveFinished(); // previous move has just finished

    if (!isGameOver()) {
        if (isAnyPlayerMovePossible(m_curPlayer)) {
            if (m_curPlayer == White)
                emit whitePlayerTurn();
            else
                emit blackPlayerTurn();
        } else {
            if (m_curPlayer == White)
                emit whitePlayerCantMove();
            else
                emit blackPlayerCantMove();

            m_lastPlayer = m_curPlayer;
            startNextTurn();
        }
    } else { //Game is over
        emit gameOver();
    }
}

int KReversiGame::undo()
{
    // TODO: calc number of undos for each player and use it

    // we're undoing all moves (if any) until we meet move done by a player.
    // We undo that player move too and we're done.
    // Simply put: we're undoing all_moves_of_computer + one_move_of_player

    int movesUndone = 0;

    while (!m_undoStack.isEmpty()) {
        MoveList lastUndo = m_undoStack.pop();
        // One thing that matters here is that we take the
        // chip color directly from board, rather than from move.color
        // That allows to take into account a previously made undo, while
        // undoing changes which are in the current list
        // Sounds not very understandable?
        // Then try to use move.color instead of m_cells[row][col]
        // and it will mess things when undoing such moves as
        // "Player captures computer-owned chip,
        //  Computer makes move and captures this chip back"
        //  Yes, I like long descriptions in comments ;).

        KReversiMove move = lastUndo.takeFirst();
        setChipColor(KReversiMove(NoColor, move.row, move.col));

        // and change back the color of the rest chips
        foreach(const KReversiMove & pos, lastUndo) {
            ChipColor opponentColor = opponentColorFor(m_cells[pos.row][pos.col]);
            setChipColor(KReversiMove(opponentColor, pos.row, pos.col));
        }

        lastUndo.clear();

        movesUndone++;
        if (move.color == m_curPlayer)
            break; //we've undone all opponent's + one current player's moves
    }

    if (!m_undoStack.empty())
        m_changedChips = m_undoStack.top();
    else
        m_changedChips.clear();

    emit boardChanged();
    kickCurrentPlayer();

    return movesUndone;
}

void KReversiGame::turnChips(const KReversiMove &move)
{
    m_changedChips.clear();

    // the first one is the move itself
    setChipColor(move);
    m_changedChips.append(move);
    // now turn color of all chips that were won
    for (int dirNum = 0; dirNum < DIRECTIONS_COUNT; dirNum++) {
        if (hasChunk(dirNum, move)) {
            for (int r = move.row + DX[dirNum], c = move.col + DY[dirNum];
                    r >= 0 && c >= 0 && r < 8 && c < 8;
                    r += DX[dirNum], c += DY[dirNum]) {
                if (m_cells[r][c] == move.color)
                    break;
                setChipColor(KReversiMove(move.color, r, c));
                m_changedChips.append(KReversiMove(move.color, r, c));
            }
        }
    }

    m_undoStack.push(m_changedChips);

    emit boardChanged();
}

bool KReversiGame::isMovePossible(const KReversiMove& move) const
{
    // first - the trivial case:
    if (m_cells[move.row][move.col] != NoColor || move.color == NoColor)
        return false;

    for (int dirNum = 0; dirNum < DIRECTIONS_COUNT; dirNum++)
        if (hasChunk(dirNum, move))
            return true;

    return false;
}

bool KReversiGame::hasChunk(int dirNum, const KReversiMove& move) const
{
    // On each step (as we proceed) we must ensure that current chip is of the
    // opponent color.
    // We'll do our steps until we reach the chip of player color or we reach
    // the end of the board in this direction.
    // If we found player-colored chip and number of opponent chips between it and
    // the starting one is greater than zero, then Hurray! we found a chunk
    //
    // Well, I wrote this description from my head, now lets produce some code for that ;)

    ChipColor opColor = opponentColorFor(move.color);
    int opponentChipsNum = 0;
    bool foundPlayerColor = false;

    for (int r = move.row + DX[dirNum], c = move.col + DY[dirNum];
            r >= 0 && c >= 0 && r < 8 && c < 8;
            r += DX[dirNum], c += DY[dirNum]) {
        ChipColor color = m_cells[r][c];
        if (color == opColor) {
            opponentChipsNum++;
        } else if (color == move.color) {
            foundPlayerColor = true;
            break;
        } else
            break;
    }

    if (foundPlayerColor && opponentChipsNum != 0)
        return true;

    return false;
}

bool KReversiGame::isGameOver() const
{
    // trivial fast-check
    if (m_score[White] + m_score[Black] == 64)
        return true; // the board is full
    else
        return !(isAnyPlayerMovePossible(White) || isAnyPlayerMovePossible(Black));
}

bool KReversiGame::isAnyPlayerMovePossible(ChipColor player) const
{
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            if (m_cells[r][c] == NoColor) {
                // let's see if we can put chip here
                if (isMovePossible(KReversiMove(player, r, c)))
                    return true;
            }
        }
    return false;
}

void KReversiGame::setDelay(int delay)
{
    m_delay = delay;
}

MoveList KReversiGame::getHistory() const
{
    MoveList l;
    for (int i = 0; i < m_undoStack.size(); i++)
        l.push_back(m_undoStack.at(i).at(0));
    return l;
}

void KReversiGame::blackPlayerMove(KReversiMove move)
{
    qDebug() << "Game: black move";
    if (move.color == White)
        return; // Black can't do White moves
    makeMove(move);
}

void KReversiGame::whitePlayerMove(KReversiMove move)
{
    qDebug() << "Game: white move";
    if (move.color == Black)
        return; // White can't do Black moves
    makeMove(move);
}

void KReversiGame::onDelayTimer()
{
    qDebug() << "On Delay";
    startNextTurn();
}

void KReversiGame::blackReady()
{
    qDebug() << "Black ready";
    m_isReady[Black] = true;
    if (m_isReady[White])
        m_player[Black]->takeTurn();
}

void KReversiGame::whiteReady()
{
    qDebug() << "White ready";
    m_isReady[White] = true;
    if (m_isReady[Black])
        m_player[Black]->takeTurn();
}

KReversiMove KReversiGame::getHint() const
{
    /// FIXME: dimsuz: don't use true, use m_competitive
    return m_engine->computeMove(*this, true);
}

KReversiMove KReversiGame::getLastMove() const
{
    // we'll take this move from changed list
    if (m_changedChips.isEmpty())
        return KReversiMove(); // invalid one

    // first item in this list is the actual move, rest is turned chips
    return m_changedChips.first();
}

MoveList KReversiGame::possibleMoves() const
{
    MoveList l;
    if (m_curPlayer == NoColor) // we are at animation period: no move is possible
        return l;

    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            KReversiMove move(m_curPlayer, r, c);
            if (isMovePossible(move))
                l.append(move);
        }
    return l;
}

int KReversiGame::playerScore(ChipColor player) const
{
    return m_score[player];
}

void KReversiGame::setChipColor(const KReversiMove &move)
{
    // first: if the current cell already contains a chip we remove it
    if (m_cells[move.row][move.col] != NoColor)
        m_score[m_cells[move.row][move.col]]--;

    // and now replacing with chip of 'color'
    m_cells[move.row][move.col] = move.color;

    if (move.color != NoColor)
        m_score[move.color]++;
}

ChipColor KReversiGame::chipColorAt(KReversiPos pos) const
{
    return m_cells[pos.row][pos.col];
}


void KReversiGame::kickCurrentPlayer()
{
    if (m_curPlayer == White)
        emit whitePlayerTurn();
    else
        emit blackPlayerTurn();
}

#include "kreversigame.moc"
