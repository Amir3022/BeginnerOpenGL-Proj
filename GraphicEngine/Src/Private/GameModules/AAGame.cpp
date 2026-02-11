#include "GameModules/AAGame.h"

AAGame::AAGame(int in_width, int in_height)
	: Game(in_width, in_height)
{
	fragmentShaderPath = "Shaders/AAScene/FragmentShader.glsl";
	vertexShaderPath = "Shaders/AAScene/VertexShader.glsl";

	bWasChangeMSAAPressed = false;
	bUseMSAA = true;
}

bool AAGame::Init()
{	
	if (!Game::Init())
		return false;

	try
	{
		//Change window to use 4 samples per pixel before creating the GLFW Window
		glfwWindowHint(GLFW_SAMPLES, 4);
		//Enable multisampling Anti Aliasing
		ReInitWindow();
		glEnable(GL_MULTISAMPLE);

		//Create Shader Program from Class
		shader = std::make_shared<Shader>(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

		//Create VAO for the Post Process quad having only a single quad taking the whole screen real state
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		//Create a unit cube at Origin, consisting only of vertex locations
		float cubeVertices[] = {
			-0.5f, -0.5f, 0.5f,		//0
			0.5f, -0.5f, 0.5f,		//1
			-0.5f, 0.5f, 0.5f,		//2
			0.5f, 0.5f, 0.5f,		//3

			-0.5f, -0.5f, -0.5f,	//4
			0.5f, -0.5f, -0.5f,		//5
			-0.5f, 0.5f, -0.5f,		//6
			0.5f, 0.5f, -0.5f,		//7
		};

		unsigned int cubeIndices[] =
		{
			0, 1, 3,
			0, 3, 2,

			4, 7, 5,
			4, 6, 7,

			1, 5, 7,
			1, 7, 3,

			4, 0, 2,
			4, 2, 6,

			2, 3, 7,
			2, 7, 6,

			0, 5, 1,
			0, 4, 5,
		};

		//Create vertex and Element buffers
		unsigned int VBO, EBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//Assign data to the buffers
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
		//Declare Vertex attributes
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		//Unbind the VAO
		glBindVertexArray(0);

		return true;
	}
	catch (std::exception e)
	{
		std::cout << "Shader creation threw an exception!" << std::endl;
		glfwTerminate();
		return false;
	}
}

void AAGame::Terminate()
{
	Game::Terminate();
}

void AAGame::UpdateGame(float deltaTime)
{
	Game::UpdateGame(deltaTime);

	if (bUseMSAA)
	{
		glEnable(GL_MULTISAMPLE);
	}
	else
	{
		glDisable(GL_MULTISAMPLE);
	}
}

void AAGame::DrawFrame()
{
	Game::DrawFrame();

	glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw the Cube
	//Check if the cubemap Shader is valid
	if (shader)
	{
		//Bind the cubemap VAO
		glBindVertexArray(VAO);

		//Create the Model matrix to convert the cube from local space to world space
		glm::mat4 modelMat = glm::identity<glm::mat4>();
		modelMat = glm::translate(modelMat, glm::vec3(0.0f, 0.0f, 0.0f));

		//Create the view matrix using camera lookAt target point, need to remove the translation data from view matrix so take the 3x3 upper left matrix
		glm::mat4 view = camera->GetLookAtMat(camera->GetCameraLocation() + camera->GetCameraForwardDir());

		//Create the projection matrix to project the view space to NDC
		glm::mat4 projection = glm::perspective(glm::radians(camera->GetCameraFOV()), (float)GetWidth() / (float)GetHeight(), 0.1f, 100.0f);

		//Use the Shader Program to draw Vertices using the defined vertex and fragment shaders, view, projection matrices
		shader->Use();
		shader->SetMat44("model", modelMat);
		shader->SetMat44("view", view);
		shader->SetMat44("projection", projection);


		//Draw the cubemap
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);

		//Unbind the cubemapVAO and cubemap texture
		glBindVertexArray(0);
	}
}

void AAGame::ProcessInput(GLFWwindow* window)
{
	Game::ProcessInput(window);
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		if (!bWasChangeMSAAPressed)
		{
			bWasChangeMSAAPressed = true;
			bUseMSAA = !bUseMSAA;
		}
	}
	else
	{
		bWasChangeMSAAPressed = false;
	}
}
