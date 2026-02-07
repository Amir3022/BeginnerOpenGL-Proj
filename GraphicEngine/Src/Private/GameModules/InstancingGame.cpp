#include "GameModules/InstancingGame.h"

InstancingGame::InstancingGame(int in_width, int in_height)
	:Game(in_width, in_height)
{
	vertexShaderPath = "Shaders/InstancingScene/VertexShader.glsl";
	fragmentShaderPath = "Shaders/InstancingScene/FragmentShader.glsl";

	offsets.clear();
	offsets.reserve(100);
}

bool InstancingGame::Init()
{
	if (!Game::Init())
		return false;

	try
	{
		shader = std::make_shared<Shader>(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

		//Generate Vertex Array Object
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		//Create a simple 4 vertex quad with colors at each vertex
		float vertices[] = {
			-0.05f, -0.05f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.05f, -0.05f, 0.0f, 0.0f, 1.0f, 0.0f,
			-0.05f, 0.05f, 0.0f, 0.0f, 0.0f, 1.0f,
			0.05f, 0.05f, 0.0f, 1.0f, 1.0f, 1.0f,
		};

		unsigned int indices[] =
		{
			0, 1, 3,
			0, 3, 2
		};

		unsigned int VBO, EBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		//Assign the Vertex Attributes
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

		//Generate the offsets values(Will draw 100 instance of the quad)
		glm::vec2 initialOffset = glm::vec2(-0.9, -0.9);
		float offsetVal = 0.2f;
		for (int y = 0; y < 10; y++)
		{
			for (int x = 0; x < 10; x++)
			{
				glm::vec2 offset = initialOffset + glm::vec2(x * offsetVal, y * offsetVal);
				offsets.push_back(offset);
			}
		}

		//Create a new buffer for the instanced array holding quads offsets
		unsigned int instanceVBO;
		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, offsets.size() * sizeof(glm::vec2), offsets.data(), GL_STATIC_DRAW);

		//Assign the instanced array vertex attribute
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glVertexAttribDivisor(2, 1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Unbind the vertex Array Object
		glBindVertexArray(0);


		return true;
	}
	catch(std::exception e)
	{
		std::cout << "Failed to create shader program: " << e.what() << std::endl;
		glfwTerminate();
		return false;
	}
}

void InstancingGame::Terminate()
{
	Game::Terminate();
}

void InstancingGame::UpdateGame(float deltaTime)
{
	Game::UpdateGame(deltaTime);
}

void InstancingGame::DrawFrame()
{
	//Disable Depth Test
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//Bind the vertex array object
	glBindVertexArray(VAO);
	//Use the Shader program to draw the quad
	shader->Use();

	//Draw the Quads as instanced draw
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 100);
}