#include <QPainter>
#include <kdebug.h>

#include "kreversiscene.h"
#include "kreversigame.h"
#include "kreversichip.h"


// FIXME dimsuz: something to remove/give-more-thinking
const int CHIP_SIZE = 32;

KReversiScene::KReversiScene( KReversiGame* game )
{
    setGame(game);
    setBackgroundBrush( Qt::lightGray );
    setSceneRect( 0, 0, CHIP_SIZE*8+10, CHIP_SIZE*8+10);
}

void KReversiScene::setGame( KReversiGame* game )
{
    m_game = game;
    updateBoard();
}

void KReversiScene::updateBoard()
{
    for(int row=0; row<8; ++row)
        for(int col=0; col<8; ++col )
        {
            if( m_game->chipColorAt( row, col ) != NoColor )
            {
                // if there's a chip, just change it color
                // otherwise create new
                KReversiChip *chip = static_cast<KReversiChip*>(itemAt( cellCenter(row, col) ));
                if( chip != 0 )
                {
                    kDebug() << "Found item at (" << row << "," << col << "). Setting its color." << endl;
                    chip->setColor( m_game->chipColorAt( row, col ) );
                }
                else
                {
                    kDebug() << "No item at (" << row << "," << col << "). Creating." << endl;
                    chip = new KReversiChip( m_game->chipColorAt( row, col ), this );
                    chip->setPos( row*CHIP_SIZE, col*CHIP_SIZE );
                }
            }
        }
}

QPointF KReversiScene::cellCenter( int row, int col ) const
{
    return QPointF( row*CHIP_SIZE + CHIP_SIZE/2, col*CHIP_SIZE + CHIP_SIZE/2 );
}

void KReversiScene::drawBackground( QPainter *p, const QRectF& )
{
// NOTE: this code assumes that sceneRect.topLeft() is (0,0)
    int width = CHIP_SIZE*8;
    int height = CHIP_SIZE*8;
    
    QPen pen(Qt::black);
    pen.setWidth(2);
    //width += 0.5

    p->setPen(pen);

    for(int x=0; x<=width; x+=CHIP_SIZE)
        p->drawLine( x, 0, x, height );
    for(int y=0; y<=height; y+=CHIP_SIZE)
        p->drawLine( 0, y, width, y );
}

#include "kreversiscene.moc"
