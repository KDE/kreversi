/*
    SPDX-FileCopyrightText: 2006-2007 Dmitry Suzdalev <dimsuz@gmail.com>
    SPDX-FileCopyrightText: 2013 Denis Kuplyakov <dener.kup@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kreversigame.h"


const int KReversiGame::DX[KReversiGame::DIRECTIONS_COUNT] = {0, 0, 1, 1, 1, -1, -1, -1};
const int KReversiGame::DY[KReversiGame::DIRECTIONS_COUNT] = {1, -1, 1, 0, -1, 1, 0, -1};

KReversiGame::KReversiGame(KReversiPlayer *blackPlayer, KReversiPlayer *whitePlayer)
    : m_delay(300), m_curPlayer(Black)
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

    connect(this, &KReversiGame::blackPlayerCantMove, blackPlayer, &KReversiPlayer::skipTurn);
    connect(this, &KReversiGame::blackPlayerTurn, blackPlayer, &KReversiPlayer::takeTurn);
    connect(this, &KReversiGame::gameOver, blackPlayer, &KReversiPlayer::gameOver);
    connect(blackPlayer, &KReversiPlayer::makeMove, this, &KReversiGame::blackPlayerMove);
    connect(blackPlayer, &KReversiPlayer::ready, this, &KReversiGame::blackReady);

    connect(this, &KReversiGame::whitePlayerCantMove, whitePlayer, &KReversiPlayer::skipTurn);
    connect(this, &KReversiGame::whitePlayerTurn, whitePlayer, &KReversiPlayer::takeTurn);
    connect(this, &KReversiGame::gameOver, whitePlayer, &KReversiPlayer::gameOver);
    connect(whitePlayer, &KReversiPlayer::makeMove, this, &KReversiGame::whitePlayerMove);
    connect(whitePlayer, &KReversiPlayer::ready, this, &KReversiGame::whiteReady);

    m_engine = new Engine(1);

    whitePlayer->prepare(this);
    blackPlayer->prepare(this);
}

KReversiGame::~KReversiGame()
{
    delete m_engine;
}

bool KReversiGame::canUndo() const
{
    if (m_curPlayer == NoColor)
        return false;
    return (m_player[m_curPlayer]->isUndoAllowed() && !m_undoStack.isEmpty());
}

void KReversiGame::makeMove(KReversiMove move)
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

    turnChips(move);
    
    m_delayTimer.singleShot(m_delay * (qMax(1, m_changedChips.count() - 1)), this, &KReversiGame::onDelayTimer);
    Q_EMIT boardChanged();
}

void KReversiGame::startNextTurn()
{
    m_curPlayer = Utils::opponentColorFor(m_lastPlayer);

    Q_EMIT moveFinished(); // previous move has just finished

    if (!isGameOver()) {
        if (isAnyPlayerMovePossible(m_curPlayer)) {
            if (m_curPlayer == White)
                Q_EMIT whitePlayerTurn();
            else
                Q_EMIT blackPlayerTurn();
        } else {
            if (m_curPlayer == White)
                Q_EMIT whitePlayerCantMove();
            else
                Q_EMIT blackPlayerCantMove();

            m_lastPlayer = m_curPlayer;
            startNextTurn();
        }
    } else { //Game is over
        Q_EMIT gameOver();
    }
}

int KReversiGame::undo()
{
    m_player[m_curPlayer]->undoUsed();
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
        for (const KReversiMove & pos : qAsConst(lastUndo)) {
            ChipColor opponentColor = Utils::opponentColorFor(m_cells[pos.row][pos.col]);
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

    Q_EMIT boardChanged();
    kickCurrentPlayer();

    return movesUndone;
}

void KReversiGame::turnChips(KReversiMove move)
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
}

bool KReversiGame::isMovePossible(KReversiMove move) const
{
    // first - the trivial case:
    if (m_cells[move.row][move.col] != NoColor || move.color == NoColor)
        return false;

    for (int dirNum = 0; dirNum < DIRECTIONS_COUNT; dirNum++)
        if (hasChunk(dirNum, move))
            return true;

    return false;
}

bool KReversiGame::hasChunk(int dirNum, KReversiMove move) const
{
    // On each step (as we proceed) we must ensure that current chip is of the
    // opponent color.
    // We'll do our steps until we reach the chip of player color or we reach
    // the end of the board in this direction.
    // If we found player-colored chip and number of opponent chips between it and
    // the starting one is greater than zero, then Hurray! we found a chunk
    //
    // Well, I wrote this description from my head, now lets produce some code for that ;)

    ChipColor opColor = Utils::opponentColorFor(move.color);
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

int KReversiGame::getPreAnimationDelay(KReversiPos pos) const
{
    for (int i = 1; i < m_changedChips.size(); i++) {
        if (m_changedChips[i].row == pos.row && m_changedChips[i].col == pos.col) {
            return (i - 1) * m_delay;
        }
    }
    return 0;
}

MoveList KReversiGame::getHistory() const
{
    MoveList l;
    for (int i = 0; i < m_undoStack.size(); i++)
        l.push_back(m_undoStack.at(i).at(0));
    return l;
}

bool KReversiGame::isHintAllowed() const
{
    if (m_curPlayer == NoColor)
        return false;
    return m_player[m_curPlayer]->isHintAllowed();
}

void KReversiGame::blackPlayerMove(KReversiMove move)
{
    if (move.color == White)
        return; // Black can't do White moves
    makeMove(move);
}

void KReversiGame::whitePlayerMove(KReversiMove move)
{
    if (move.color == Black)
        return; // White can't do Black moves
    makeMove(move);
}

void KReversiGame::onDelayTimer()
{
    startNextTurn();
}

void KReversiGame::blackReady()
{
    m_isReady[Black] = true;
    if (m_isReady[White])
        m_player[Black]->takeTurn();
}

void KReversiGame::whiteReady()
{
    m_isReady[White] = true;
    if (m_isReady[Black])
        m_player[Black]->takeTurn();
}

KReversiMove KReversiGame::getHint() const
{
    /// FIXME: dimsuz: don't use true, use m_competitive
    m_player[m_curPlayer]->hintUsed();
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

void KReversiGame::setChipColor(KReversiMove move)
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
        Q_EMIT whitePlayerTurn();
    else
        Q_EMIT blackPlayerTurn();
}

