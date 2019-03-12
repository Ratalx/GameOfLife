#include"../include/GameOfLifeLogic.h"

GameOfLifeLogic::GameOfLifeLogic(int sizeOfRow)
{
    cells.resize(sizeOfRow);
    for(int i=0;i<sizeOfRow;++i)
    {
        for(int j=0;j<sizeOfRow;++j)
        {
            cells[i].push_back(Cell::dead);
        }
    }
}

GameOfLifeLogic::GameOfLifeLogic(int sizeOfRow,const  std::vector<int> & cellsIndexes)
{
    cells.resize(sizeOfRow);

    for(int i=0;i<sizeOfRow;++i)
    {
        for(int j=0;j<sizeOfRow;++j)
        {
            cells[i].push_back(Cell::dead);
        }
    }

    for(auto cellIndex : cellsIndexes)
    {
        int i = cellIndex / sizeOfRow;
        int j = cellIndex % sizeOfRow;
        cells[i][j] = Cell::life;
    }
}

void GameOfLifeLogic::UpadateCells()
{   
    std::vector cells2 = cells;
    for(int i =0;i<cells2.size();++i)
    {

    for(int j =0;j<cells2[i].size();++j)
    {
            int sum = 0;

        if(j != 0)
        sum+=static_cast<int>(cells2[i][j-1]);       
        if(i != 0)                                  
        sum+=static_cast<int>(cells2[i-1][j]);       
        if(j != 0&&i!=0)
        sum+=static_cast<int>(cells2[i-1][j-1]);
        if(j!=cells2[i].size()-1)
        sum+=static_cast<int>(cells2[i][j+1]);
        if(i!=cells2.size()-1)
        sum+=static_cast<int>(cells2[i+1][j]);
        if(i!=cells2.size()-1&&j!=cells2[i].size()-1)
        sum+=static_cast<int>(cells2[i+1][j+1]);
        if(i!=0&&j!=cells2[i].size())
        sum+=static_cast<int>(cells2[i-1][j+1]);
        if(i!=cells2.size()-1&&j!=0)
        sum+=static_cast<int>(cells2[i+1][j-1]);
        
        if(cells[i][j]==Cell::dead&&sum==3)
            cells[i][j]=Cell::life;
        else if(cells[i][j]==Cell::life&&(sum==2||sum==3))
            cells[i][j]=Cell::life;
        else 
            cells[i][j]=Cell::dead;
    }}

}
