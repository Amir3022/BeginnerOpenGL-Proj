#include "GameModules/CubesGame.h"
#include "Utilities/EngineUtilities.h"

bool CubesGame::Init()
{
	if (!Game::Init())
		return false;

    try
    {
        //Create Shader Program from Class
        shader = std::make_shared<Shader>(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

        //Generate Vertex Array Object (VAO)
        unsigned int VAO;
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
        unsigned int texture1 = EngineUtilities::LoadImageIntoTexture("Assets/Textures/container.jpg");
        unsigned int texture2 = EngineUtilities::LoadImageIntoTexture("Assets/Textures/awesomeface.png");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        //Set Vertex Attribute Data
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //0 Attribute in the vertex shader, 3 vec3 has 3 float values, GL_FLOAT each input is a float value, GL_FALSE no normalization to be used, size of each stride to move to the next vertex, pointer to the first element in the Vertex Buffer
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

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

void CubesGame::UpdateGame(float deltaTime)
{
    Game::UpdateGame(deltaTime);
}

void CubesGame::DrawFrame()
{
    Game::DrawFrame();

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
        glm::mat4 projection = glm::perspective(glm::radians(camera->GetCameraFOV()), float(GetWidth() / GetHeight()), 0.1f, 100.0f);
        shader->SetMat44("projection", projection);

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
    }
}

void CubesGame::ProcessInput(GLFWwindow* window)
{
    Game::ProcessInput(window);

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
}


