#include<vector>
#include<iostream>
enum class Cell : int {dead=0,life=1};


class GameOfLifeLogic
{
    public:
    std::vector<std::vector<Cell>> cells; 
    std::vector<std::vector<Cell>> cells2; 
    unsigned nextI,nextJ,prevI,prevJ;
    GameOfLifeLogic()=delete;
    GameOfLifeLogic(int sizeOfRow); 
    GameOfLifeLogic(int sizeOfRow,const std::vector< int> &cellsIndexes);
    void UpadateCells();


};