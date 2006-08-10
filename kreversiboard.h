#ifndef KREVERSI_BOARD_H
#define KREVERSI_BOARD_H

#include <QVector>

// noColor = empty
enum ChipColor { NoColor, White, Black };

/**
 *  This class represents a reversi board.
 *  It only holds a current state of the game, 
 *  i.e. what type of chip the cells contain - nothing more.
 *  So this is just a plain QVector with some nice additions.
 */
class KReversiBoard
{
public:
    KReversiBoard();
    /**
     *  Resets board to initial (start-of-new-game) state
     */
    void resetBoard();
    /**
     *  Sets the type of chip at (row,col)
     */
    void setChipColor(int row, int col, ChipColor type);
    /**
     *  Retruns type of the chip in cell at (row, col)
     */
    ChipColor chipColorAt(int row, int col) const;
private:
    ChipColor m_cells[8][8];
};

struct KReversiMove
{
    KReversiMove( ChipColor col = NoColor, int r = -1, int c = -1 )
        : color(col), row(r), col(c) { }
    ChipColor color;
    int row;
    int col;
};

#endif
