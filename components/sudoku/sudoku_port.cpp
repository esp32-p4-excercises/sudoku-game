#include "freertos/FreeRTOS.h"

#include "esp_random.h"

#include "esp_sudoku_port.h"

using namespace std;

bool FindUnassignedLocation(int grid[9][9], int &row, int &col);

bool EspSudoku::finished()
{
	int x, y;
	return !FindUnassignedLocation(grid, x, y);
}

void EspSudoku::genDifficulty(int lvl)
{
	do
	{
		auto rnd = esp_random() % 81;
		auto x = rnd / 9;
		auto y = rnd % 9;
		int temp = this->grid[x][y];
		this->grid[x][y] = UNASSIGNED;

		// If now more than 1 solution , replace the removed cell back.
		int check = 0;
		countSoln(check);
		if (check != 1)
		{
			this->grid[x][y] = temp;
		}
		calculateDifficulty();
        // vTaskDelay(1);
	} while (difficultyLevel < lvl);
}

int EspSudoku::countNumber(int num)
{
    int count = 0;
    for (size_t i = 0; i < 9; i++)
    {
        for (size_t j = 0; j < 9; j++)
        {
            if (grid[i][j] == num)
            {
                count++;
            }
            
        }
    }
    return count;
}
