#include <kdebug.h>

#include "kreversiboard.h"
#include "kreversigame.h"
#include "Engine.h"


KReversiGame::KReversiGame()
    : m_curPlayer(Black), m_computerColor( White )
{
    m_board = new KReversiBoard();
    m_engine = new Engine(1);
}

KReversiGame::~KReversiGame()
{
    delete m_board;
    delete m_engine;
}

void KReversiGame::makePlayerMove( int row, int col )
{
// FIXME dimsuz: debug temporary code
    if( m_curPlayer == m_computerColor )
    {
        kDebug() << "Warning! Player turn called while it's computer turn" << endl;
        return;
    }

    KReversiMove move( m_curPlayer, row, col );
    if( !isMovePossible(move) )
    {
        kDebug() << "No move possible" << endl;
        return;
    }
    makeMove( move );
}

void KReversiGame::makeComputerMove()
{
// FIXME dimsuz: debug temporary code
    if( m_curPlayer != m_computerColor )
    {
        kDebug() << "Warning! Computer turn called while it's player turn" << endl;
        return;
    }
    KReversiMove move = m_engine->computeMove( *this, true );
    // FIXME dimsuz: if move.color == m_computerColor then this might mean that player already has won!
    // Check it!
    Q_ASSERT(move.color == m_computerColor);
    kDebug() << "Computer plays ("<<move.row<<","<<move.col<<")" <<endl;
    makeMove(move);
}

void KReversiGame::makeMove( const KReversiMove& move )
{
    m_changedChips.clear();

    m_board->setChipColor( move.row, move.col, move.color );
    // the first one is the move itself
    m_changedChips.append( move );
    // now turn color of all chips that were won
    if( hasChunk( Up, move ) )
    {
        for(int r=move.row-1; r >= 0; --r)
        {
            if( m_board->chipColorAt( r, move.col ) == move.color )
                break;
            m_board->setChipColor( r, move.col, move.color );
            m_changedChips.append( KReversiMove( move.color, r, move.col ) );
        }
    }
    if( hasChunk( Down, move ) )
    {
        for(int r=move.row+1; r < 8; ++r)
        {
            if( m_board->chipColorAt( r, move.col ) == move.color )
                break;
            m_board->setChipColor( r, move.col, move.color );
            m_changedChips.append( KReversiMove( move.color, r, move.col ) );
        }
    }
    if( hasChunk( Left, move ) )
    {
        for(int c=move.col-1; c >= 0; --c)
        {
            if( m_board->chipColorAt( move.row, c ) == move.color )
                break;
            m_board->setChipColor( move.row, c, move.color );
            m_changedChips.append( KReversiMove( move.color, move.row, c ) );
        }
    }
    if( hasChunk( Right, move ) )
    {
        for(int c=move.col+1; c < 8; ++c)
        {
            if( m_board->chipColorAt( move.row, c ) == move.color )
                break;
            m_board->setChipColor( move.row, c, move.color );
            m_changedChips.append( KReversiMove( move.color, move.row, c ) );
        }
    }
    if( hasChunk( UpLeft, move ) )
    {
        for(int r=move.row-1, c=move.col-1; r>=0 && c >= 0; --r, --c)
        {
            if( m_board->chipColorAt( r, c ) == move.color )
                break;
            m_board->setChipColor( r, c, move.color );
            m_changedChips.append( KReversiMove( move.color, r, c ) );
        }
    }
    if( hasChunk( UpRight, move ) )
    {
        for(int r=move.row-1, c=move.col+1; r>=0 && c < 8; --r, ++c)
        {
            if( m_board->chipColorAt( r, c ) == move.color )
                break;
            m_board->setChipColor( r, c, move.color );
            m_changedChips.append( KReversiMove( move.color, r, c ) );
        }
    }
    if( hasChunk( DownLeft, move ) )
    {
        for(int r=move.row+1, c=move.col-1; r < 8 && c >= 0; ++r, --c)
        {
            if( m_board->chipColorAt( r, c ) == move.color )
                break;
            m_board->setChipColor( r, c, move.color );
            m_changedChips.append( KReversiMove( move.color, r, c ) );
        }
    }
    if( hasChunk( DownRight, move ) )
    {
        for(int r=move.row+1, c=move.col+1; r < 8 && c < 8; ++r, ++c)
        {
            if( m_board->chipColorAt( r, c ) == move.color )
                break;
            m_board->setChipColor( r, c, move.color );
            m_changedChips.append( KReversiMove( move.color, r, c ) );
        }
    }

    m_curPlayer = (m_curPlayer == White ? Black : White );
    kDebug() << "Now " << (m_curPlayer == White ? "White" : "Black" )<< " play" << endl;
    // FIXME dimsuz: is it needed at all?
    //emit boardChanged();
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
            else if(color == m_curPlayer)
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
            else if(color == m_curPlayer)
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
            else if(color == m_curPlayer)
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
            else if(color == m_curPlayer)
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
            else if(color == m_curPlayer)
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
            else if(color == m_curPlayer)
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
            else if(color == m_curPlayer)
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
            else if(color == m_curPlayer)
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

const KReversiBoard& KReversiGame::board() const
{
    return *m_board;
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
