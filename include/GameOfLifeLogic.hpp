#pragma once

#include<vector>

enum class Cell : int {dead=0,life=1};

namespace GameOfLife
{
struct Point;

class GameOfLifeLogic
{
public:
    std::vector<std::vector<Cell>> cells; 
    std::vector<std::vector<Cell>> cells2; 

    GameOfLifeLogic() = delete;
    GameOfLifeLogic(int sizeOfRow); 
    GameOfLifeLogic(std::vector<std::vector<Cell>> cells): cells(cells){};
    GameOfLifeLogic(int sizeOfRow, const std::vector< int> &cellsIndexes);
    void UpadateCells(std::vector<Point> cellsToAdd);
    void TogleCells(int x, int y);

};
} //namespace GameOfLife
