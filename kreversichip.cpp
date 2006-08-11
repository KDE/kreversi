#include "kreversichip.h"
#include <QPainter>
#include <QPixmap>

KReversiChip::KReversiChip( ChipColor color, QGraphicsScene* scene )
    : QGraphicsPixmapItem( 0, scene )
{
    QPixmap pix(32, 32);
    pix.fill( Qt::white );
    QPainter p(&pix);
    p.setBrush( color == White ? Qt::white : Qt::black );
    p.drawEllipse( QRectF(0,0,32,32) );
    p.end();
    setPixmap( pix );
}

void KReversiChip::setColor( ChipColor color )
{
    QPixmap pix(32, 32);
    pix.fill( Qt::white );
    QPainter p(&pix);
    p.setBrush( color == White ? Qt::white : Qt::black );
    p.drawEllipse( QRectF(0,0,32,32) );
    p.end();
    setPixmap( pix );
}
