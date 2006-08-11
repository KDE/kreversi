#include "kreversiview.h"

#include <QGraphicsScene>

#include <kdebug.h>

const int BOARD_SIZE = 8;
// FIXME dimsuz: something to remove/give-more-thinking
const int CHIP_SIZE = 32;

KReversiView::KReversiView( QGraphicsScene* scene, QWidget *parent )
    : QGraphicsView(scene, parent)
{
    scene->setSceneRect( 0, 0, CHIP_SIZE*BOARD_SIZE, CHIP_SIZE*BOARD_SIZE );
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //setFixedSize( sceneRect().toRect().size() );
}

/* 
QSize KReversiView::sizeHint() const
{
    return QSize( scene()->width()+8, scene()->height()+8 );
}
 */
