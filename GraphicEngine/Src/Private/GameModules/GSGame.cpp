#include "GameModules/GSGame.h"

GSGame::GSGame(int in_width, int in_height)
	:Game(in_width, in_height)
{
	fragmentShaderPath = "Shaders/GSSceneShaders/FragmentShader.glsl";
	vertexShaderPath = "Shaders/GSSceneShaders/VertexShader.glsl";
	geometryShaderPath = "Shaders/GSSceneShaders/GeometryShader.glsl";
}

bool GSGame::Init()
{
	if (!Game::Init())
		return false;

	try
	{
		//Create Shader Program Instance
		shader = std::make_shared<Shader>(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

		//Try attaching the Geometry Shader to the Created Shader Program
		if (!shader->CreateGeometryShader(geometryShaderPath.c_str()))
		{
			std::cout << "Failed to Create the Geometry Shader" << std::endl;
			return false;
		}

		//Create Vertex Array Object
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		//Declare Vertices and Elements Data (Vertex Coordinates)
		float vertices[] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			-0.5f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.0f,
		};

		unsigned int indices[] = {
			0, 1, 2, 3
		};

		unsigned int VBO, EBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		//Unbind the Generated VAO
		glBindVertexArray(0);

		return true;
	}
	catch(std::exception e)
	{
		std::cout << "Failed to initialize the Geometry Game Module with the following exception: " << e.what() << std::endl;
		return false;
	}
}

void GSGame::Terminate()
{
	Game::Terminate();
}

void GSGame::UpdateGame(float deltaTime)
{
	Game::UpdateGame(deltaTime);
}

void GSGame::DrawFrame()
{
	Game::DrawFrame();

	//Disable Depth Test, Reset the Color Buffer to absolute black
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Bind the Used VAO
	glBindVertexArray(VAO);

	//Use the Create Shader Program
	shader->Use();

	//Draw the Points vertices on Screen
	glDrawElements(GL_POINTS, 4, GL_UNSIGNED_INT, (void*)0);
}