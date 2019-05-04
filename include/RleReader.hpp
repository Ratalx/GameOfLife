#pragma once
#include<string>
#include<istream>
#include"GameOfLifeLogic.hpp"
class RleReader {
    using matrixOfCells = std::vector<std::vector<Cell>>;
    public:

    RleReader(const std::string& filePath) : filePath(filePath){};
    std::vector<std::vector<Cell>>GenerateStartVector();

    private:
    std::string filePath;
    int gridIndex;
    matrixOfCells MakeGrid(const std::string&);
    matrixOfCells MakeStartingPattern(matrixOfCells&, const std::string&);
   inline bool isNumber(char) const noexcept;
   inline bool isNewLine(char) const noexcept;
   inline bool isEnd(char) const noexcept;
};
