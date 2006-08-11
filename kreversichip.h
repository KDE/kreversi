#ifndef KREVERSI_CHIP_H
#define KREVERSI_CHIP_H

#include <QGraphicsPixmapItem>
// this is to get ChipColor declaration.
// FIXME dimsuz: Move it to some common .h? (as it won't be the only h) where it is needed
#include "kreversiboard.h"

class QGraphicsScene;

class KReversiChip : public QGraphicsPixmapItem
{
public:
    KReversiChip( ChipColor color, QGraphicsScene *scene );
    void setColor( ChipColor color );
};
#endif
