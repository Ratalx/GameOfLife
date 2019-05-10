#include"../include/GameOfLifeLogic.hpp"
#include<iostream>
#include"../include/GameOfLifeRenderer.hpp"
namespace GameOfLife{
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

GameOfLifeLogic::GameOfLifeLogic(int sizeOfRow,const  std::vector< int> & cellsIndexes)
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

void GameOfLifeLogic::UpadateCells(std::vector<Point> cellsToAdd)
{   
    cells2 = cells;
    for(int i =0;i<cells2.size();++i)
    {
            nextI = (i+1) % (cells2.size());
            prevI = i==0?(cells2.size()-1):i-1;

        for(int j =0;j<cells2[i].size();++j)
        {
                int sum = 0;
                int nextJ= (j+1) % (cells2[i].size());
                int prevJ = j==0?(cells2[i].size()-1):j-1;

            sum+=static_cast<int>(cells2[i][prevJ]);       
            sum+=static_cast<int>(cells2[prevI][j]);       
            sum+=static_cast<int>(cells2[prevI][prevJ]);
            sum+=static_cast<int>(cells2[i][nextJ]);
            sum+=static_cast<int>(cells2[nextI][j]);
            sum+=static_cast<int>(cells2[nextI][nextJ]);
            sum+=static_cast<int>(cells2[prevI][nextJ]);
            sum+=static_cast<int>(cells2[nextI][prevJ]);
            
            if(cells[i][j]==Cell::dead&&sum==3)
                cells[i][j]=Cell::life;
            else if(cells[i][j]==Cell::life&&(sum==2||sum==3))
                cells[i][j]=Cell::life;
            else 
                cells[i][j]=Cell::dead;
        }
    }
    while (cellsToAdd.size()>0)
    {
        auto cellIndexes = cellsToAdd.back();
        cellsToAdd.pop_back();
        cells[cellIndexes.y][cellIndexes.x]=Cell::life;
    }
    
}
void GameOfLifeLogic::TogleCells(int x, int y)
{
    cells[x][y]= Cell::life;
    cells[x][y]=static_cast<bool>(cells[x][y]) ? Cell::life : Cell::dead;
}
}