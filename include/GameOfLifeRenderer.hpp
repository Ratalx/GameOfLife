#pragma once
#include<shader.hpp>
#include<GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include<RleReader.hpp>
#include<memory>
#include<functional>
#include<vector>
namespace GameOfLife{
struct ConfigData;
class GameOfLifeRenderer
{
    using uniqueWindowPtr =std::unique_ptr<GLFWwindow,std::function<void(GLFWwindow*)>>;
    const char* glsl_version = "#version 130";

    public:
    uniqueWindowPtr window;

    private:
    unsigned int VBO,VAO,EBO;
    std::unique_ptr<Shader> CellShader;
    std::unique_ptr<Shader> GridShader;
    std::shared_ptr<ConfigData> configData;
    ImGuiIO io;

    public:
    //Prepare
    GameOfLifeRenderer(std::shared_ptr<ConfigData> configData);
    GameOfLifeRenderer(GameOfLifeRenderer && renderer) noexcept;
   ~GameOfLifeRenderer()
   {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glfwTerminate();
   }
    void MakeShaders();
    void PrepareBuffers(const std::vector<float>& vertices,const std::vector<unsigned int>& gridIndices);

    bool toClose(){ return glfwWindowShouldClose(window.get());}
    void Clear()
    {
        glClearColor(0.0f,0.0f,0.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    void checkInput(){window = processInput(std::move(window));}

    void DrawGrid(std::vector<unsigned int> gridIndices);
    void DrawCells(std::vector<unsigned int>vertices);
    void BufferSwap();
    void DrawGui();

    private:
    ImGuiIO SetupImGuiContext();
    uniqueWindowPtr InitializeWindow();
    uniqueWindowPtr processInput(uniqueWindowPtr window);


};

struct ConfigData
{
    float tickTime = 0.0;
    int rleFileIndex = 0;
    bool isRunning = true;
    bool isChanged = false; 
};
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
}