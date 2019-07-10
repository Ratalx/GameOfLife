#include"../include/RleReader.hpp"
#include<vector>
#include<fstream>
#include<iostream>
#include<sstream>
namespace GameOfLife
{
using matrixOfCells = std::vector<std::vector<Cell>>;
matrixOfCells RleReader::GenerateStartVector()
{
    std::fstream file(filePath,file.in);
    std::string line, bigLine;
    matrixOfCells grid;
    if(!file.is_open())
    {
        std::cout<<"Rlefile not opened correctly\n";
        throw std::runtime_error("File Not Open");
    }
   
    while(getline(file,line)&&(line[0]=='#'));
    grid = MakeGrid(line);
    
    while(getline(file,line))
    {
       bigLine += line;
    }
    file.close();
    return MakeStartingPattern(grid,bigLine);
}

matrixOfCells RleReader::MakeGrid(const std::string& line)
{
    int x, y, max, sizeOfRow;
    char unusedChars;
    std::stringstream lineStream(line);
    lineStream >>unusedChars>>unusedChars >> x >>unusedChars>>unusedChars>>unusedChars>> y ;
    max = x > y ? x : y;
    sizeOfRow = 20;
    max /=20;

    while(max!=0)
    {
        max/=5;
        sizeOfRow*=5;
    }
    matrixOfCells grid;
    if(sizeOfRow/2+x > sizeOfRow)
    {
        sizeOfRow*=5;
    }
    grid.resize(sizeOfRow);
    for(int i=0;i<sizeOfRow;++i)
    {
        for(int j=0;j<sizeOfRow;++j)
        {
            grid[i].push_back(Cell::dead);
        }
    }
    gridIndex = (sizeOfRow*sizeOfRow)/2 + sizeOfRow/2;

    return grid;
}

matrixOfCells RleReader::MakeStartingPattern(matrixOfCells& grid, const std::string& line)
{   
    int sizeOfRow = grid.size();
    int startPoint = gridIndex;
    int y = startPoint / sizeOfRow;
    int x = startPoint % sizeOfRow;
    int count=0;

    for(unsigned int i =0;i<line.size();++i)
    {
        if(isNumber(line[i]))
        {   
            count*=10;
            count += static_cast<int>(line[i])-48;
        }
        else if(isNewLine(line[i]))
        {
            y++;
            x= startPoint % sizeOfRow;
            count =0;
        }
        else if(isEnd(line[i]))
        {
            break;
        }
        else
        {   
            if(count==0)
            {
                count=1;
            }
            for(int k =0;k<count;++k)
            {
                if(line[i]=='o')
                {
                    grid[y][x]=Cell::life;
                }
                x++;
            }
            count=0;
        }
    }
    return grid;
}

bool RleReader::isNumber(char c) const noexcept
{
    return (c>='0' && c<='9');
}

bool RleReader::isNewLine(char c) const noexcept
{
    return (c == '$');
}

bool RleReader::isEnd(char c) const noexcept
{
    return (c == '!');
}
} //namespace GameOfLife

