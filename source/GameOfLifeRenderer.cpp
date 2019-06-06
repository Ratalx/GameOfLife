#include"../include/GameOfLifeRenderer.hpp"
    namespace GameOfLife{
    using uniqueWindowPtr =std::unique_ptr<GLFWwindow,std::function<void(GLFWwindow*)>>;

    GameOfLifeRenderer::GameOfLifeRenderer(ConfigData* configData) : VBO(0),VAO(0),EBO(0),configData(*configData)
    {
        window = InitializeWindow();
        glfwSetWindowUserPointer(window.get(), configData);
        io = SetupImGuiContext();
        ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    GameOfLifeRenderer::GameOfLifeRenderer(GameOfLifeRenderer && renderer) noexcept :
        window(std::move(renderer.window)),
        VBO(renderer.VBO),
        VAO(renderer.VAO),
        EBO(renderer.EBO),
        CellShader(std::move(renderer.CellShader)),
        GridShader(std::move(renderer.GridShader)),
        configData(renderer.configData),
        io(std::move(renderer.io))
    {
    }

    uniqueWindowPtr GameOfLifeRenderer::InitializeWindow()
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
        glfwSetMouseButtonCallback(window.get(), mouse_button_callback);

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        {
            std::cerr << "Failed to initialize GLAD"<<std::endl;
            throw std::runtime_error("Glad Load Failed");
        }
        glViewport(0,0,800,600);

        return window;
    }

 

    uniqueWindowPtr GameOfLifeRenderer::processInput(uniqueWindowPtr window)
    {
        if(glfwGetKey(window.get(),GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window.get(),static_cast<int>(true));
        }
        return window;
    }
  
    void GameOfLifeRenderer::PrepareBuffers(const std::vector<float> &vertices,const std::vector<unsigned int>& gridIndices)
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    gridIndices.size()*sizeof( unsigned int),
                    &gridIndices[0], GL_DYNAMIC_DRAW);

        glGenBuffers(1,&VBO);
        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float),&vertices[0],GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
        glEnableVertexAttribArray(0);

    }

    void GameOfLifeRenderer::DrawGrid(std::vector<unsigned int> gridIndices)
    {
        GridShader->use();
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, gridIndices.size()*sizeof(unsigned int),
                                                    &gridIndices[0], GL_STATIC_DRAW);
        glDrawElements(GL_TRIANGLES,gridIndices.size(),GL_UNSIGNED_INT,nullptr);
    }

    void GameOfLifeRenderer::DrawCells(std::vector<unsigned int> lifeCellsIndices)
    {
        CellShader->use();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, lifeCellsIndices.size()*sizeof(unsigned int),
                                                    &lifeCellsIndices[0], GL_DYNAMIC_DRAW);
        glDrawElements(GL_TRIANGLES, lifeCellsIndices.size(), GL_UNSIGNED_INT, nullptr);
    }

    void GameOfLifeRenderer::BufferSwap()
    {
        glfwSwapBuffers(window.get());
        glfwPollEvents();
    }
    
    void GameOfLifeRenderer::MakeShaders()
    {
          
        try
        {
          CellShader = std::make_unique<Shader>(Shader("shaders/vertexShader.vs",
                                "shaders/BoxFragmentShader.fs"));
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';

        }
        try
        {
            GridShader = std::make_unique<Shader>(Shader("shaders/vertexShader.vs",
                                    "shaders/GridFragmentShader.fs"));    
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    ImGuiIO GameOfLifeRenderer::SetupImGuiContext()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        return  std::move(io);
    }

    void GameOfLifeRenderer::DrawGui()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("GameOfLife");                          
            auto rleFileIndexTemp=configData.rleFileIndex;
            ImGui::RadioButton("Blinker", &configData.rleFileIndex,0);   
            ImGui::SameLine();
            ImGui::RadioButton("GliterGun", &configData.rleFileIndex,1);
            ImGui::RadioButton("Blinker++",&configData.rleFileIndex,2);
            ImGui::SameLine();
            ImGui::RadioButton("bi-gun",&configData.rleFileIndex,3);
            if(rleFileIndexTemp!=configData.rleFileIndex)
            {
                configData.isChanged = true;
            }
            if (ImGui::Button("Start/Stop"))
            {
                configData.isRunning=!configData.isRunning;
            }  
            ImGui::SameLine();

            ImGui::SliderFloat("tickTime", &configData.tickTime, 0.0f, 1.0f);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        ImGui::Render();

         ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
       glViewport(0,0,width,height);
    }

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            int width,height;
            double xpos, ypos;
            glfwGetWindowSize(window,&width,&height);
            glfwGetCursorPos(window, &xpos, &ypos);
            auto & configData = *static_cast<ConfigData*>(glfwGetWindowUserPointer(window));
            auto x = static_cast<int>(xpos/width*configData.sizeOfRow);
            auto y = static_cast<int>(ypos/height*configData.sizeOfRow);
            configData.cellsToAdd.push_back({x,y});
        }
    }
} // namespace GameOfLife
