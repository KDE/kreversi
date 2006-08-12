#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#include <kdebug.h>

#include "kreversiscene.h"
#include "kreversigame.h"
#include "kreversichip.h"


// FIXME dimsuz: something to remove/give-more-thinking
const int CHIP_SIZE = 32;

KReversiScene::KReversiScene( KReversiGame* game )
{
    setBackgroundBrush( Qt::lightGray );

    m_boardRect = QRectF(5, 5, CHIP_SIZE*8, CHIP_SIZE*8);
    setSceneRect( 0, 0, m_boardRect.width()+10, m_boardRect.height()+10);

    setGame(game);
}

void KReversiScene::setGame( KReversiGame* game )
{
    m_game = game;
    connect( m_game, SIGNAL(boardChanged()), this, SLOT(updateBoard()) );

    // this will remove all items left from previous game
    QList<QGraphicsItem*> allChips = items( m_boardRect );
    foreach( QGraphicsItem* chip, allChips )
    {
        removeItem( chip );
        delete chip;
    }

    updateBoard();
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
                        kDebug() << "Found item at (" << row << "," << col << "). Setting its color." << endl;
                        chip->setColor( color );
                    }
                }
                else
                {
                    kDebug() << "No item at (" << row << "," << col << "). Creating." << endl;
                    chip = new KReversiChip( color, this );
                    chip->setPos( cellTopLeft(row, col) );
                }
            }
        }
}

QPointF KReversiScene::cellCenter( int row, int col ) const
{
    return QPointF( m_boardRect.x() + col*CHIP_SIZE + CHIP_SIZE/2, m_boardRect.y() + row*CHIP_SIZE + CHIP_SIZE/2 );
}

QPointF KReversiScene::cellTopLeft( int row, int col ) const
{
    return QPointF( m_boardRect.x() + col*CHIP_SIZE, m_boardRect.y() + row*CHIP_SIZE );
}

void KReversiScene::drawBackground( QPainter *p, const QRectF& )
{
    QPen pen(Qt::black);
    pen.setWidth(2);

    p->setPen(pen);

    qreal startx = m_boardRect.x();
    qreal starty = m_boardRect.y();
    qreal endx = m_boardRect.x() + m_boardRect.width();
    qreal endy = m_boardRect.y() + m_boardRect.height();
    
    for(qreal x=m_boardRect.x(); x<=endx; x+=CHIP_SIZE)
        p->drawLine( QPointF(x, starty), QPointF(x, endy) );
    for(qreal y=m_boardRect.y(); y<=endy; y+=CHIP_SIZE)
        p->drawLine( QPointF(startx, y), QPointF(endx, y) );
}

void KReversiScene::mousePressEvent( QGraphicsSceneMouseEvent* ev )
{
    if( !m_boardRect.contains(ev->scenePos()) )
        return;
    int row = (int)ev->scenePos().y() / CHIP_SIZE;
    int col = (int)ev->scenePos().x() / CHIP_SIZE;
    
    kDebug() << "Cell (" << row << "," << col << ") clicked." << endl;

    m_game->putChipAt( row, col );
}

#include "kreversiscene.moc"
