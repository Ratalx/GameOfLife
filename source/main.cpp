#include<shader.hpp>
#include<GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include<GameOfLifeLogic.hpp>
#include<RleReader.hpp>
#include<iostream>
#include<string>
#include<memory>
#include<functional>
#include<array>


using uniqueWindowPtr =std::unique_ptr<GLFWwindow,std::function<void(GLFWwindow*)>>;

void framebuffer_size_callback(uniqueWindowPtr window, int width, int height);
uniqueWindowPtr processInput(uniqueWindowPtr window);
std::vector<unsigned int> getIndices(std::vector<unsigned int> globalIndices,
                            std::vector<std::vector<Cell>> cells);
uniqueWindowPtr InitializeWindow();
std::vector<unsigned int> makeGridIndices(const int sizeOfGridIndices,int sizeOfRow);
double clockToMilliseconds(clock_t ticks);
ImGuiIO SetupImGuiContext();

int main()
{
    int sizeOfRow = 10;
    float tickTime = 0.0;
    float frameCap = 60.0;
    const char* glsl_version = "#version 130";


    RleReader rleReader("RlePatterns/bi-gun.rle");

    std::unique_ptr<GameOfLifeLogic> Life(new GameOfLifeLogic(rleReader.GenerateStartVector()));
    sizeOfRow = Life->cells.size();



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
     auto sizeOfGridIndices =sizeOfRow*sizeOfRow*sizeOfRow; 
    
    auto gridIndices = makeGridIndices(sizeOfGridIndices,sizeOfRow);

    auto lifeCellsIndices = getIndices(gridIndices,Life->cells);

    auto window = InitializeWindow();

    auto io = SetupImGuiContext();
    ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


   std::unique_ptr<Shader> BoxShader;
    std::unique_ptr<Shader> GridShader;
    
    try
    {
        BoxShader = std::make_unique<Shader>(Shader("vertexShader.vs",
                               "BoxFragmentShader.fs"));
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }
    try
    {
        GridShader = std::make_unique<Shader>(Shader("vertexShader.vs",
                                "GridFragmentShader.fs"));    
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }
    


    unsigned int VBO,VAO,EBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float),&vertices[0],GL_STATIC_DRAW);

    
    
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gridIndices.size()*sizeof( unsigned int),
                                                 &gridIndices[0], GL_DYNAMIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
    glEnableVertexAttribArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    double lastTime =0.0;

    clock_t deltaTime = 0;
    unsigned int frames = 0;
    double  frameRate = 30;
    double  averageFrameTimeMilliseconds = 33.333;

    while(!glfwWindowShouldClose(window.get()))
    { 
        clock_t beginFrame = clock();
        window = processInput(std::move(window));


       ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            // ImGui::Checkbox("Start/Stop", &isStart);      // Edit bools storing our window open/close state
            // ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        ImGui::Render();

        glClearColor(0.0f,0.0f,0.0f,1.0f);
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        GridShader->use();
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, gridIndices.size()*sizeof(unsigned int),
                                                    &gridIndices[0], GL_STATIC_DRAW);

        glDrawElements(GL_TRIANGLES,gridIndices.size(),GL_UNSIGNED_INT,0);


        BoxShader->use();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, lifeCellsIndices.size()*sizeof(unsigned int),
                                                    &lifeCellsIndices[0], GL_DYNAMIC_DRAW);
        glDrawElements(GL_TRIANGLES, lifeCellsIndices.size(), GL_UNSIGNED_INT, 0);
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window.get());
        glfwPollEvents();

     


        if(glfwGetTime()-lastTime >= tickTime)
        {   
            lastTime=glfwGetTime();  
            Life->UpadateCells();
            lifeCellsIndices=getIndices(gridIndices,Life->cells);

            frames ++;
        }



            clock_t endFrame = clock();        
            deltaTime += endFrame - beginFrame;

        if( clockToMilliseconds(deltaTime)>1000.0)
        { 
            frameRate = (double)frames*0.5 +  frameRate*0.5; //more stable
            std::cout<<"Frames:"<<frames<<std::endl;

            frames = 0;
            deltaTime -= CLOCKS_PER_SEC;
            averageFrameTimeMilliseconds  = 1000.0/(frameRate==0?0.001:frameRate);

            std::cout<<"CPU time was:"<<averageFrameTimeMilliseconds<<std::endl;
            std::cout<<"fps:"<<frameRate<<std::endl;
        }
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0,0,width,height);
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
    for(int i =0;i<cells.size();++i)
    {
        for(int j=0;j<cells[i].size();++j)
        {
            if(cells[i][j]==Cell::life)
            indices.insert(indices.begin(),&globalIndices[i*cells.size()*6 + j*6],
                           &globalIndices[i*cells.size()*6 +j*6+6]);
        }
    }
    return indices;
}

std::vector<unsigned int> makeGridIndices( int sizeOfGridIndices, int sizeOfRow) 
{
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

ImGuiIO SetupImGuiContext()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
return  std::move(io);
}

double clockToMilliseconds(clock_t ticks){
    // units/(units/time) => time (seconds) * 1000 = milliseconds
    return (ticks/(double)CLOCKS_PER_SEC)*1000.0;
}