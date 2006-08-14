#include "kreversichip.h"
#include <QPainter>
#include <QPixmap>

#include <kdebug.h>

KReversiChip::KReversiChip( ChipColor color, const KReversiChipFrameSet* frameSet, QGraphicsScene* scene )
    : QGraphicsPixmapItem( 0, scene ), m_color(color), m_frameSet(frameSet)
{
    setColor(m_color);
}

void KReversiChip::setColor( ChipColor color )
{
    m_color = color;
    setPixmap( m_frameSet->chipPixmap( m_color ) );
}

KReversiChipFrameSet::KReversiChipFrameSet( const QPixmap& allFrames, int frameSize )
{
    // we skip x,y = (0,0) case, because allFrames has a transparent frame as the 
    // the first one. Just for symmetry I guess (so the pix remains square)
    for(int y=0; y < allFrames.height(); y += frameSize )
        for(int x=0; x < allFrames.width(); x += frameSize )
        {
            if( x==0 && y==0 ) continue;
            m_frames.append( allFrames.copy(x,y, frameSize, frameSize) );
        }
}

QPixmap KReversiChipFrameSet::frame( int frameNo ) const
{
    return m_frames.at(frameNo);
}

QPixmap KReversiChipFrameSet::chipPixmap( ChipColor color ) const
{
    return ( color == White ? m_frames.at(frameCount()-1) : m_frames.at(0) );
}
