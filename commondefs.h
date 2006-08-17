#ifndef COMMON_DEFINITIONS_H
#define COMMON_DEFINITIONS_H

// noColor = empty
enum ChipColor { White = 0, Black = 1, NoColor = 2 };

struct KReversiMove
{
    KReversiMove( ChipColor col = NoColor, int r = -1, int c = -1 )
        : color(col), row(r), col(c) { }
    ChipColor color;
    int row;
    int col;
};

typedef QList<KReversiMove> MoveList;

#endif
