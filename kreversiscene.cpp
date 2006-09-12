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
#include <kstandarddirs.h>

#include "kreversiscene.h"
#include "kreversigame.h"
#include "kreversichip.h"


// something to remove/give-more-thinking
const int CHIP_SIZE = 36;

KReversiScene::KReversiScene( KReversiGame* game , const QPixmap& chipsPixmap )
    : m_game(0), m_frameSet(0), m_hintChip(0), m_lastMoveChip(0), m_timerDelay(25), 
    m_showingHint(false), m_demoMode(false), m_showLastMove(false), m_showPossibleMoves(false)
{
    setBackgroundBrush( Qt::lightGray );

    QFont font; // it'll be initialised to default application font
    font.setBold(true);
    // NOTE we assume that fontMetrics in drawBackground() will be the same as here
    int fontHeight = QFontMetrics(font).height();

    m_boardRect = QRectF(fontHeight, fontHeight, CHIP_SIZE*8, CHIP_SIZE*8);

    setSceneRect( 0, 0, m_boardRect.width()+2*fontHeight, m_boardRect.height()+2*fontHeight);

    setChipsPixmap(chipsPixmap);

    m_animTimer = new QTimer(this);
    connect(m_animTimer, SIGNAL(timeout()), SLOT(slotAnimationStep()));

    setGame(game);
}

KReversiScene::~KReversiScene()
{
    delete m_frameSet;
}

void KReversiScene::setChipsPixmap( const QPixmap& chipsPixmap )
{
    delete m_frameSet;
    m_frameSet = new KReversiChipFrameSet( chipsPixmap, CHIP_SIZE );
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

void KReversiScene::setGame( KReversiGame* game )
{
    m_game = game;
    connect( m_game, SIGNAL(boardChanged()), SLOT(updateBoard()) );
    connect( m_game, SIGNAL(moveFinished()), SLOT(slotGameMoveFinished()) );
    connect( m_game, SIGNAL(gameOver()), SLOT(slotGameOver()) );

    // this will remove all items left from previous game
    QList<QGraphicsItem*> allChips = items( m_boardRect );
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
                }
            }
            else
            {
                // this if-branch happens on undos

                // FIXME dimsuz: qgraphicsitem_cast<...>(0) crashes.
                // Therefore I leave dynamic_cast here until this is fixed in qt
                // deleting only KReversiChips
                KReversiChip *chip = dynamic_cast<KReversiChip*>(itemAt( cellCenter(row, col) ));
                delete chip;
            }
        }
    m_lastMoveChip = 0;
    displayLastAndPossibleMoves();
}

void KReversiScene::toggleDemoMode( bool toggle )
{
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
                QGraphicsRectItem *item = new QGraphicsRectItem( 0, 0, CHIP_SIZE-1, CHIP_SIZE-1, 0, this );
                item->setBrush( Qt::darkGreen );
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
    m_showingHint = true;
    m_animTimer->start(500);
}

void KReversiScene::slotGameOver()
{
    m_demoMode = false;
}

QPointF KReversiScene::cellCenter( int row, int col ) const
{
    return QPointF( m_boardRect.x() + col*CHIP_SIZE + CHIP_SIZE/2, m_boardRect.y() + row*CHIP_SIZE + CHIP_SIZE/2 );
}

QPointF KReversiScene::cellTopLeft( int row, int col ) const
{
    return QPointF( m_boardRect.x() + col*CHIP_SIZE, m_boardRect.y() + row*CHIP_SIZE );
}

void KReversiScene::setBackgroundPixmap( const QPixmap& pix )
{
    m_bkgndPix = pix;
    update();
}

void KReversiScene::drawBackground( QPainter *p, const QRectF& rc)
{
    if(!m_bkgndPix.isNull())
    {
        p->setBrush( m_bkgndPix );
        p->drawRect( rc );
    }

    QPen pen(Qt::black);
    pen.setWidth(2);

    p->setPen(pen);

    qreal startx = m_boardRect.x();
    qreal starty = m_boardRect.y();
    qreal endx = m_boardRect.x() + m_boardRect.width();
    qreal endy = m_boardRect.y() + m_boardRect.height();
    
    for(qreal x=startx; x<=endx; x+=CHIP_SIZE)
        p->drawLine( QPointF(x, starty), QPointF(x, endy) );
    for(qreal y=starty; y<=endy; y+=CHIP_SIZE)
        p->drawLine( QPointF(startx, y), QPointF(endx, y) );

    QFont f = p->font();
    f.setBold(true);
    p->setFont(f);
    int fontHeight = p->fontMetrics().height();

    const char horLabels[] = "ABCDEFGH";
    const char verLabels[] = "12345678";

    QRect rect;
    // draw top+bottom labels
    for(int c=0; c<8;++c)
    {
        rect = QRect((int)startx+c*CHIP_SIZE, (int)starty-fontHeight, CHIP_SIZE, fontHeight );
        p->drawText( rect, Qt::AlignCenter | Qt::AlignTop, QChar(horLabels[c]) );
        rect.moveTop( (int)endy );
        p->drawText( rect, Qt::AlignCenter | Qt::AlignTop, QChar(horLabels[c]) );
    }
    // draw left+right labels
    for(int r=0; r<8;++r)
    {
        rect = QRect( (int)startx-fontHeight, (int)starty+r*CHIP_SIZE, fontHeight, CHIP_SIZE );
        p->drawText( rect, Qt::AlignCenter | Qt::AlignVCenter, QChar(verLabels[r]) );
        rect.moveLeft( (int)endx );
        p->drawText( rect, Qt::AlignCenter | Qt::AlignVCenter, QChar(verLabels[r]) );
    }
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

    if( !m_boardRect.contains(ev->scenePos()) )
        return;
    int row = (int)(-m_boardRect.y() + ev->scenePos().y()) / CHIP_SIZE;
    int col = (int)(-m_boardRect.x() + ev->scenePos().x()) / CHIP_SIZE;

    if( row < 0 ) row = 0;
    if( row > 7 ) row = 7;
    if( col < 0 ) col = 0;
    if( col > 7 ) col = 7;
    
    //kDebug() << "Cell (" << row << "," << col << ") clicked." << endl;

    m_game->makePlayerMove( row, col, false );
}

#include "kreversiscene.moc"
