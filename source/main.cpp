//#include<glad/glad.h>
#include<shader.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<string>
#include<GameOfLifeLogic.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
std::vector<unsigned int> getIndices(std::vector<unsigned int> globalIndices,
                            std::vector<std::vector<Cell>> cells);
GLFWwindow* InitializeWindow();


int main()
{
    float sizeOfRow =10;

    std::vector<int> seed;
    seed.emplace_back(55);
    seed.emplace_back(54);
    seed.emplace_back(53);


    auto Life = new GameOfLifeLogic(sizeOfRow,seed);

    std::vector<float> vertices;
    for(float i =0;i<=sizeOfRow;++i)
    {
        for(float j =0;j<=sizeOfRow;++j)
        {
            vertices.emplace_back( -1.f+j*(2.0f/sizeOfRow) );
            vertices.emplace_back( 1.f -i*(2.0f/sizeOfRow));
            vertices.emplace_back(0);
        }
    }
    std::vector<unsigned int> indices;
    for(int j =0;j<sizeOfRow*sizeOfRow;j+=sizeOfRow+1)
        for(int i=0;i<sizeOfRow;i++)
        {

                indices.emplace_back(j+i);
                indices.emplace_back(j+i+1);
                indices.emplace_back(j+i+sizeOfRow+2);
                indices.emplace_back(j+i);
                indices.emplace_back(j+i+sizeOfRow+1);
                indices.emplace_back(j+i+sizeOfRow+2);
        }
        auto indices2 = getIndices(indices,Life->cells);
    int k =0;
    for(auto i : indices2)
    {

k++;
        std::cout << static_cast<int>(i) <<" ";
        if(k==3)
        {
            k=0;
        std::cout<<"\n";
        }
        
    }

    auto* window = InitializeWindow();
    
    Shader ourShader("vertexShader.vs",
                     "fragmentShader.fs");
    
    unsigned int VBO,VAO,EBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float),&vertices[0],GL_STATIC_DRAW);
    
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,  indices2.size()*sizeof( unsigned int), &indices2[0], GL_DYNAMIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

double lastTime =0.0;
    while(!glfwWindowShouldClose(window))
    { 
        processInput(window);

        glClearColor(0.0f,0.0f,0.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,  indices.size()*sizeof( unsigned int), &indices[0], GL_DYNAMIC_DRAW);

        ourShader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,  indices2.size()*sizeof( unsigned int), &indices2[0], GL_DYNAMIC_DRAW);

        glDrawElements(GL_TRIANGLES, indices2.size(), GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
        std::cout<<glfwGetTime()<<"\n";
        if(glfwGetTime()-lastTime >= 2.0)
        {   
            lastTime=glfwGetTime();  
            Life->UpadateCells();
            indices2=getIndices(indices,Life->cells);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,  indices2.size()*sizeof( unsigned int), &indices2[0], GL_DYNAMIC_DRAW);
        }
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0,0,width,height);
}

void processInput(GLFWwindow* window)
{
    if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window,true);
    }
}

GLFWwindow* InitializeWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);   


    auto* window = glfwCreateWindow(800,600,"Learn OpenGL",nullptr,nullptr);
    if (window == nullptr)
    {
        std::cerr<<"Failed to Create GLFW Window"<<std::endl;
        glfwTerminate();
        throw std::runtime_error("GLFW Load Failed");
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD"<<std::endl;
        throw std::runtime_error("Glad Load Failed");
    }
    glViewport(0,0,800,600);

    return window;
}

std::vector<unsigned int> getIndices(std::vector<unsigned int> globalIndices,std::vector<std::vector<Cell>> cells)
{
    std::vector<unsigned int> indices;
    for(int i =0;i<cells.size();++i)
    {
        for(int j=0;j<cells[i].size();++j)
        {
            if(cells[i][j]==Cell::life)
            indices.insert(indices.begin(),&globalIndices[i*cells.size()*6 + j*6],&globalIndices[i*cells.size()*6 +j*6+6]);
        }
    }
    return indices;
}