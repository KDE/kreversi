#include "kreversiview.h"

#include <QGraphicsScene>

#include <kdebug.h>

KReversiView::KReversiView( QGraphicsScene* scene, QWidget *parent )
    : QGraphicsView(scene, parent)
{
    setCacheMode( QGraphicsView::CacheBackground );
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setMinimumSize( sizeHint() );
    resize( sizeHint() );
}

QSize KReversiView::sizeHint() const
{
    return QSize( (int)scene()->width(), (int)scene()->height() );
}
