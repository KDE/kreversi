/*******************************************************************
 *
 * Copyright 2006 Dmitry Suzdalev <dimsuz@gmail.com>
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
#include <kdebug.h>

#include <kggzmod/module.h>
#include <kggzmod/player.h>
#include <kggznet/kggzraw.h>

#include "kreversigame.h"
#include "Engine.h"

KReversiGame::KReversiGame()
    : m_curPlayer(Black), m_playerColor(Black), m_computerColor( White ),
      m_mod(0), m_raw(0)
{
    // reset board
    for(int r=0; r<8; ++r)
        for(int c=0; c<8; ++c)
            m_cells[r][c] = NoColor;
    // initial pos
    m_cells[3][3] = m_cells[4][4] = White;
    m_cells[3][4] = m_cells[4][3] = Black;

    m_score[White] = m_score[Black] = 2;

    m_engine = new Engine(1);

    kDebug() << "GGZDEBUG: see if we're in ggz mode" << endl;
    if(KGGZMod::Module::isGGZ())
    {
        kDebug() << "GGZDEBUG: yep we're in ggz mode, now activate kggzmod" << endl;
        m_mod = new KGGZMod::Module("KReversi");
        connect(m_mod, SIGNAL(signalError()), SLOT(networkErrorHandler()));
        connect(m_mod, SIGNAL(signalNetwork(int)), SLOT(networkData(int)));
        kDebug() << "GGZDEBUG: kggzmod activated" << endl;
    }
    m_raw = NULL;
}

KReversiGame::~KReversiGame()
{
    delete m_engine;
    delete m_raw;
    delete m_mod;
}

void KReversiGame::makePlayerMove( int row, int col, bool demoMode )
{
    m_curPlayer = m_playerColor;
    KReversiPos move;

    if( m_mod )
    {
        makeNetworkMove( row, col );
        return;
    }

    if( !demoMode )
        move = KReversiPos( m_playerColor, row, col );
    else
        move = m_engine->computeMove( *this, true );

    if( !isMovePossible(move) )
    {
        kDebug() << "No move possible" << endl;
        return;
    }
    //kDebug() << "Black (player) play ("<<move.row<<","<<move.col<<")" <<endl;
    makeMove( move );
    m_undoStack.push( m_changedChips );
}

void KReversiGame::startNextTurn(bool demoMode)
{
    if( !isGameOver() )
    {
        if( isComputersTurn() )
        {
            if(isAnyComputerMovePossible())
            {
                makeComputerMove();
            }
            else if( demoMode )
            {
                    makePlayerMove(-1, -1, true );
            }
            else // no comp move possible and not in demo mode
            {
                kDebug() << "Computer can't move!" << endl;
                m_curPlayer = m_playerColor;
                emit computerCantMove();
            }
        }
        else
        {
            // if player cant move let the computer play again!
            if( !isAnyPlayerMovePossible() )
            {
                // FIXME dimsuz: emit something like "playerCantMove()" for gui to catch this?
                kDebug() << "Player can't move!" << endl;
                makeComputerMove();
            }
            else if( demoMode ) // let the computer play instead of player
            {
                makePlayerMove( -1, -1, true );
            }
        }
    }
    else
    {
        kDebug() << "GAME OVER" << endl;
        emit gameOver();
    }
}

void KReversiGame::makeComputerMove()
{
    if( m_mod )
    {
        // Network games will automatically send us the opponent move
        return;
    }

    m_curPlayer = m_computerColor;
    // FIXME dimsuz: m_competitive. Read from config.
    // (also there's computeMove in getHint)
    KReversiPos move = m_engine->computeMove( *this, true );
    Q_ASSERT(move.color == m_computerColor);
    //kDebug() << "White (computer) play ("<<move.row<<","<<move.col<<")" <<endl;
    makeMove(move);
    m_undoStack.push( m_changedChips );
}

int KReversiGame::undo()
{
    // we're undoing all moves (if any) until we meet move done by a player.
    // We undo that player move too and we're done.
    // Simply put: we're undoing all_moves_of_computer + one_move_of_player

    int movesUndone = 0;

    while( !m_undoStack.isEmpty() )
    {
        PosList lastUndo = m_undoStack.pop();
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

        KReversiPos move = lastUndo.takeFirst();
        setChipColor( NoColor, move.row, move.col );

        // and change back the color of the rest chips
        foreach( const KReversiPos &pos, lastUndo )
        {
            ChipColor opponentColor = opponentColorFor( m_cells[pos.row][pos.col] );
            setChipColor( opponentColor, pos.row, pos.col );
        }

        lastUndo.clear();

        movesUndone++;
        if( move.color == m_playerColor )
            break; //we've undone all computer + one player moves
    }
    
    m_curPlayer = m_playerColor;

    kDebug() << "Undone " << movesUndone << " moves." << endl;
    //kDebug() << "Current player changed to " << (m_curPlayer == White ? "White" : "Black" )<< endl;

    emit boardChanged();

    return movesUndone;
}

void KReversiGame::makeMove( const KReversiPos& move )
{
    m_changedChips.clear();

    setChipColor( move.color, move.row, move.col );
    // the first one is the move itself
    m_changedChips.append( move );
    // now turn color of all chips that were won
    if( hasChunk( Up, move ) )
    {
        for(int r=move.row-1; r >= 0; --r)
        {
            if( m_cells[r][move.col] == move.color )
                break;
            setChipColor( move.color, r, move.col );
            m_changedChips.append( KReversiPos( move.color, r, move.col ) );
        }
    }
    if( hasChunk( Down, move ) )
    {
        for(int r=move.row+1; r < 8; ++r)
        {
            if( m_cells[r][move.col] == move.color )
                break;
            setChipColor( move.color, r, move.col );
            m_changedChips.append( KReversiPos( move.color, r, move.col ) );
        }
    }
    if( hasChunk( Left, move ) )
    {
        for(int c=move.col-1; c >= 0; --c)
        {
            if( m_cells[move.row][c] == move.color )
                break;
            setChipColor( move.color, move.row, c );
            m_changedChips.append( KReversiPos( move.color, move.row, c ) );
        }
    }
    if( hasChunk( Right, move ) )
    {
        for(int c=move.col+1; c < 8; ++c)
        {
            if( m_cells[move.row][c] == move.color )
                break;
            setChipColor( move.color, move.row, c );
            m_changedChips.append( KReversiPos( move.color, move.row, c ) );
        }
    }
    if( hasChunk( UpLeft, move ) )
    {
        for(int r=move.row-1, c=move.col-1; r>=0 && c >= 0; --r, --c)
        {
            if( m_cells[r][c] == move.color )
                break;
            setChipColor( move.color, r, c );
            m_changedChips.append( KReversiPos( move.color, r, c ) );
        }
    }
    if( hasChunk( UpRight, move ) )
    {
        for(int r=move.row-1, c=move.col+1; r>=0 && c < 8; --r, ++c)
        {
            if( m_cells[r][c] == move.color )
                break;
            setChipColor( move.color, r, c );
            m_changedChips.append( KReversiPos( move.color, r, c ) );
        }
    }
    if( hasChunk( DownLeft, move ) )
    {
        for(int r=move.row+1, c=move.col-1; r < 8 && c >= 0; ++r, --c)
        {
            if( m_cells[r][c] == move.color )
                break;
            setChipColor( move.color, r, c );
            m_changedChips.append( KReversiPos( move.color, r, c ) );
        }
    }
    if( hasChunk( DownRight, move ) )
    {
        for(int r=move.row+1, c=move.col+1; r < 8 && c < 8; ++r, ++c)
        {
            if( m_cells[r][c] == move.color )
                break;
            setChipColor( move.color, r, c );
            m_changedChips.append( KReversiPos( move.color, r, c ) );
        }
    }

    m_curPlayer = (m_curPlayer == White ? Black : White );
    //kDebug() << "Current player changed to " << (m_curPlayer == White ? "White" : "Black" )<< endl;
    emit moveFinished();
}

bool KReversiGame::isMovePossible( const KReversiPos& move ) const
{
    // first - the trivial case:
    if( m_cells[move.row][move.col] != NoColor || move.color == NoColor )
        return false;

    if( hasChunk( Up, move ) || hasChunk( Down, move ) 
            || hasChunk( Left, move ) || hasChunk( Right, move ) 
            || hasChunk( UpLeft, move ) || hasChunk( UpRight, move ) 
            || hasChunk( DownLeft, move ) || hasChunk( DownRight, move ) )
    {
        return true;
    }

    return false;
}

bool KReversiGame::hasChunk( Direction dir, const KReversiPos& move ) const
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
    if( dir == Up )
    {
        for( int row=move.row-1; row >= 0; --row )
        {
            ChipColor color = m_cells[row][move.col];
            if( color == opColor )
            {
                opponentChipsNum++;
            }
            else if(color == move.color)
            {
                foundPlayerColor = true;
                break; //bail out
            }
            else // NoColor
                break; // no luck in this direction
        }

        if(foundPlayerColor && opponentChipsNum != 0)
            return true;
    }
    else if( dir == Down )
    {
        for( int row=move.row+1; row < 8; ++row )
        {
            ChipColor color = m_cells[row][move.col];
            if( color == opColor )
            {
                opponentChipsNum++;
            }
            else if(color == move.color)
            {
                foundPlayerColor = true;
                break; //bail out
            }
            else // NoColor
                break; // no luck in this direction
        }

        if(foundPlayerColor && opponentChipsNum != 0)
            return true;
    }
    else if( dir == Left )
    {
        for( int col=move.col-1; col >= 0; --col )
        {
            ChipColor color = m_cells[move.row][col];
            if( color == opColor )
            {
                opponentChipsNum++;
            }
            else if(color == move.color)
            {
                foundPlayerColor = true;
                break; //bail out
            }
            else // NoColor
                break; // no luck in this direction
        }

        if(foundPlayerColor && opponentChipsNum != 0)
            return true;
    }
    else if( dir == Right )
    {
        for( int col=move.col+1; col < 8; ++col )
        {
            ChipColor color = m_cells[move.row][col];
            if( color == opColor )
            {
                opponentChipsNum++;
            }
            else if(color == move.color)
            {
                foundPlayerColor = true;
                break; //bail out
            }
            else // NoColor
                break; // no luck in this direction
        }

        if(foundPlayerColor && opponentChipsNum != 0)
            return true;
    }
    else if( dir == UpLeft )
    {
        for( int row=move.row-1, col=move.col-1; row >= 0 && col >= 0; --row, --col )
        {
            ChipColor color = m_cells[row][col];
            if( color == opColor )
            {
                opponentChipsNum++;
            }
            else if(color == move.color)
            {
                foundPlayerColor = true;
                break; //bail out
            }
            else // NoColor
                break; // no luck in this direction
        }

        if(foundPlayerColor && opponentChipsNum != 0)
            return true;
    }
    else if( dir == UpRight )
    {
        for( int row=move.row-1, col=move.col+1; row >= 0 && col < 8; --row, ++col )
        {
            ChipColor color = m_cells[row][col];
            if( color == opColor )
            {
                opponentChipsNum++;
            }
            else if(color == move.color)
            {
                foundPlayerColor = true;
                break; //bail out
            }
            else // NoColor
                break; // no luck in this direction
        }

        if(foundPlayerColor && opponentChipsNum != 0)
            return true;
    }
    else if( dir == DownLeft )
    {
        for( int row=move.row+1, col=move.col-1; row < 8 && col >= 0; ++row, --col )
        {
            ChipColor color = m_cells[row][col];
            if( color == opColor )
            {
                opponentChipsNum++;
            }
            else if(color == move.color)
            {
                foundPlayerColor = true;
                break; //bail out
            }
            else // NoColor
                break; // no luck in this direction
        }

        if(foundPlayerColor && opponentChipsNum != 0)
            return true;
    }
    else if( dir == DownRight )
    {
        for( int row=move.row+1, col=move.col+1; row < 8 && col < 8; ++row, ++col )
        {
            ChipColor color = m_cells[row][col];
            if( color == opColor )
            {
                opponentChipsNum++;
            }
            else if(color == move.color)
            {
                foundPlayerColor = true;
                break; //bail out
            }
            else // NoColor
                break; // no luck in this direction
        }

        if(foundPlayerColor && opponentChipsNum != 0)
            return true;
    }

    return false;
}

bool KReversiGame::isGameOver() const
{
    // trivial fast-check
    if( m_score[White] + m_score[Black] == 64 )
        return true; // the board is full
    else
        return !(isAnyPlayerMovePossible() || isAnyComputerMovePossible());
}

bool KReversiGame::isAnyPlayerMovePossible() const
{
    for( int r=0; r<8; ++r )
        for( int c=0; c<8; ++c )
        {
            if( m_cells[r][c] == NoColor )
            {
                // let's see if we can put chip here
                if( isMovePossible( KReversiPos( m_playerColor, r, c ) ) )
                        return true;
            }
        }
    return false;
}

bool KReversiGame::isAnyComputerMovePossible() const
{
    for( int r=0; r<8; ++r )
        for( int c=0; c<8; ++c )
        {
            if( m_cells[r][c] == NoColor )
            {
                // let's see if we can put chip here
                if( isMovePossible( KReversiPos( m_computerColor, r, c ) ) )
                        return true;
            }
        }
    return false;
}

void KReversiGame::setComputerSkill(int skill)
{
    m_engine->setStrength( skill );
}

KReversiPos KReversiGame::getHint() const
{
    // FIXME dimsuz: don't use true, use m_competitive
    return m_engine->computeMove( *this, true );
}

KReversiPos KReversiGame::getLastMove() const
{
    // we'll take this move from changed list
    if( m_changedChips.isEmpty() )
        return KReversiPos(NoColor, -1, -1); // invalid one

    // first item in this list is the actual move, rest is turned chips
    return m_changedChips.first();
}

PosList KReversiGame::possibleMoves() const
{
    PosList l;
    for(int row=0; row < 8; ++row)
        for(int col=0; col<8; ++col)
        {
            KReversiPos move(m_curPlayer, row, col);
            if( isMovePossible( move ) )
                l.append(move);
        }
    return l;
}

int KReversiGame::playerScore( ChipColor player ) const
{
    return m_score[player];
}

void KReversiGame::setChipColor(ChipColor color, int row, int col)
{
    Q_ASSERT( row < 8 && col < 8 );
    // first: if the current cell already contains a chip of the opponent,
    // we'll decrease opponents score
    if( m_cells[row][col] != NoColor && color != NoColor && m_cells[row][col] != color )
        m_score[opponentColorFor(color)]--;
    // if the cell contains some chip and is being replaced by NoColor,
    // we'll decrease the score of that color
    // Such replacements (with NoColor) occur during undoing
    if( m_cells[row][col] != NoColor && color == NoColor )
        m_score[ m_cells[row][col] ]--;

    // and now replacing with chip of 'color'
    m_cells[row][col] = color;

    if( color != NoColor )
        m_score[color]++;

    //kDebug() << "Score of White player: " << m_score[White] << endl;
    //kDebug() << "Score of Black player: " << m_score[Black] << endl;
}

ChipColor KReversiGame::chipColorAt( int row, int col ) const
{ 
    Q_ASSERT( row < 8 && col < 8 );
    return m_cells[row][col];
}

////////////////////////////////////////////////////////////////////////////
// Network code starts here - might be moved or auto-generated in the future
////////////////////////////////////////////////////////////////////////////

// FIXME: move those to a protocol class
#define MSG_SEAT 0
#define MSG_PLAYERS 1
#define MSG_MOVE 2
#define MSG_GAMEOVER 3
#define MSG_START 5
#define MSG_SYNC 6

#define REQ_MOVE 4
#define REQ_SYNC 7
#define REQ_AGAIN 8

void KReversiGame::networkErrorHandler()
{
    kError() << "GGZDEBUG: Network error, disconnect all channels" << endl;
    delete m_raw;
    delete m_mod;
    m_raw = NULL;
    m_mod = NULL;

    emit networkError();
    // This is either on the GGZ channel or on the game channel
    // FIXME: propagate error information to the user
}

void KReversiGame::networkData(int fd)
{
    int opcode;
    QString playername1;
    QString playername2;
    int playertype1;
    int playertype2;
    int seat;
    qint8 turn, boardfield[64];
    int winner;
    int movevalue;
    KReversiPos move;

    kDebug() << "GGZDEBUG: Network traffic on fd " << fd << endl;

    if(!m_raw)
    {
        kDebug() << "GGZDEBUG: Set up packet reader" << endl;
        m_raw = new KGGZRaw();
        m_raw->setNetwork(fd);
        connect(m_raw, SIGNAL(signalError()), SLOT(networkErrorHandler()));
    }

    *m_raw >> opcode;
    kDebug() << "GGZDEBUG: opcode=" << opcode << endl;

    if(opcode == MSG_SEAT)
    {
        *m_raw >> seat;
        kDebug() << "GGZDEBUG: MSG_SEAT: seat=" << seat << endl;
    }
    else if(opcode == MSG_PLAYERS)
    {
        *m_raw >> playertype1;
        if(playertype1 != KGGZMod::Player::open)
        {
            *m_raw >> playername1;
        }
        *m_raw >> playertype2;
        if(playertype2 != KGGZMod::Player::open)
        {
            *m_raw >> playername2;
        }
        kDebug() << "GGZDEBUG: MSG_PLAYERS:" << endl;
        kDebug() << " player1=" << playername1 << endl;
        kDebug() << " player2=" << playername2 << endl;
    }
    else if(opcode == MSG_SYNC)
    {
        *m_raw >> turn;
        for(int i = 0; i < 64; i++)
        {
            *m_raw >> boardfield[i];
        }
        kDebug() << "GGZDEBUG: MSG_SYNC: turn=" << turn << endl;
    }
    else if(opcode == MSG_START)
    {
        kDebug() << "GGZDEBUG: MSG_START" << endl;
    }
    else if(opcode == MSG_MOVE)
    {
        *m_raw >> movevalue;
        kDebug() << "GGZDEBUG: MSG_MOVE move=" << movevalue << endl;

        if( movevalue == -1 )
        {
            // FIXME: do something here
            return;
        }

        // translate protocol moves into local moves
        ChipColor color = currentPlayer();
        int row = movevalue % 8;
        int col = movevalue / 8;
        // FIXME: re-use the code to handle impossible moves (i.e. double turns)

        move = KReversiPos( color, row, col );
        makeMove( move );
    }
    else if(opcode == MSG_GAMEOVER)
    {
        *m_raw >> winner;
        kDebug() << "GGZDEBUG: MSG_WINNER winner=" << winner << endl;
    }
    else
    {
        kDebug() << "GGZDEBUG: Waaaah, we've not implemented the whole protocol yet!" << endl;
        networkError();
    }
}

void KReversiGame::makeNetworkMove( int row, int col )
{
    int movevalue = col * 8 + row;

    if( !m_raw )
    {
        kError() << "GGZDEBUG: Not connected to server" << endl;
        return;
    }

    kDebug() << "GGZDEBUG: submit move " << movevalue << " to network" << endl;
    *m_raw << REQ_MOVE;
    *m_raw << movevalue;
}

#include "kreversigame.moc"
