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
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QTimer>

#include <kdebug.h>
#include <ksvgrenderer.h>

#include "kreversiscene.h"
#include "kreversigame.h"
#include "kreversichip.h"

KReversiScene::KReversiScene( KReversiGame* game , const QString& chipsPath )
    : m_game(0), m_frameSet(0), m_hintChip(0), m_lastMoveChip(0), m_timerDelay(25), 
    m_showingHint(false), m_demoMode(false), m_showLastMove(false), m_showPossibleMoves(false),
    m_showLabels(false)
{
    m_bkgndRenderer = new KSvgRenderer(this);
    m_bkgndLabelsRenderer = new KSvgRenderer(this);

    setBackgroundBrush( Qt::lightGray );

    setChipsPixmap(chipsPath);

    resizeScene( (int)m_curCellSize*10, (int)m_curCellSize*10 );

    m_animTimer = new QTimer(this);
    connect(m_animTimer, SIGNAL(timeout()), SLOT(slotAnimationStep()));

    setGame(game);
}

KReversiScene::~KReversiScene()
{
    delete m_frameSet;
}

void KReversiScene::resizeScene( int width, int height )
{
    int size = qMin(width, height);
    setSceneRect( 0, 0, size, size );

    // bkgnd is square so no matter what to use - width or height
    qreal scale = (qreal)size / m_bkgndRenderer->defaultSize().width();
    // FIXME dimsuz: remove and find out where this comes from!
    if(scale <=0)
        return;

    m_curCellSize = m_bkgndRenderer->defaultSize().width() * scale / 10;

    // adopt to new chip size
    m_frameSet->setChipSize( (int)m_curCellSize );

    QList<QGraphicsItem*> allItems = items();
    KReversiChip *chip = 0;
    foreach( QGraphicsItem* item, allItems )
    {
        chip = qgraphicsitem_cast<KReversiChip*>(item);
        if( chip )
        {
            // adjust pos to new one
            chip->setPos( cellTopLeft( chip->row(), chip->col() ) );
            chip->setColor( chip->color() ); // this will reread pixmap
        }
    }
    //recalc possible moves items rects
    foreach( QGraphicsRectItem* rect, m_possibleMovesItems )
        rect->setRect( 1, 1, m_curCellSize-2, m_curCellSize-2 );
    // and reposition them according to new cell sizes
    displayLastAndPossibleMoves();
}

void KReversiScene::setChipsPixmap( const QString& chipsPath )
{
    if(!m_frameSet) // this is a first invocation
    {
        m_frameSet = new KReversiChipFrameSet();
        m_frameSet->loadFrames( chipsPath );
        m_curCellSize = m_frameSet->defaultChipSize();
    }
    else // we're changing frameset's pixmap (monochrome-chips <-> color-chips transition)
    {
        // m_curCellSize is already defined in this case, so lets scale chips on load
        m_frameSet->loadFrames( chipsPath, (int)m_curCellSize );
    }


    if(m_game)
    {
        // FIXME: Qt rc1 bug? there was items( m_boardRect ) here
        // but for some reason it returns only 2 white chips on initial board with 4 chips.
        // Check out one more time!
        QList<QGraphicsItem*> allItems = items();
        KReversiChip *chip = 0;
        foreach( QGraphicsItem* item, allItems )
        {
            chip = qgraphicsitem_cast<KReversiChip*>(item);
            if( chip )
            {
                chip->setFrameSet( m_frameSet );
                chip->setColor( chip->color() ); // this will reread pixmap
            }
        }

        if(m_hintChip)
        {
            m_hintChip->setFrameSet( m_frameSet );
            m_hintChip->setColor( m_hintChip->color() );
        }
    }
}

void KReversiScene::setShowBoardLabels( bool show )
{
    m_showLabels = show;
}

void KReversiScene::setGame( KReversiGame* game )
{
    m_game = game;
    connect( m_game, SIGNAL(boardChanged()), SLOT(updateBoard()) );
    connect( m_game, SIGNAL(moveFinished()), SLOT(slotGameMoveFinished()) );
    connect( m_game, SIGNAL(gameOver()), SLOT(slotGameOver()) );

    // this will remove all items left from previous game
    QList<QGraphicsItem*> allChips = items();
    foreach( QGraphicsItem* chip, allChips )
    {
        removeItem( chip );
        delete chip;
    }

    m_possibleMovesItems.clear();

    m_animTimer->stop();
    m_hintChip = 0; // it was deleted above if it was shown
    m_showingHint = false;
    m_lastMoveChip = 0;
    m_demoMode = false;

    updateBoard();
}

void KReversiScene::setShowLastMove( bool show )
{
    m_showLastMove = show;
    if(show)
        displayLastAndPossibleMoves();
    else
    {
        if(m_lastMoveChip)
            m_lastMoveChip->showLastMoveMarker(false);
    }
}

void KReversiScene::setShowLegalMoves( bool show )
{
    m_showPossibleMoves = show;
    if(show)
        displayLastAndPossibleMoves();
    else
    {
        // NOTE: or delete?
        foreach( QGraphicsRectItem* rect, m_possibleMovesItems )
            rect->hide();
    }
}

void KReversiScene::setAnimationSpeed(int speed)
{
    if( speed == 0 ) // slow
        m_timerDelay = 40;
    else if( speed == 1 ) // normal
        m_timerDelay = 25;
    else if( speed == 2 ) // fast
        m_timerDelay = 15;
}

bool KReversiScene::isBusy() const
{
    return m_animTimer->isActive();
}

void KReversiScene::updateBoard()
{
    for(int row=0; row<8; ++row)
        for(int col=0; col<8; ++col )
        {
            ChipColor color = m_game->chipColorAt( row, col );
            if( color != NoColor )
            {
                // if there's a chip, just change it color
                // otherwise create new
                KReversiChip *chip = static_cast<KReversiChip*>(itemAt( cellCenter(row, col) ));
                if( chip != 0 )
                {
                    if( chip->color() != color )
                    {
                        //kDebug() << "Found item at (" << row << "," << col << "). Setting its color." << endl;
                        chip->setColor( color );
                    }
                }
                else
                {
                    //kDebug() << "No item at (" << row << "," << col << "). Creating." << endl;
                    chip = new KReversiChip( color, m_frameSet, this );
                    chip->setPos( cellTopLeft(row, col) );
                    chip->setRowCol( row, col );
                }
            }
            else
            {
                // this if-branch happens on undos

                // deleting only KReversiChips
                KReversiChip *chip = qgraphicsitem_cast<KReversiChip*>(itemAt( cellCenter(row, col) ));
                delete chip;
            }
        }
    m_lastMoveChip = 0;
    displayLastAndPossibleMoves();
}

void KReversiScene::toggleDemoMode( bool toggle )
{
    if( m_game->isGameOver() )
        return;
    m_demoMode = toggle;
    stopHintAnimation();
    // if we are currently waiting for user mouse input and not animating,
    // let's do the turn right now!
    if( !m_game->isComputersTurn() && !m_animTimer->isActive() )
        m_game->startNextTurn(m_demoMode);
}

void KReversiScene::slotGameMoveFinished()
{
    // hide shown legal moves
    if( m_showPossibleMoves )
    {
        foreach( QGraphicsRectItem* rect, m_possibleMovesItems )
            rect->hide();
    }

    m_changedChips = m_game->changedChips();
    // create an item that was placed by player
    // by convention it will be the first in the list of changed items
    KReversiMove move = m_changedChips.takeFirst();
    KReversiChip *newchip = new KReversiChip( move.color, m_frameSet, this );
    newchip->setPos( cellTopLeft( move.row, move.col ) );
    newchip->setRowCol( move.row, move.col );
    // start animation
    if( m_lastMoveChip )
        m_lastMoveChip->showLastMoveMarker( false );
    m_animTimer->start(m_timerDelay);
}

void KReversiScene::slotAnimationStep()
{
    if( !m_showingHint )
    { // we're animating chips move

        KReversiMove move = m_changedChips.at(0);
        KReversiChip *chip = qgraphicsitem_cast<KReversiChip*>(itemAt( cellCenter(move.row, move.col) ));

        bool animFinished = chip->nextFrame();
        if(animFinished)
        {
            chip->setColor( move.color );

            m_changedChips.removeFirst(); // we finished animating it

            if(m_changedChips.count() == 0)
            {
                // whole animation sequence finished. On to next turn!
                m_animTimer->stop();
                emit moveFinished();

                displayLastAndPossibleMoves();

                m_game->startNextTurn(m_demoMode);
            }
        }
    }
    else
    { // we're just showing hint to the user
        m_hintChip->setVisible( !m_hintChip->isVisible() );
        update(m_hintChip->sceneBoundingRect());
    }
}

void KReversiScene::displayLastAndPossibleMoves()
{
    // ==== Show What Last Move Was ====
    if( m_showLastMove )
    {
        KReversiMove lastPos = m_game->getLastMove();
        m_lastMoveChip = qgraphicsitem_cast<KReversiChip*>(itemAt(cellCenter(lastPos.row, lastPos.col)));
        if(m_lastMoveChip)
            m_lastMoveChip->showLastMoveMarker(true);
    }

    // ==== Show Possible Moves ====
    if( m_showPossibleMoves && !m_game->isComputersTurn() )
    {
        //hide currently displayed if any
        foreach( QGraphicsRectItem* rect, m_possibleMovesItems )
            rect->hide();

        MoveList l = m_game->possibleMoves();
        // if m_possibleMovesItems contains less rects then there are items in l
        // lets fill it with additional rects.
        // Else we'll just reuse rects that we already have.
        // NOTE: maybe make m_possibleMovesItems a QVector and simply do resize()?
        if( m_possibleMovesItems.count() < l.count() )
        {
            int numtoadd = l.count() - m_possibleMovesItems.count();
            for( int i=0; i<numtoadd; ++i)
            {
                QGraphicsRectItem *item = new QGraphicsRectItem( 1, 1, m_curCellSize-2, m_curCellSize-2, 0, this );
                item->setBrush( Qt::darkGreen );
                item->setPen( Qt::NoPen );
                item->setZValue(-1);
                m_possibleMovesItems.append( item );
            }
        }

        // now let's setup rects to appropriate positions
        for(int i=0; i<l.size(); ++i )
        {
            m_possibleMovesItems[i]->setPos( cellTopLeft( l.at(i).row, l.at(i).col ) );
            m_possibleMovesItems[i]->show();
        }
    }
}

void KReversiScene::slotHint()
{
    if( m_game->isComputersTurn() )
    {
        kDebug() << "It is not a very good time to ask me for a hint, human. I'm thinking..." << endl;
        return;
    }
    if( m_animTimer->isActive() )
    {
        kDebug() << "Don't you see I'm animating? Be patient, human child..." << endl;
        return;
    }
    KReversiMove hint = m_game->getHint();
    if(hint.row == -1 || hint.col == -1)
        return;
    if( m_hintChip == 0 )
        m_hintChip = new KReversiChip( hint.color, m_frameSet, this );
    m_hintChip->setPos( cellTopLeft( hint.row, hint.col ) );
    m_hintChip->setRowCol( hint.row, hint.col );
    m_showingHint = true;
    m_animTimer->start(500);
}

void KReversiScene::slotGameOver()
{
    m_demoMode = false;
}

QPointF KReversiScene::cellCenter( int row, int col ) const
{
    return cellTopLeft(row,col) + QPointF( m_curCellSize / 2, m_curCellSize / 2 );
}

QPointF KReversiScene::cellTopLeft( int row, int col ) const
{
    return QPointF( (col+1) * m_curCellSize, (row+1) * m_curCellSize );
}

void KReversiScene::setBackground( const QString& bkgndPath, const QString& bkgndLabelsPath )
{
    m_bkgndRenderer->load( bkgndPath );
    m_bkgndLabelsRenderer->load( bkgndLabelsPath );
}

void KReversiScene::drawBackground( QPainter *p, const QRectF&)
{
    // we render on square, so if sceneRect() is not square let's adjust this
    QRectF sRect = sceneRect();
    int minSize = qMin( (int)sRect.width(), (int)sRect.height() );
    sRect.setWidth( minSize );
    sRect.setHeight( minSize );

    m_bkgndRenderer->render(p, sRect);
    if(m_showLabels)
        m_bkgndLabelsRenderer->render(p, sRect);
}

void KReversiScene::stopHintAnimation()
{
    if( m_animTimer->isActive() )
    {
        if( m_showingHint )
        {
            m_animTimer->stop();
            m_hintChip->hide();
            m_showingHint = false;
            update(m_hintChip->sceneBoundingRect());
        }
    }
}

void KReversiScene::mousePressEvent( QGraphicsSceneMouseEvent* ev )
{
    stopHintAnimation();
    if( m_animTimer->isActive() || m_game->isComputersTurn() )
    {
        kDebug() << "Don't you see I'm busy? Be patient, human child..." << endl;
        return;
    }

    QRectF boardRect( cellTopLeft(0, 0), QSizeF( m_curCellSize * 8, m_curCellSize * 8) );
    if( !boardRect.contains(ev->scenePos()) )
        return;
    int row = (int)((-boardRect.y() + ev->scenePos().y()) / m_curCellSize);
    int col = (int)((-boardRect.x() + ev->scenePos().x()) / m_curCellSize);

    if( row < 0 ) row = 0;
    if( row > 7 ) row = 7;
    if( col < 0 ) col = 0;
    if( col > 7 ) col = 7;
    
    //kDebug() << "Cell (" << row << "," << col << ") clicked." << endl;

    m_game->makePlayerMove( row, col, false );
}

#include "kreversiscene.moc"
