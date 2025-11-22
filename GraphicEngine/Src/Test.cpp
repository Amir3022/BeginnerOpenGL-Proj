#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

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

    //Prevent application from closing when window shouldn't be closed
    while (!glfwWindowShouldClose(currentWindow))
    {
        //Input, Game Logic
        processInput(currentWindow);
        
        //Render Logic
        //For test, clear the framebuffer with a reddish color
        glClearColor(1.0f, 0.0f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

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
}