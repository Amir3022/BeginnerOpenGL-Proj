#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

const char* VertexShaderSource = "#version 330 core\nlayout(location = 0) in vec3 aPos;\nlayout(location = 1) in vec3 aColor;\nout vec4 ourColor;\nvoid main()\n{\ngl_Position = vec4(aPos, 1.0f);\nourColor = vec4(aColor, 1.0f);\n}";
const char* FragmentShaderSource = "#version 330 core\nin vec4 ourColor;\nout vec4 FragColor;\nvoid main()\n{\nFragColor = ourColor;\n}";
bool bDrawingInWireframe = false;

void framebuffer_resize_callback(GLFWwindow* targetWindow, int newWidth, int newHeight);
void processInput(GLFWwindow* window);

int main()
{
    //Initialize GLFW Context
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  //Set Major Version to 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);  //Set minor version to 3, final version 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  //Set the OpenGL profile to use the Core profile

    //Create GLFW Window
    GLFWwindow* currentWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Learning OpenGL", nullptr, nullptr);
    if (!currentWindow)
    {
        std::cout << "Failed to create glfw Window";
        glfwTerminate();
        return -1;
    }
    //Set current Window as the Current Context for GLFW
    glfwMakeContextCurrent(currentWindow);

    //Make sure GLAD is initialized
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD";
        glfwTerminate();
        return -1;
    }

    //Set openGL viewport coordinates
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT); //Example of having GLAD get the function pointer for this specific system

    //Adding callback to the window to change the viewport size when it's size changes
    glfwSetFramebufferSizeCallback(currentWindow, framebuffer_resize_callback);

    //Create Vertex Shader and compile the vertex shader source
    unsigned int vertexShaderObject;
    vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderObject, 1, &VertexShaderSource, nullptr);
    glCompileShader(vertexShaderObject);

    //Check if the Shader Compiles Successfully
    int vertCompileState;
    char vertInfoLog[512];
    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &vertCompileState);

    if (vertCompileState)
    {
        std::cout << "Vertex Shader compiled Successfully!" << std::endl;
    }
    else
    {
        glGetShaderInfoLog(vertexShaderObject, 512, nullptr, vertInfoLog);
        std::cout << "Vertex Shader failed to compile: " << vertInfoLog << std::endl;
        glfwTerminate();
        return -1;
    }

    //Create Fragment Shader and Compile the Fragment Shader source
    unsigned int fragmentShaderObject;
    fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderObject, 1, &FragmentShaderSource, nullptr);
    glCompileShader(fragmentShaderObject);
    //Check if fragment shader compiled successfully
    int fragCompileStatus; 
    char fragInfoLog[512];
    glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &fragCompileStatus);
    if (fragCompileStatus)
    {
        std::cout << "Fragment Shader compiled Successfully!" << std::endl;
    }
    else
    {
        glGetShaderInfoLog(fragmentShaderObject, 512, nullptr, fragInfoLog);
        std::cout << "Fragment Shader failed to compile: " << fragInfoLog << std::endl;
        glfwTerminate();
        return -1;
    }

    //Create Shader program to link previously compiled shaders
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShaderObject);
    glAttachShader(shaderProgram, fragmentShaderObject);
    glLinkProgram(shaderProgram);
    //Check the Shader Program linked successfully
    int SPLinkingStatus;
    char SPInfoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &SPLinkingStatus);
    if (SPLinkingStatus)
    {
        std::cout << "Shader program successfully linked!" << std::endl;
    }
    else
    {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, SPInfoLog);
        std::cout << "Shader Program failed to link: " << SPInfoLog << std::endl;
        glfwTerminate();
        return -1;
    }

    //Delete created shaders after linking them to Shader Program
    glDeleteShader(vertexShaderObject);
    glDeleteShader(fragmentShaderObject);

    //Generate Vertex Array Object (VAO)
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    //Bind the Vertex Array Object to be the one used to hold the VBO info
    glBindVertexArray(VAO);

    //Draw a triangle
    //Create a vertices array
    float vertices[] =
    {
       -0.7f, -0.7f, 0.0f, 1.0f, 0.0f, 0.0f,
       0.0f, 0.7f, 0.0f, 0.0f, 1.0f, 0.0f,
       0.7f, -0.7f, 0.0f, 0.0f, 0.0f, 1.0f
    };

    unsigned int indices[] =
    {
        0, 2, 1
    };

    //Create Vertex Buffer Object (VBO)
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    //Bind the created VBO to the Array Buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //Move the vertices data to the target Buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //Generate Elements Buffer Object (Indices array)
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //Move indices data to the Elements Buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //Set Vertix Attribute Data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //0 Attribute in the vertex shader, 3 vec3 has 3 float values, GL_FLOAT each input is a float value, GL_FALSE no normalization to be used, size of each stride to move to the next vertex, pointer to the first element in the Vertex Buffer
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    

    //Prevent application from closing when window shouldn't be closed
    while (!glfwWindowShouldClose(currentWindow))
    {
        //Input, Game Logic
        processInput(currentWindow);
        
        //Render Logic

        //Clear previous buffer
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float greenColorVal = 0.0f;
        //Draw triangle from vertices
        {
            //Update the current color to set in the Uniform property in shader program
            greenColorVal = 0.5f + sinf(4 * glfwGetTime()) / 2.0f;
            unsigned int ourColorUniformLocation = glGetUniformLocation(shaderProgram, "ourColor");

            //Use the Shader Program to draw Vertices using the defined vertex and fragment shaders
            glUseProgram(shaderProgram);

            //Setting the Uniform color value in current used shader program
            glUniform4f(ourColorUniformLocation, 0.0f, greenColorVal, 0.0f, 1.0f);

            //Bind the VAO
            glBindVertexArray(VAO);

            //Draw triangle using previous data
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)0);

            glBindVertexArray(0);
        }

        //Context Swapping and check polling events
        glfwSwapBuffers(currentWindow);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


void framebuffer_resize_callback(GLFWwindow* targetWindow, int newWidth, int newHeight)
{
    glViewport(0, 0, newWidth, newHeight);
}

void processInput(GLFWwindow* window)
{
    //Close the window if the ESC key is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    //Change between wireframe and filled drawing mode when TAB is pressed
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
    {
        bDrawingInWireframe = !bDrawingInWireframe;
        glPolygonMode(GL_FRONT_AND_BACK, bDrawingInWireframe ? GL_LINE : GL_FILL);
    }
}