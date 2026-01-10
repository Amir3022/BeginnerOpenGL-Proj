#include "GameModules/Game.h"

int Game::Process()
{
    if (Init())
    {
        try
        {
            //Prevent application from closing when window shouldn't be closed
            while (!glfwWindowShouldClose(currentWindow))
            {
                //Update DeltaTime
                calculateDeltaTime();

                //Update Game Logic
                UpdateGame(GetDeltaTime());

                //Draw Frame with updated variables
                DrawFrame();

                //Context Swapping and check polling events
                glfwSwapBuffers(currentWindow);
                glfwPollEvents();
            }
            //Terminate the game processes
            Terminate();

            glfwTerminate();
            return 0;
        }
        catch (std::exception e)
        {
            std::cout << "Shader creation threw an exception!" << std::endl;
            glfwTerminate();
            return -1;
        }
    }
    else
    {
        std::cout << "Failed to initialize GLFW!" << std::endl;
        glfwTerminate();
        return -1;
    }
}

bool Game::Init()
{
    //Initialize GLFW Context
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  //Set Major Version to 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);  //Set minor version to 3, final version 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  //Set the OpenGL profile to use the Core profile

    //Create GLFW Window
    currentWindow = glfwCreateWindow(width, height, "Learning OpenGL", nullptr, nullptr);
    if (!currentWindow)
    {
        std::cout << "Failed to create glfw Window";
        glfwTerminate();
        return false;
    }
    //Set current Window as the Current Context for GLFW
    glfwMakeContextCurrent(currentWindow);

    //Make sure GLAD is initialized
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD";
        glfwTerminate();
        return false;
    }

    //Set openGL viewport coordinates
    glViewport(0, 0, width, height); //Example of having GLAD get the function pointer for this specific system

    //Enable Depth Test to allow usage of Z-Buffer
    glEnable(GL_DEPTH_TEST);

    //Set the Window User Pointer to the game class so it can have control over the window actions
    glfwSetWindowUserPointer(currentWindow, this);

    //Adding callback to the window to change the viewport size when it's size changes
    glfwSetFramebufferSizeCallback(currentWindow, framebuffer_resize_callback);

    //Capturing mouse cursor input, and add callback to when camera cusror is moved
    glfwSetInputMode(currentWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(currentWindow, mouseCursor_move_callback);

    //Capture mouse scroll input, and add callback to when scroll is used
    glfwSetScrollCallback(currentWindow, mouseScroll_change_callback);
    
    return true;
}

void Game::Terminate()
{
    //Set the bound vertex array to 0
    glBindVertexArray(0);
}

void Game::UpdateGame(float deltaTime)
{
    //Input, Game Logic
    ProcessInput(currentWindow);
}

void Game::DrawFrame()
{
    //Clear previous buffer
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Game::framebuffer_resize_callback(GLFWwindow* targetWindow, int newWidth, int newHeight)
{
    glViewport(0, 0, newWidth, newHeight);
}

void Game::mouseCursor_move_callback(GLFWwindow* window, double xpos, double ypos)
{
    glm::vec2 curCursorPos = glm::vec2(xpos, ypos);
    Game* gameRef = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (gameRef)
    {
        if (gameRef->IsCursorFirstUpdate())
        {
            gameRef->SetLastCursorPos(curCursorPos);
            gameRef->SetCursorFirstUpdate(false);
            return;
        }
        glm::vec2 deltaCursorPos = curCursorPos - gameRef->GetLastCursorPos();
        gameRef->SetLastCursorPos(curCursorPos);

        if(gameRef->GetCamera())
            gameRef->GetCamera()->ProcessMouseMovementInput(deltaCursorPos);
    }
}

void Game::mouseScroll_change_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Game* gameRef = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if(gameRef && gameRef->GetCamera())
        gameRef->GetCamera()->ProcessMouseScrollInput((float)(-yoffset));
}

void Game::ProcessInput(GLFWwindow* window)
{
    //Close the window if the ESC key is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    //Change between wireframe and filled drawing mode when TAB is pressed
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
    {
        if (!bSwitchPolygonDrawModePressed)
        {
            bSwitchPolygonDrawModePressed = true;
            bDrawingInWireframe = !bDrawingInWireframe;
            glPolygonMode(GL_FRONT_AND_BACK, bDrawingInWireframe ? GL_LINE : GL_FILL);
        }
    }
    else
    {
        bSwitchPolygonDrawModePressed = false;
    }

    //Move Camera Position using WASD
    glm::vec2 inputVec = glm::vec2(0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        inputVec.y = 1.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        inputVec.y = -1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        inputVec.x = 1.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        inputVec.x = -1.0f;
    }

    //Move Camera Upwards and Downwards using Space, CTRL inputs
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camera->ProcessKeyboardHeightInput(1.0f, GetDeltaTime());
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        camera->ProcessKeyboardHeightInput(-1.0f, GetDeltaTime());
    }

    if (inputVec.x > 0 || inputVec.y > 0)
        inputVec = glm::normalize(inputVec);

    camera->ProcessKeyboardInput(inputVec, GetDeltaTime());
}

void Game::calculateDeltaTime()
{
    currentFrameTime = glfwGetTime();
    deltaTime = currentFrameTime - lastFrameTime;
    lastFrameTime = currentFrameTime;
}