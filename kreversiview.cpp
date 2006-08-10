#include "kreversiview.h"

#include <QGraphicsScene>

#include <kdebug.h>

const int BOARD_SIZE = 8;
// FIXME dimsuz: something to remove/give-more-thinking
const int CHIP_SIZE = 32;

KReversiView::KReversiView( QGraphicsScene* scene, QWidget *parent )
    : QGraphicsView(scene, parent)
{
    setRenderHints(QPainter::Antialiasing );
    scene->setSceneRect( 0, 0, CHIP_SIZE*BOARD_SIZE, CHIP_SIZE*BOARD_SIZE );
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void KReversiView::showEvent(QShowEvent*)
{
    // NOTE. it seems that QGV initializes it's margin only when
    // it has been shown. So we need to setSize here
    // Rude. It would be much better to calc the sizes in constructor.
    setFixedSize(sizeHint());
}

QSize KReversiView::sizeHint() const
{
    // QGraphicsView creates a 4px (as by time of writing) 
    // margin around the scene // and it's no way not to
    // hardcode it here, rather then this:
    //  we map scene's (0,0) to view coord-system and then use 
    // (for example) x coordinate of resulting point. 
    // That'll be the margin
    int margin = mapFromScene( 0, 0 ).x();
    return QSize(scene()->width()+(margin*2), scene()->height()+margin*2);
}
