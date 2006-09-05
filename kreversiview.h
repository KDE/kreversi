#ifndef KREVERSI_VIEW_H
#define KREVERSI_VIEW_H

#include <QGraphicsView>

class KReversiView : public QGraphicsView
{
public:
    KReversiView( QGraphicsScene* scene, QWidget *parent );
private:
    virtual QSize sizeHint() const;
};
#endif
