#include "kreversiplayer.h"

KReversiPlayer::KReversiPlayer(ChipColor color):
    m_state(UNKNOWN), m_color(color)
{
}

ChipColor KReversiPlayer::getColor() const
{
    return m_color;
}
