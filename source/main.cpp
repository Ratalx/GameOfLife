#include<shader.hpp>
#include<GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include<GameOfLifeLogic.hpp>
#include<GameOfLifeRenderer.hpp>
#include<RleReader.hpp>
#include<iostream>
#include<string>
#include<memory>
#include<functional>
#include<array>

using namespace GameOfLife;
using uniqueWindowPtr =std::unique_ptr<GLFWwindow,std::function<void(GLFWwindow*)>>;

uniqueWindowPtr processInput(uniqueWindowPtr window);
std::vector<unsigned int> getIndices(std::vector<unsigned int> globalIndices,
                            std::vector<std::vector<Cell>> cells);
uniqueWindowPtr InitializeWindow();
std::vector<unsigned int> MakeGridIndices(int sizeOfRow);
double clockToMilliseconds(clock_t ticks);
ImGuiIO SetupImGuiContext();
std::vector<float> MakeVertices(int sizeOfRow);
std::unique_ptr<RleReader> RleUpdate(int rleFileIndex);

int main()
{
    int sizeOfRow = 10;
    std::shared_ptr<ConfigData> configData(new ConfigData());
    std::unique_ptr<RleReader> rleReader;
    try
    {
        rleReader=std::make_unique<RleReader>("RlePatterns/blinker.rle");
    }
    catch(const std::exception& e)
    {
         std::cerr << e.what() << '\n';
         return -1;
    }
    std::unique_ptr<GameOfLifeRenderer> Renderer(new GameOfLifeRenderer(configData));
    std::unique_ptr<GameOfLifeLogic> Life(new GameOfLifeLogic(rleReader->GenerateStartVector()));
   configData->sizeOfRow = Life->cells.size();

    auto vertices= MakeVertices(configData->sizeOfRow);
    auto gridIndices = MakeGridIndices(configData->sizeOfRow);
    auto lifeCellsIndices = getIndices(gridIndices,Life->cells);

    Renderer->MakeShaders();
    Renderer->PrepareBuffers(vertices,gridIndices);

    double lastTime =0.0;

    while(!Renderer->toClose())
    { 

        Renderer->checkInput();

        Renderer->Clear();

        Renderer->DrawGrid(gridIndices);
        Renderer->DrawCells(lifeCellsIndices);
        Renderer->DrawGui();

        Renderer->BufferSwap();

        if(glfwGetTime()-lastTime >= configData->tickTime&&configData->isRunning)
        {   
            if(configData->isChanged)
            {
                rleReader = std::move(RleUpdate(configData->rleFileIndex));
                Life = std::make_unique<GameOfLifeLogic>(rleReader->GenerateStartVector());
                configData->sizeOfRow = Life->cells.size();
                vertices= MakeVertices(configData->sizeOfRow);
                gridIndices = MakeGridIndices(configData->sizeOfRow);
                lifeCellsIndices = getIndices(gridIndices,Life->cells);
                Renderer->PrepareBuffers(vertices,gridIndices);
                configData->isChanged=false;
            }
            lastTime=glfwGetTime();  
            Life->UpadateCells(configData->cellsToAdd);
            configData->cellsToAdd.erase(configData->cellsToAdd.begin(),configData->cellsToAdd.end());
            lifeCellsIndices=getIndices(gridIndices,Life->cells);
        }
    }

    return 0;
}

uniqueWindowPtr processInput(uniqueWindowPtr window)
{
    if(glfwGetKey(window.get(),GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window.get(),true);
    }
    return window;
}

uniqueWindowPtr InitializeWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);   


    uniqueWindowPtr window (glfwCreateWindow(800,600,"Game Of Life",nullptr,nullptr),glfwDestroyWindow);
    if (window == nullptr)
    {
        std::cerr<<"Failed to Create GLFW Window"<<std::endl;
        glfwTerminate();
        throw std::runtime_error("GLFW Load Failed");
    }

    glfwMakeContextCurrent(window.get());
    glfwSetFramebufferSizeCallback(window.get(),framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD"<<std::endl;
        throw std::runtime_error("Glad Load Failed");
    }
    glViewport(0,0,800,600);

    return window;
}

std::vector<unsigned int> getIndices(std::vector<unsigned int> globalIndices,
                                     std::vector<std::vector<Cell>> cells)
{
    std::vector<unsigned int> indices;
    for(unsigned int i =0;i<cells.size();++i)
    {
        for(unsigned int j=0;j<cells[i].size();++j)
        {
            if(cells[i][j]==Cell::life)
            indices.insert(indices.begin(),&globalIndices[i*cells.size()*6 + j*6],
                           &globalIndices[i*cells.size()*6 +j*6+6]);
        }
    }
    return indices;
}

std::vector<unsigned int> MakeGridIndices(int sizeOfRow) 
{
    auto sizeOfGridIndices =sizeOfRow*sizeOfRow*sizeOfRow; 
    std::vector<unsigned int> gridIndices;
    gridIndices.resize(sizeOfGridIndices);
    int k=0;
    for(int j =0;j<sizeOfRow*sizeOfRow;j+=sizeOfRow+1)
        for(int i=0;i<sizeOfRow;i++)
        {
            
                gridIndices[k]=(j+i);
                gridIndices[k+1]=(j+i+1);
                gridIndices[k+2]=(j+i+sizeOfRow+2);
                gridIndices[k+3]=(j+i);
                gridIndices[k+4]=(j+i+sizeOfRow+1);
                gridIndices[k+5]=(j+i+sizeOfRow+2);
                k+=6;
        }
        
return gridIndices;
}

std::vector<float> MakeVertices(int sizeOfRow)
{
    std::vector<float> vertices;
    for(float i =0;i<=sizeOfRow;++i)
    {
        for(float j =0;j<=sizeOfRow;++j)
        {
            vertices.push_back( -1.f+j*(2.0f/sizeOfRow) );
            vertices.push_back( 1.f -i*(2.0f/sizeOfRow));
            vertices.push_back(0);
        }
    }
    return vertices;
}

std::unique_ptr<RleReader> RleUpdate(int rleFileIndex)
{   
    std::string path;
    switch (rleFileIndex)
    {
    case 3:
        path = "RlePatterns/bi-gun.rle";
        break;
    case 1:
        path = "RlePatterns/gosperGliderGun.rle";
        break;
    case 2:
        path = "RlePatterns/blinkerfuse.rle";
        break;
    case 0:
        path = "RlePatterns/blinker.rle";
        break;
    default:
        path ="RlePatterns/blinker.rle";
        break;
    }

    return std::unique_ptr<RleReader>(new RleReader(path));
}
