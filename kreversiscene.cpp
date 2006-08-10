#include "kreversiscene.h"

#include <QGraphicsRectItem>

KReversiScene::KReversiScene( KReversiGame* game )
{
    setGame(game);
    setBackgroundBrush( Qt::green );
}

void KReversiScene::setGame( KReversiGame* game )
{
    QPen pen(Qt::red);
    //pen.setWidth(1);
    addRect( sceneRect(), pen )->show();
    m_game = game;
}
