#include "Game.h"

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

    //Set the Window User Pointer to the game class so it can have control over the window actions
    glfwSetWindowUserPointer(currentWindow, this);

    //Adding callback to the window to change the viewport size when it's size changes
    glfwSetFramebufferSizeCallback(currentWindow, framebuffer_resize_callback);

    //Capturing mouse cursor input, and add callback to when camera cusror is moved
    glfwSetInputMode(currentWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(currentWindow, mouseCursor_move_callback);

    //Capture mouse scroll input, and add callback to when scroll is used
    glfwSetScrollCallback(currentWindow, mouseScroll_change_callback);

    try
    {
        //Create Shader Program from Class
        shader = std::make_unique<Shader>(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

        //Generate Vertex Array Object (VAO)
        glGenVertexArrays(1, &VAO);

        //Bind the Vertex Array Object to be the one used to hold the VBO info
        glBindVertexArray(VAO);

        //Draw a Cube (Cube consists of 6 faces each consisting of 2 triangles, 8 vertices with 36 index)
        //Create a vertices array   (Vertex Location, Vertex Color, Texture Coordinate)
        float vertices[] =
        {
           -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
           0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
           -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
           0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,

           -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
           0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
           -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
           0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,

           -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
           0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
           -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
           0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f
        };

        unsigned int indices[]
        {
            0, 1, 2,
            2, 1, 3,
            4, 6, 5,
            6, 7, 5,
            2, 4, 0,
            2, 6, 4,
            1, 7, 3,
            1, 5, 7,
            2, 11, 10,
            2, 11, 3,
            0, 8, 9,
            0, 9, 1
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


        //Set Vertex Attribute Data
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //0 Attribute in the vertex shader, 3 vec3 has 3 float values, GL_FLOAT each input is a float value, GL_FALSE no normalization to be used, size of each stride to move to the next vertex, pointer to the first element in the Vertex Buffer
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);


        glBindVertexArray(0);

        //Enable Depth Test to allow usage of Z-Buffer
        glEnable(GL_DEPTH_TEST);

        //Array for locations, rotations for 10 random cubes(x, y, z, rot orientation)
        cubeTransforms = 
        {
           {glm::vec3(-4.0f,  -0.8f,  -9.0f), glm::vec3(1.7f, -0.4f, 0.9f)},
           {glm::vec3(2.0f,  5.0f, -15.0f), glm::vec3(-1.3f, 1.8f, -2.0f)},
           {glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(0.5f, -1.9f, 1.2f)},
           {glm::vec3(-3.8f, -2.0f, -12.3), glm::vec3(-0.8f, 0.3f, -1.5f)},
           {glm::vec3(2.4f, -0.4f, -3.5f), glm::vec3(2.0f, 1.1f, -0.7f)},
           {glm::vec3(-1.7f,  3.0f, -7.5f),glm::vec3(-1.6f, -1.0f, 1.4f)},
           {glm::vec3(1.3f, -2.0f, -2.5f), glm::vec3(0.2f, 1.9f, -0.1f)},
           {glm::vec3(1.5f,  2.0f, -2.5f), glm::vec3(1.5f, -2.0f, 0.6f)},
           {glm::vec3(1.5f,  0.2f, -1.5f), glm::vec3(-0.9f, 0.0f, 1.7f)},
           {glm::vec3(-1.3f,  1.0f, -1.5f), glm::vec3(1.2f, -1.4f, -0.5f)}
        };

        return true;
    }
    catch (std::exception e)
    {
        std::cout << "Shader creation threw an exception!" << std::endl;
        glfwTerminate();
        return false;
    }
}

void Game::UpdateGame(float deltaTime)
{
    //Input, Game Logic
    processInput(currentWindow);
}

void Game::DrawFrame()
{
    //Clear previous buffer
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    //Draw triangle from vertices
    if (shader && camera)
    {
        //Use the Shader Program to draw Vertices using the defined vertex and fragment shaders
        shader->Use();

        //Set the texture Unit for each sampler2D uniform parameter in the Fragment Shader
        shader->SetInt("Texture1", 0);
        shader->SetInt("Texture2", 1);
        shader->SetFloat("mixAlpha", texturesMixAlpha);

        //Create Transform matrix to transform the drawn image
        //First, create the model matrix to rotate the object in world space
        glm::mat4 model = glm::identity<glm::mat4>();
        model = glm::rotate(model, glm::radians(objectXRotation), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(objectYRotation), glm::vec3(0.0f, 1.0f, 0.0f));
        shader->SetMat44("model", model);

        //Second, create the view matrix using camera lookAt target point
        glm::mat4 view = camera->GetLookAtMat(camera->GetCameraLocation() + camera->GetCameraForwardDir());
        shader->SetMat44("view", view);

        //Third, create the projection matrix to project the view space to NDC
        glm::mat4 projection = glm::perspective(glm::radians(camera->GetCameraFOV()), float(width / height), 0.1f, 100.0f);
        shader->SetMat44("projection", projection);

        //Bind the VAO
        glBindVertexArray(VAO);

        //Draw triangle using previous data
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);

        //Draw other 10 cubes
        for (int i = 0; i < cubeTransforms.size(); i++)
        {
            model = glm::identity<glm::mat4>();
            float angle = (i + 1) * 20.0f * glfwGetTime();
            model = glm::translate(model, cubeTransforms[i].pos);
            model = glm::rotate(model, glm::radians(angle), cubeTransforms[i].orient);
            shader->SetMat44("model", model);

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
        }

        glBindVertexArray(0);
    }
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

void Game::processInput(GLFWwindow* window)
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

    //Change the object rotation around the x axis with increments or decrements when U,J are pressed
    if ((glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS))
    {
        objectXRotation = objectXRotation - 0.5f;
    }
    else if ((glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS))
    {
        objectXRotation = objectXRotation + 0.5f;
    }
    if (objectXRotation >= 180.0f)
        objectXRotation -= 360.0f;
    else if (objectXRotation < -180.0f)
        objectXRotation += 360.0f;

    //Change the object rotation around the y axis with increments or decrements when H,K are pressed
    if ((glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS))
    {
        objectYRotation = objectYRotation - 0.5f;
    }
    else if ((glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS))
    {
        objectYRotation = objectYRotation + 0.5f;
    }
    if (objectYRotation >= 180.0f)
        objectYRotation -= 360.0f;
    else if (objectYRotation < -180.0f)
        objectYRotation += 360.0f;

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

    if (inputVec.x > 0 || inputVec.y > 0)
        inputVec = glm::normalize(inputVec);

    camera->ProcessKeyboardInput(inputVec, deltaTime);
}

unsigned int Game::LoadImageIntoTexture(const char* imagePath, GLenum textureUnit, GLenum dataFormat)
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

void Game::calculateDeltaTime()
{
    currentFrameTime = glfwGetTime();
    deltaTime = currentFrameTime - lastFrameTime;
    lastFrameTime = currentFrameTime;
}