#pragma once

#include <stdint.h>
#include <vector>
#include <map>

#include "esp_random.h"

#include "Sudoku-Generator/sudokuGen.hpp"

class EspSudoku : public Sudoku
{
protected:

public:
    using Sudoku::Sudoku;
    ~EspSudoku() {}

    bool finished();
    void genDifficulty(int lvl = 0);

    int countNumber(int num);
    int getValue(int x, int y)
    {
        return grid[x][y];
    }

    void set(int x, int y, int val)
    {
        grid[x][y] = val;
    }
    bool check(int x, int y, int val)
    {
        return solnGrid[x][y] == val;
    }
};

