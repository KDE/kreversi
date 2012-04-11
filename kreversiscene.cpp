/*******************************************************************
 *
 * Copyright 2006-2007 Dmitry Suzdalev <dimsuz@gmail.com>
 * Copyright 2010 Brian Croom <brian.s.croom@gmail.com>
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

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QTimer>

#include <kdebug.h>
#include <KLocale>
#include <KStandardDirs>
#include <KGamePopupItem>
#include <KgTheme>

static KgTheme* theme()
{
    KgTheme* theme = new KgTheme("pics/default_theme.desktop");
    theme->setGraphicsPath(KStandardDirs::locate("appdata", "pics/default_theme.svgz"));
    return theme;
}

KReversiScene::KReversiScene( KReversiGame* game , const QString& chipsPrefix )
    : m_renderer(theme()), m_game(0),
    m_pendingNewGame(0), m_hintChip(0), m_lastMoveChip(0), m_timerDelay(25),
    m_showingHint(false), m_demoMode(false), m_showLastMove(false), m_showPossibleMoves(false),
    m_showLabels(false)
{
    m_messageItem = new KGamePopupItem();
    m_messageItem->setMessageOpacity(0.9);
    addItem(m_messageItem);

    m_renderer.setFrameBaseIndex(1);
    m_curCellSize = qMin(width(), height()) / 10;
    KReversiChip::initLastMoveMarker(m_curCellSize);
    setChipsPrefix(chipsPrefix);

    m_animTimer = new QTimer(this);
    connect(m_animTimer, SIGNAL(timeout()), SLOT(slotAnimationStep()));

    setGame(game);
}

void KReversiScene::resizeScene( int width, int height )
{
    kDebug() << "resizeScene" << width << "x" << height;
    setSceneRect( 0, 0, width, height );

    int size = qMin(width, height);
    m_boardRect.setRect( width/2 - size/2, height/2 - size/2, size, size );

    m_curCellSize = size / 10.0;

    if(m_lastMoveChip)
        m_lastMoveChip->showLastMoveMarker(false);
    KReversiChip::initLastMoveMarker(m_curCellSize);
    if(m_lastMoveChip && m_showLastMove)
        m_lastMoveChip->showLastMoveMarker(true);

    // adopt new chip size
    QList<QGraphicsItem*> allItems = items();
    KReversiChip *chip = 0;
    foreach( QGraphicsItem* item, allItems )
    {
        chip = qgraphicsitem_cast<KReversiChip*>(item);
        if( chip )
        {
            // adjust pos to new one
            chip->setPos( cellTopLeft( chip->row(), chip->col() ) );
            chip->setChipSize( (int)m_curCellSize );
        }
    }

    foreach( KGameRenderedItem* item, m_possibleMovesItems )
        item->setRenderSize(QSize(m_curCellSize, m_curCellSize));
    displayLastAndPossibleMoves();
}

void KReversiScene::setChipsPrefix( const QString& chipsPrefix )
{
    m_chipsPrefix = chipsPrefix;
    if(m_game)
    {
        QList<QGraphicsItem*> allItems = items(m_boardRect);
        KReversiChip *chip = 0;
        foreach( QGraphicsItem* item, allItems )
        {
            chip = qgraphicsitem_cast<KReversiChip*>(item);
            if( chip )
                chip->setChipPrefix( chipsPrefix );
        }

        if(m_hintChip)
            m_hintChip->setChipPrefix( chipsPrefix );
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

    foreach( QGraphicsItem* item, m_possibleMovesItems )
    {
        removeItem( item );
        delete item;
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
        foreach( KGameRenderedItem* item, m_possibleMovesItems )
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
                    chip = new KReversiChip( &m_renderer, color, m_chipsPrefix, m_curCellSize, this );
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
        foreach( KGameRenderedItem* item, m_possibleMovesItems )
            item->hide();
    }

    m_changedChips = m_game->changedChips();
    // create an item that was placed by player
    // by convention it will be the first in the list of changed items
    KReversiPos move = m_changedChips.takeFirst();
    KReversiChip *newchip = new KReversiChip( &m_renderer, move.color, m_chipsPrefix, m_curCellSize, this );
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

    if(m_changedChips.isEmpty() && !m_showingHint)
    {
        m_animTimer->stop();
        emit moveFinished();
        return;
    }

    if( !m_showingHint )
    { // we're animating chips move

        KReversiPos move = m_changedChips.at(0);
        QPointF pos = cellCenter(move.row, move.col);
        KReversiChip *chip = qgraphicsitem_cast<KReversiChip*>(itemAt(pos));
        if (!chip)
        {
            kDebug() << "looks like pos" << move.row << "," << move.col << "is hovered by message item, searching below";
            QList<QGraphicsItem*> allItemsAtPos = items(pos);
            foreach (QGraphicsItem* item, allItemsAtPos)
            {
                chip = qgraphicsitem_cast<KReversiChip*>(item);
                if (chip)
                    break;
            }
            kDebug() << (chip ? "found it!" : "not found... skipping all animation frames and hoping for the best");
        }

        bool animFinished = chip ? chip->nextFrame() : true /* skip frames */;
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
        foreach( KGameRenderedItem* item, m_possibleMovesItems )
            item->hide();

        PosList l = m_game->possibleMoves();
        // if m_possibleMovesItems contains less rects then there are items in l
        // lets fill it with additional rects.
        // Else we'll just reuse rects that we already have.
        // NOTE: maybe make m_possibleMovesItems a QVector and simply do resize()?
        while( m_possibleMovesItems.count() < l.count() )
        {
            KGameRenderedItem *item = new KGameRenderedItem( &m_renderer, QLatin1String( "move_hint" ) );
            addItem(item);
            item->setRenderSize( QSize(m_curCellSize, m_curCellSize) );
            item->setZValue(-1);
            m_possibleMovesItems.append( item );
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
        m_hintChip = new KReversiChip( &m_renderer, hint.color, m_chipsPrefix, m_curCellSize, this );
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

void KReversiScene::drawBackground( QPainter *p, const QRectF& )
{
    p->drawPixmap(sceneRect().topLeft(), m_renderer.spritePixmap(QLatin1String( "background" ), sceneRect().size().toSize()));
    p->drawPixmap(m_boardRect.topLeft(), m_renderer.spritePixmap(QLatin1String( "board" ), m_boardRect.size().toSize()));
    if(m_showLabels)
        p->drawPixmap(m_boardRect.topLeft(), m_renderer.spritePixmap(QLatin1String( "board_numbers" ), m_boardRect.size().toSize()));
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

    displayLastAndPossibleMoves();
}

void KReversiScene::slotPlayerCantMove()
{
    m_messageItem->setMessageTimeout(3000);
    m_messageItem->showMessage(i18n("You can not perform any move. Computer takes next turn now."), KGamePopupItem::BottomLeft);

    displayLastAndPossibleMoves();
}

#include "kreversiscene.moc"
