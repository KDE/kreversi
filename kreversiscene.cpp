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
#include "kreversiscene.h"
#include "kreversigame.h"
#include "kreversichip.h"
#include "kreversirenderer.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QTimer>

#include <kdebug.h>
#include <KLocale>
#include <KGamePopupItem>

KReversiScene::KReversiScene( KReversiGame* game , const QString& chipsPath )
    : m_game(0), m_pendingNewGame(0), m_frameSet(0), m_hintChip(0), m_lastMoveChip(0), m_timerDelay(25),
    m_showingHint(false), m_demoMode(false), m_showLastMove(false), m_showPossibleMoves(false),
    m_showLabels(false)
{
    m_messageItem = new KGamePopupItem();
    m_messageItem->setMessageOpacity(0.9);
    addItem(m_messageItem);

    setChipsPrefix(chipsPath);

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
    kDebug() << "resizeScene" << width << "x" << height;
    setSceneRect( 0, 0, width, height );

    int size = qMin(width, height);
    m_boardRect.setRect( width/2 - size/2, height/2 - size/2, size, size );

    // board is square so no matter what to use - width or height
    int defWidth = KReversiRenderer::self()->defaultBoardSize().width();
    qreal scale = (qreal)size / defWidth;

    if(scale <=0)
        return;

    m_curCellSize = defWidth * scale / 10;

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

    // Render possible moves pixmap
    QImage baseImg((int)m_curCellSize, (int)m_curCellSize, QImage::Format_ARGB32_Premultiplied);
    QRectF moveRect(0,0,m_curCellSize,m_curCellSize);
    baseImg.fill(0);
    QPainter p(&baseImg);
    KReversiRenderer::self()->renderPossibleMove( &p, moveRect );
    p.end();
    m_possMovePix = QPixmap::fromImage(baseImg);

    //recalc possible moves items rects
    foreach( QGraphicsPixmapItem* item, m_possibleMovesItems )
        item->setPixmap( m_possMovePix );
    // and reposition them according to new cell sizes
    displayLastAndPossibleMoves();
}

void KReversiScene::setChipsPrefix( const QString& chipsPrefix )
{
    if(!m_frameSet) // this is a first invocation
    {
        m_frameSet = new KReversiChipFrameSet();
        m_frameSet->switchChipSet( chipsPrefix );
        m_curCellSize = m_frameSet->defaultChipSize();
    }
    else // we're changing frameset's pixmap (monochrome-chips <-> color-chips transition)
    {
        // m_curCellSize is already defined in this case, so lets scale chips on load
        m_frameSet->switchChipSet( chipsPrefix, (int)m_curCellSize );
    }

    if(m_game)
    {
        QList<QGraphicsItem*> allItems = items(m_boardRect);
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
    // if animation is running or we are thinking save this game in pending var.
    // It will be made current animation slot will get called
    // @see setNewGameObject and @see slotAnimationStep
    //
    // NOTE: all this magic is needed for game not to crash while pressing new game:
    // if we'd simply set the new object right away and deleted an old one, slotAnimationStep
    // might got called *after* this, trying to do smth with already deleted object.
    // For example see BUG #154946
    // So we postpone the setting && deletion...
    if( m_animTimer->isActive() || ( m_game && m_game->isThinking() ) )
        m_pendingNewGame = game;
    else
        setNewGameObject( game );
}

void KReversiScene::setNewGameObject( KReversiGame* game )
{
    m_animTimer->stop();

    // disconnect signals from previous game if it exists,
    // we are not interested in them anymore
    if( m_game )
    {
        disconnect( m_game, SIGNAL(boardChanged()), this, SLOT(updateBoard()) );
        disconnect( m_game, SIGNAL(moveFinished()), this, SLOT(slotGameMoveFinished()) );
        disconnect( m_game, SIGNAL(gameOver()), this, SLOT(slotGameOver()) );
        disconnect( m_game, SIGNAL(computerCantMove()), this, SLOT(slotComputerCantMove()) );
        disconnect( m_game, SIGNAL(playerCantMove()), this, SLOT(slotPlayerCantMove()) );
    }

    // delete old object
    delete m_game;	

    m_game = game;

    m_pendingNewGame = 0; // it's made official now ;)

    connect( m_game, SIGNAL(boardChanged()), SLOT(updateBoard()) );
    connect( m_game, SIGNAL(moveFinished()), SLOT(slotGameMoveFinished()) );
    connect( m_game, SIGNAL(gameOver()), SLOT(slotGameOver()) );
    connect( m_game, SIGNAL(computerCantMove()), SLOT(slotComputerCantMove()) );
    connect( m_game, SIGNAL(playerCantMove()), SLOT(slotPlayerCantMove()) );

    // this will remove all chips left from previous game
    QList<QGraphicsItem*> allItems = items();
    KReversiChip* chip;
    foreach( QGraphicsItem* item, allItems )
    {
        chip = qgraphicsitem_cast<KReversiChip*>(item);
        if(chip)
        {
            removeItem( chip );
            delete chip;
        }
    }

    m_possibleMovesItems.clear();

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
        foreach( QGraphicsPixmapItem* item, m_possibleMovesItems )
            item->hide();
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
                        //kDebug() << "Found item at (" << row << "," << col << "). Setting its color.";
                        chip->setColor( color );
                    }
                }
                else
                {
                    //kDebug() << "No item at (" << row << "," << col << "). Creating.";
                    chip = new KReversiChip( color, m_frameSet, this );
                    chip->setPos( cellTopLeft(row, col) );
                    chip->setRowCol( row, col );
                }
            }
            else
            {
                // this if-branch happens on undos

                // deleting only KReversiChips
                // (for other QGItems cast will return 0 causing "delete" to be no-op)
                KReversiChip *chip = qgraphicsitem_cast<KReversiChip*>(itemAt( cellCenter(row, col) ));
                delete chip;
            }
        }
    m_lastMoveChip = 0;
    displayLastAndPossibleMoves();
    update();
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
        foreach( QGraphicsPixmapItem* item, m_possibleMovesItems )
            item->hide();
    }

    m_changedChips = m_game->changedChips();
    // create an item that was placed by player
    // by convention it will be the first in the list of changed items
    KReversiPos move = m_changedChips.takeFirst();
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
    if( m_pendingNewGame != 0 )
    {
	// aha! we have new game waiting for us
	m_animTimer->stop();
	setNewGameObject( m_pendingNewGame );
	return;
    }

    if(m_changedChips.count() == 0 && !m_showingHint)
    {
        // FIXME: GGZ only - doesn't yet report flipped chips
        m_animTimer->stop();
        emit moveFinished();
        return;
    }

    if( !m_showingHint )
    { // we're animating chips move

        KReversiPos move = m_changedChips.at(0);
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
        KReversiPos lastPos = m_game->getLastMove();
        m_lastMoveChip = qgraphicsitem_cast<KReversiChip*>(itemAt(cellCenter(lastPos.row, lastPos.col)));
        if(m_lastMoveChip)
            m_lastMoveChip->showLastMoveMarker(true);
    }

    // ==== Show Possible Moves ====
    if( m_showPossibleMoves && !m_game->isComputersTurn() )
    {
        //hide currently displayed if any
        foreach( QGraphicsPixmapItem* item, m_possibleMovesItems )
            item->hide();

        PosList l = m_game->possibleMoves();
        // if m_possibleMovesItems contains less rects then there are items in l
        // lets fill it with additional rects.
        // Else we'll just reuse rects that we already have.
        // NOTE: maybe make m_possibleMovesItems a QVector and simply do resize()?
        if( m_possibleMovesItems.count() < l.count() )
        {
            int numtoadd = l.count() - m_possibleMovesItems.count();
            for( int i=0; i<numtoadd; ++i)
            {
                QGraphicsPixmapItem *item = new QGraphicsPixmapItem( m_possMovePix, 0, this );
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
        kDebug() << "It is not a very good time to ask me for a hint, human. I'm thinking...";
        return;
    }
    if( m_animTimer->isActive() )
    {
        kDebug() << "Don't you see I'm animating? Be patient, human child...";
        return;
    }
    KReversiPos hint = m_game->getHint();
    if( !hint.isValid() )
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
    return m_boardRect.topLeft()+QPointF( (col+1) * m_curCellSize, (row+1) * m_curCellSize );
}

void KReversiScene::drawBackground( QPainter *p, const QRectF&)
{
    KReversiRenderer::self()->renderBackground( p, sceneRect() );
    KReversiRenderer::self()->renderBoard(p, m_boardRect);
    if(m_showLabels)
        KReversiRenderer::self()->renderBoardLabels(p, m_boardRect);
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

    // user moves not allowed in demo mode
    if( m_demoMode )
        return;

    if( m_animTimer->isActive() || m_game->isComputersTurn() )
    {
        kDebug() << "Don't you see I'm busy? Be patient, human child...";
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

    m_game->makePlayerMove( row, col, false );
}

void KReversiScene::slotComputerCantMove()
{
    m_messageItem->setMessageTimeout(3000);
    m_messageItem->showMessage(i18n("Computer can not move. It is your turn again."), KGamePopupItem::BottomLeft);
}

void KReversiScene::slotPlayerCantMove()
{
    m_messageItem->setMessageTimeout(3000);
    m_messageItem->showMessage(i18n("You can not perform any move. Computer makes next turn now."), KGamePopupItem::BottomLeft);
}

#include "kreversiscene.moc"
