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

#include "kreversiboard.h"
#include "kreversigame.h"
#include "Engine.h"


KReversiGame::KReversiGame()
    : m_curPlayer(Black), m_playerColor(Black), m_computerColor( White )
{
    m_board = new KReversiBoard();
    m_engine = new Engine(1);
}

KReversiGame::~KReversiGame()
{
    delete m_board;
    delete m_engine;
}

void KReversiGame::makePlayerMove( int row, int col, bool demoMode )
{
    m_curPlayer = m_playerColor;
    KReversiMove move;

    if( !demoMode )
        move = KReversiMove( m_playerColor, row, col );
    else
        move = m_engine->computeMove( *this, true );

    if( !isMovePossible(move) )
    {
        kDebug() << "No move possible" << endl;
        return;
    }
    kDebug() << "Black (player) play ("<<move.row<<","<<move.col<<")" <<endl;
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
    m_curPlayer = m_computerColor;
    // FIXME dimsuz: m_competitive. Read from config.
    // (also there's computeMove in getHint)
    KReversiMove move = m_engine->computeMove( *this, true );
    Q_ASSERT(move.color == m_computerColor);
    kDebug() << "White (computer) play ("<<move.row<<","<<move.col<<")" <<endl;
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
        MoveList lastUndo = m_undoStack.pop();
        // One thing that matters here is that we take the
        // chip color directly from board, rather than from move.color
        // That allows to take into account a previously made undo, while
        // undoing changes which are in the current list
        // Sounds not very understandable?
        // Then try to use move.color instead of chipColorAt
        // and it will mess things when undoing such moves as
        // "Player captures computer-owned chip,
        //  Computer makes move and captures this chip back"
        //  Yes, I like long descriptions in comments ;).

        KReversiMove move = lastUndo.takeFirst();
        m_board->setChipColor( NoColor, move.row, move.col );

        // and change back the color of the rest chips
        foreach( KReversiMove mv, lastUndo )
        {
            // NOTE: if chipColorAt == NoColor here, we've some serious problem. It shouldn't be :)
            ChipColor opponentColor = (m_board->chipColorAt(mv.row,mv.col) == White ? Black : White);
            m_board->setChipColor( opponentColor, mv.row, mv.col );
        }

        lastUndo.clear();

        movesUndone++;
        if( move.color == m_playerColor )
            break; //we've undone all computer + one player moves
    }
    
    m_curPlayer = m_playerColor;

    kDebug() << "Undone " << movesUndone << " moves." << endl;
    kDebug() << "Current player changed to " << (m_curPlayer == White ? "White" : "Black" )<< endl;

    emit boardChanged();

    return movesUndone;
}

void KReversiGame::makeMove( const KReversiMove& move )
{
    m_changedChips.clear();

    m_board->setChipColor( move.color, move.row, move.col );
    // the first one is the move itself
    m_changedChips.append( move );
    // now turn color of all chips that were won
    if( hasChunk( Up, move ) )
    {
        for(int r=move.row-1; r >= 0; --r)
        {
            if( m_board->chipColorAt( r, move.col ) == move.color )
                break;
            m_board->setChipColor( move.color, r, move.col );
            m_changedChips.append( KReversiMove( move.color, r, move.col ) );
        }
    }
    if( hasChunk( Down, move ) )
    {
        for(int r=move.row+1; r < 8; ++r)
        {
            if( m_board->chipColorAt( r, move.col ) == move.color )
                break;
            m_board->setChipColor( move.color, r, move.col );
            m_changedChips.append( KReversiMove( move.color, r, move.col ) );
        }
    }
    if( hasChunk( Left, move ) )
    {
        for(int c=move.col-1; c >= 0; --c)
        {
            if( m_board->chipColorAt( move.row, c ) == move.color )
                break;
            m_board->setChipColor( move.color, move.row, c );
            m_changedChips.append( KReversiMove( move.color, move.row, c ) );
        }
    }
    if( hasChunk( Right, move ) )
    {
        for(int c=move.col+1; c < 8; ++c)
        {
            if( m_board->chipColorAt( move.row, c ) == move.color )
                break;
            m_board->setChipColor( move.color, move.row, c );
            m_changedChips.append( KReversiMove( move.color, move.row, c ) );
        }
    }
    if( hasChunk( UpLeft, move ) )
    {
        for(int r=move.row-1, c=move.col-1; r>=0 && c >= 0; --r, --c)
        {
            if( m_board->chipColorAt( r, c ) == move.color )
                break;
            m_board->setChipColor( move.color, r, c );
            m_changedChips.append( KReversiMove( move.color, r, c ) );
        }
    }
    if( hasChunk( UpRight, move ) )
    {
        for(int r=move.row-1, c=move.col+1; r>=0 && c < 8; --r, ++c)
        {
            if( m_board->chipColorAt( r, c ) == move.color )
                break;
            m_board->setChipColor( move.color, r, c );
            m_changedChips.append( KReversiMove( move.color, r, c ) );
        }
    }
    if( hasChunk( DownLeft, move ) )
    {
        for(int r=move.row+1, c=move.col-1; r < 8 && c >= 0; ++r, --c)
        {
            if( m_board->chipColorAt( r, c ) == move.color )
                break;
            m_board->setChipColor( move.color, r, c );
            m_changedChips.append( KReversiMove( move.color, r, c ) );
        }
    }
    if( hasChunk( DownRight, move ) )
    {
        for(int r=move.row+1, c=move.col+1; r < 8 && c < 8; ++r, ++c)
        {
            if( m_board->chipColorAt( r, c ) == move.color )
                break;
            m_board->setChipColor( move.color, r, c );
            m_changedChips.append( KReversiMove( move.color, r, c ) );
        }
    }

    m_curPlayer = (m_curPlayer == White ? Black : White );
    kDebug() << "Current player changed to " << (m_curPlayer == White ? "White" : "Black" )<< endl;
    emit moveFinished();
}

bool KReversiGame::isMovePossible( const KReversiMove& move ) const
{
    // first - the trivial case:
    if( m_board->chipColorAt( move.row, move.col ) != NoColor || move.color == NoColor )
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

bool KReversiGame::hasChunk( Direction dir, const KReversiMove& move ) const
{
    // On each step (as we proceed) we must ensure that current chip is of the 
    // opponent color.
    // We'll do our steps until we reach the chip of player color or we reach
    // the end of the board in this direction.
    // If we found player-colored chip and number of opponent chips between it and
    // the starting one is greater than zero, then Hurray! we found a chunk
    //
    // Well, I wrote this description from my head, now lets produce some code for that ;)

    ChipColor opColor = ( move.color == White ? Black : White );
    int opponentChipsNum = 0;
    bool foundPlayerColor = false;
    if( dir == Up )
    {
        for( int row=move.row-1; row >= 0; --row )
        {
            ChipColor color = m_board->chipColorAt( row, move.col );
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
            ChipColor color = m_board->chipColorAt( row, move.col );
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
            ChipColor color = m_board->chipColorAt( move.row, col );
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
            ChipColor color = m_board->chipColorAt( move.row, col );
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
            ChipColor color = m_board->chipColorAt( row, col );
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
            ChipColor color = m_board->chipColorAt( row, col );
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
            ChipColor color = m_board->chipColorAt( row, col );
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
            ChipColor color = m_board->chipColorAt( row, col );
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
    if( m_board->playerScore(White) + m_board->playerScore(Black) == 64 )
        return true; // the board is full
    else
        return !(isAnyPlayerMovePossible() || isAnyComputerMovePossible());
}

bool KReversiGame::isAnyPlayerMovePossible() const
{
    for( int r=0; r<8; ++r )
        for( int c=0; c<8; ++c )
        {
            if( m_board->chipColorAt(r,c) == NoColor )
            {
                // let's see if we can put chip here
                if( isMovePossible( KReversiMove( m_playerColor, r, c ) ) )
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
            if( m_board->chipColorAt(r,c) == NoColor )
            {
                // let's see if we can put chip here
                if( isMovePossible( KReversiMove( m_computerColor, r, c ) ) )
                        return true;
            }
        }
    return false;
}

void KReversiGame::setComputerSkill(int skill)
{
    kDebug() << "setting skill to " << skill << endl;
    m_engine->setStrength( skill );
}

KReversiMove KReversiGame::getHint() const
{
    // FIXME dimsuz: don't use true, use m_competitive
    return m_engine->computeMove( *this, true );
}

KReversiMove KReversiGame::getLastMove() const
{
    // we'll take this move from changed list
    if( m_changedChips.isEmpty() )
        return KReversiMove(NoColor, -1, -1); // invalid one

    // first item in this list is the actual move, rest is turned chips
    return m_changedChips.first();
}

MoveList KReversiGame::possibleMoves() const
{
    MoveList l;
    for(int row=0; row < 8; ++row)
        for(int col=0; col<8; ++col)
        {
            KReversiMove move(m_curPlayer, row, col);
            if( isMovePossible( move ) )
                l.append(move);
        }
    return l;
}

int KReversiGame::playerScore( ChipColor player ) const
{
    return m_board->playerScore( player );
}

ChipColor KReversiGame::chipColorAt( int row, int col ) const
{ 
    return m_board->chipColorAt( row, col ); 
}

#include "kreversigame.moc"
