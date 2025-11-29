#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <algorithm>
#include "Shader.h"
#include "stb_image.h"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

bool bDrawingInWireframe = false;

float texturesMixAlpha = 0.f;

void framebuffer_resize_callback(GLFWwindow* targetWindow, int newWidth, int newHeight);
void processInput(GLFWwindow* window);
unsigned int LoadImageIntoTexture(const char* imagePath, GLenum textureUnit, GLenum dataFormat);

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

    try
    {
        //Create Shader Program from Class
        Shader shader("Shaders/VertexShader.glsl", "Shaders/FragmentShader.glsl");

        //Generate Vertex Array Object (VAO)
        unsigned int VAO;
        glGenVertexArrays(1, &VAO);

        //Bind the Vertex Array Object to be the one used to hold the VBO info
        glBindVertexArray(VAO);

        //Draw a triangle
        //Create a vertices array   (Vertex Location, Vertex Color, Texture Coordinate)
        float vertices[] =
        {
           -0.7f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
           0.7f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
           -0.7f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
           0.7f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f
        };

        unsigned int indices[] =
        {
            0, 1, 2,
            2, 3, 1
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

        //Load two Images into 2 different textures
        unsigned int texture1 = LoadImageIntoTexture("Assets/Textures/container.jpg", GL_TEXTURE0, GL_RGB);
        unsigned int texture2 = LoadImageIntoTexture("Assets/Textures/awesomeface.png", GL_TEXTURE1, GL_RGBA);
        

        //Set Vertix Attribute Data
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //0 Attribute in the vertex shader, 3 vec3 has 3 float values, GL_FLOAT each input is a float value, GL_FALSE no normalization to be used, size of each stride to move to the next vertex, pointer to the first element in the Vertex Buffer
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);


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

            //Draw triangle from vertices
            {
                //Use the Shader Program to draw Vertices using the defined vertex and fragment shaders
                shader.Use();

                //Set the texture Unit for each sampler2D uniform parameter in the Fragment Shader
                shader.SetInt("Texture1", 0);
                shader.SetInt("Texture2", 1);
                shader.SetFloat("mixAlpha", texturesMixAlpha);

                //Bind the VAO
                glBindVertexArray(VAO);

                //Draw triangle using previous data
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

                glBindVertexArray(0);
            }

            //Context Swapping and check polling events
            glfwSwapBuffers(currentWindow);
            glfwPollEvents();
        }

        glfwTerminate();
        return 0;
    }
    catch(std::exception e)
    {
        std::cout << "Shader creation threw an exception!" << std::endl;
        glfwTerminate();
        return -1;
    }
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
    //Change the texture mix Alpha by 0.1f increments or decrements when arrow keys are pressed
    if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS))
    {
        texturesMixAlpha = std::clamp(texturesMixAlpha + 0.01f, 0.0f, 1.0f);
    }
    else if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS))
    {
        texturesMixAlpha = std::clamp(texturesMixAlpha - 0.01f, 0.0f, 1.0f);
    }
}

unsigned int LoadImageIntoTexture(const char* imagePath, GLenum textureUnit, GLenum dataFormat)
{
    unsigned int outTexture = -1;
    //Load Texture Data from image path
    int width, height, nChannels;
    stbi_set_flip_vertically_on_load(true); //To match images and OpenGL texture y orientation
    unsigned char* data = stbi_load(imagePath, &width, &height, &nChannels, 0);
    if (data)
    {
        //Generate Texture Object
        glGenTextures(1, &outTexture);

        //Activate Texture Unit and Texture to object
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_2D, outTexture);

        //Set Texture Parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //Load image data to Texture object
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture with path: " << imagePath << std::endl;
    }

    //Free loaded Image
    stbi_image_free(data);

    return outTexture;
}