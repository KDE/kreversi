#ifndef KREVERSI_BOARD_H
#define KREVERSI_BOARD_H

// noColor = empty
enum ChipColor { White = 0, Black = 1, NoColor = 2 };

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
    /**
     *  Returns number of chips owned by this player
     */
    int playerScore( ChipColor player ) const;
private:
    /**
     *  The board itself
     */
    ChipColor m_cells[8][8];
    /**
     *  Score of each player
     */
    int m_score[2];
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
