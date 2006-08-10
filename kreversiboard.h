#ifndef KREVERSI_BOARD_H
#define KREVERSI_BOARD_H

#include <QVector>

/**
 *  This class represents a reversi board.
 *  It only holds a current state of the game, 
 *  i.e. what type of chip the cells contain - nothing more.
 *  So this is just a plain QVector with some nice additions.
 */
class KReversiBoard
{
public:
    enum ChipType { noChip, playerChip, computerChip };

    KReversiBoard(int boardSize=10);
    /**
     *  Resets board to initial (start-of-new-game) state
     */
    void resetBoard();
    /**
     *  Sets the type of chip at (row,col)
     */
    void setChip(int row, int col, ChipType type);
    /**
     *  Retruns type of the chip in cell at (row, col)
     */
    ChipType chipAt(int row, int col) const;
private:
    QVector<ChipType> m_cells;
    int m_boardSize;
};
#endif
