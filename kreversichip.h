#ifndef KREVERSI_CHIP_H
#define KREVERSI_CHIP_H

#include <QGraphicsPixmapItem>
#include "commondefs.h"

class QGraphicsScene;

class KReversiChip : public QGraphicsPixmapItem
{
public:
    KReversiChip( ChipColor color, QGraphicsScene *scene );
    void setColor( ChipColor color );
    ChipColor color() const { return m_color; }
private:
    ChipColor m_color;
};
#endif
