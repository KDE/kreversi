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
    // FIXME dimsuz: let it be so at the beginning
    setFixedSize(sizeHint());
}

QSize KReversiView::sizeHint() const
{
    kDebug() << "sceneRect:" << sceneRect() << endl;
    QRect r = mapFromScene(sceneRect()).boundingRect();
    kDebug() << "viewRect:" << r << endl;
    return QSize(r.size());
}
