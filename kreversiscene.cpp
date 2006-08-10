#include "kreversiscene.h"

KReversiScene::KReversiScene( KReversiGame* game )
{
    setGame(game);
}

void KReversiScene::setGame( KReversiGame* game )
{
    m_game = game;
}
