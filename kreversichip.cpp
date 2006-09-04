#include "kreversichip.h"
#include <QPainter>
#include <QPixmap>

#include <kdebug.h>

KReversiChip::KReversiChip( ChipColor color, const KReversiChipFrameSet* frameSet, QGraphicsScene* scene )
    : QGraphicsPixmapItem( 0, scene ), m_color(color), m_frameSet(frameSet), m_curFrame(0)
{
    setColor(m_color);
}

void KReversiChip::setColor( ChipColor color )
{
    m_color = color;
    setPixmap( m_frameSet->chipPixmap( m_color ) );
}

bool KReversiChip::nextFrame()
{
    setPixmap( m_frameSet->frame( m_color, m_curFrame++ ) );
    bool finished = (m_curFrame == m_frameSet->frameCount());
    if(finished)
        m_curFrame = 0;
    return finished;
}

void KReversiChip::showLastMoveMarker(bool show)
{
    if(show)
    {
        QPixmap origPix = pixmap();
        QPixmap pix = origPix;
        QPainter p(&pix);
        p.fillRect( 1, 1, pix.width()-2, pix.height()-2, Qt::gray );
        p.drawPixmap(0,0, origPix);
        p.end();
        setPixmap(pix);
    }
    else
        setPixmap( m_frameSet->chipPixmap( m_color ) );
}

// -------------------------------------------------------------------------------

KReversiChipFrameSet::KReversiChipFrameSet( const QPixmap& allFrames, int frameSize )
{
    // we skip x,y = (0,0) case, because allFrames has a transparent frame as the 
    // the first one. It has it just for symmetry I guess (so the whole big pix remains square)
    for(int y=0; y < allFrames.height(); y += frameSize )
        for(int x=0; x < allFrames.width(); x += frameSize )
        {
            if( x==0 && y==0 ) continue;
            m_frames.append( allFrames.copy(x,y, frameSize, frameSize) );
        }
}

QPixmap KReversiChipFrameSet::frame( ChipColor color, int frameNo ) const
{
    int idx = 0;
    if( color == White )
        idx = m_frames.count() - frameNo - 1;
    else if(color == Black)
        idx = frameNo;

    Q_ASSERT( idx >= 0 && idx < m_frames.count() );

    return m_frames.at( idx );
}

QPixmap KReversiChipFrameSet::chipPixmap( ChipColor color ) const
{
    return ( color == White ? m_frames.at(frameCount()-1) : m_frames.at(0) );
}
