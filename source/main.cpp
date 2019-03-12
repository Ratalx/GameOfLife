//#include<glad/glad.h>
#include<shader.h>

#include<GLFW/glfw3.h>
#include<iostream>
#include<string>
#include<vector>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
GLFWwindow* InitializeWindow();


int main()
{
    float sizeOfRow =2;
    float vertices[] = 
    {
        0.5f,  0.5f,  0.0f,
        0.5f,   -0.5f,  0.0f,

        -0.5f,  -0.5f,  0.0f,
        -0.5f,  0.5f,   0.0f
    };
    std::vector<float> vertices2;
    for(float i =0;i<=sizeOfRow;++i)
    {
        for(float j =0;j<=sizeOfRow;++j)
        {
            vertices2.emplace_back( -1.f+j*(2.0f/sizeOfRow) );
            vertices2.emplace_back( 1.f -i*(2.0f/sizeOfRow));
            vertices2.emplace_back(0);
        }
    }
    std::vector<unsigned int> indices2;
    //indices2.resize(sizeOfRow*sizeOfRow*3);
    
        for(int i=0;i<sizeOfRow*sizeOfRow;i=+sizeOfRow)
        {

                indices2.emplace_back(i);
                indices2.emplace_back(i+1);
                indices2.emplace_back(i+sizeOfRow+2);
                indices2.emplace_back(i);
                indices2.emplace_back(i+sizeOfRow+1);
                indices2.emplace_back(i+sizeOfRow+2);
            
        }
    
    int k =0;
    for(auto i : indices2)
    {
k++;
        std::cout << i <<" ";
        if(k==3)
        {
            k=0;
        std::cout<<"\n";
        }
    }


    unsigned int indices[] =
    {
        0,1,3, //first triangle
       1,2,3 //second triangle
    };

    auto* window = InitializeWindow();
    
    Shader ourShader("vertexShader.vs",
                     "fragmentShader.fs");
    
    unsigned int VBO,VAO,EBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices2.size()*sizeof(float),&vertices2[0],GL_STATIC_DRAW);
    
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,  indices2.size()*sizeof( unsigned int), &indices2[0], GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);



        glClearColor(0.0f,0.0f,0.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ourShader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
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
void InitializeOpenGL();
void InitializeBuffers();