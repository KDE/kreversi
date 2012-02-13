/*******************************************************************
 *
 * Copyright 2006-2007 Dmitry Suzdalev <dimsuz@gmail.com>
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

#include <kdebug.h>

#include "Engine.h"

KReversiGame::KReversiGame()
    : m_curPlayer(Black), m_playerColor(Black), m_computerColor( White )
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
}

KReversiGame::~KReversiGame()
{
    delete m_engine;
}

void KReversiGame::makePlayerMove( int row, int col, bool demoMode )
{
    m_curPlayer = m_playerColor;
    KReversiPos move;

    if( !demoMode )
        move = KReversiPos( m_playerColor, row, col );
    else
    {
        move = m_engine->computeMove( *this, true );
        if( !move.isValid() )
            return;
    }

    if( !isMovePossible(move) )
    {
        kDebug() << "No move possible";
        return;
    }
    //kDebug() << "Black (player) play ("<<move.row<<","<<move.col<<")";
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
                kDebug() << "Computer can't move!";
                m_curPlayer = m_playerColor;
                emit computerCantMove();
            }
        }
        else
        {
            // if player cant move let the computer play again!
            if( !isAnyPlayerMovePossible() )
            {
                emit playerCantMove();
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
        kDebug() << "GAME OVER";
        emit gameOver();
    }
}

void KReversiGame::makeComputerMove()
{
    m_curPlayer = m_computerColor;
    // FIXME dimsuz: m_competitive. Read from config.
    // (also there's computeMove in getHint)
    KReversiPos move = m_engine->computeMove( *this, true );
    if( !move.isValid() )
        return;

    if( move.color != m_computerColor )
    {
        kDebug() << "Strange! makeComputerMove() just got not computer move!";
        return;
    }

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

    kDebug() << "Undone" << movesUndone << "moves.";
    //kDebug() << "Current player changed to" << (m_curPlayer == White ? "White" : "Black" );

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
    //kDebug() << "Current player changed to" << (m_curPlayer == White ? "White" : "Black" );
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

bool KReversiGame::isThinking() const
{
    if( !m_engine )
        return false;

    return m_engine->isThinking();
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

    //kDebug() << "Score of White player:" << m_score[White];
    //kDebug() << "Score of Black player:" << m_score[Black];
}

ChipColor KReversiGame::chipColorAt( int row, int col ) const
{
    Q_ASSERT( row < 8 && col < 8 );
    return m_cells[row][col];
}

#include "kreversigame.moc"
