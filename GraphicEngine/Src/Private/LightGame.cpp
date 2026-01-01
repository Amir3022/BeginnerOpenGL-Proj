#include "LightGame.h"


LightGame::LightGame(int in_width, int in_height)
	: Game(in_width, in_height)
{
	fragmentShaderPath = "Shaders/LightScene/FragmentShader.glsl";
	vertexShaderPath = "Shaders/LightScene/VertexShader.glsl";

	lightFragmentShaderPath = "Shaders/LightScene/LightFragmentShader.glsl";
	lightVertexShaderPath = "Shaders/LightScene/LightVertexShader.glsl";

	lightCubePos = glm::vec3(0.5f, 0.5f, 2.0f);

	objectColor = glm::vec3(1.0f, 0.5f, 0.31f);
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
}

bool LightGame::Init()
{
	if (!Game::Init())
		return false;

	try
	{
		//Create Shader Program from Class
		shader = std::make_unique<Shader>(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

		//Create Light Shader to render the Light Cube
		lightShader = std::make_unique<Shader>(lightVertexShaderPath.c_str(), lightFragmentShaderPath.c_str());

		//Generate Vertex Array Object (VAO)
		glGenVertexArrays(1, &VAO);

		//Bind the Vertex Array Object to be the one used to hold the VBO info
		glBindVertexArray(VAO);

		//Draw a Cube (Cube consists of 6 faces each consisting of 2 triangles, 8 vertices with 36 index)
	   //Create a vertices array   (Vertex Location, normal direction)
		float vertices[] =
		{
		   -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,	//0		//0
		   -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,	//1
		   -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,	//2

		   0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,		//3		//1
		   0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,		//4
		   0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,	//5

		   -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,		//6		//2
		   -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,	//7
		   -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,		//8

		   0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,		//9		//3
		   0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,		//10
		   0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,		//11

		   -0.5f, -0.5f, -0.5, 0.0f, 0.0f, -1.0f,	//12	//4
		   -0.5f, -0.5f, -0.5, -1.0f, 0.0f, 0.0f,	//13
		   -0.5f, -0.5f, -0.5, 0.0f, -1.0f, 0.0f,	//14

		   0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,	//15	//5
		   0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,	//16
		   0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,	//17

		   -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,	//18	//6
		   -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,	//19
		   -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,	//20

		   0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,	//21	//7
		   0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,		//22
		   0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,		//23

		};
		unsigned int indices[]
		{
			0, 3, 6,
			6, 3, 9,

			12, 18, 21,
			12, 21, 15,

			1, 7, 13,
			7, 19, 13,

			4, 16, 10,
			10, 16, 22,

			2, 14, 5,
			14, 17, 5, 

			8, 11, 20,
			20, 11, 23
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

		//Set Vertex Attribute Data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //0 Attribute in the vertex shader, 3 vec3 has 3 float values, GL_FLOAT each input is a float value, GL_FALSE no normalization to be used, size of each stride to move to the next vertex, pointer to the first element in the Vertex Buffer
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		//Create a new Vertex Array Object for the Light Cube
		glGenVertexArrays(1, &lightVAO);
		glBindVertexArray(lightVAO);

		//Bind the same created VBO and EBO to the light VAO, no need to move data as it already was added to the container
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		//Set Vertex Attribute Data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //0 Attribute in the vertex shader, 3 vec3 has 3 float values, GL_FLOAT each input is a float value, GL_FALSE no normalization to be used, size of each stride to move to the next vertex, pointer to the first element in the Vertex Buffer
		glEnableVertexAttribArray(0);

		//Enable Depth Test to allow usage of Z-Buffer
		glEnable(GL_DEPTH_TEST);

		return true;
		
	}
	catch (std::exception e)
	{
		std::cout << "Shader creation threw an exception!" << std::endl;
		glfwTerminate();
		return false;
	}
}

void LightGame::UpdateGame(float deltaTime)
{
	Game::UpdateGame(deltaTime);
}

void LightGame::DrawFrame()
{
	Game::DrawFrame();

	//Draw triangle from vertices
	if (shader && lightShader && camera)
	{
		//Create Transform matrix to transform the drawn image
		//Create the model matrix to rotate the object in world space
		glm::mat4 model = glm::identity<glm::mat4>();

		//Create the Normal Model Matrix to convert normal from local space to World coordinates while respecting scale
		glm::mat3 normalModelMatrix = glm::mat3(glm::transpose(glm::inverse(model)));

		//Create the model matrix for the light cube
		/*lightCubePos += glm::vec3(0.0f, 1.0f, 0.0f) * lightMoveDir * 1.0f * GetDeltaTime();
		if (lightCubePos.y > 2.5f)
			lightMoveDir = -1;
		else if (lightCubePos.y < -2.5f)
			lightMoveDir = 1;*/

		glm::vec3 lightCubeScale = glm::vec3(0.2f);
		glm::mat4 lightModel = glm::identity<glm::mat4>();
		lightModel = glm::translate(lightModel, lightCubePos);
		lightModel = glm::scale(lightModel, lightCubeScale);

		//Create the view matrix using camera lookAt target point
		glm::mat4 view = camera->GetLookAtMat(camera->GetCameraLocation() + camera->GetCameraForwardDir());

		//Create the projection matrix to project the view space to NDC
		glm::mat4 projection = glm::perspective(glm::radians(camera->GetCameraFOV()), float(GetWidth() / GetHeight()), 0.1f, 100.0f);


		//Use the Shader Program to draw Vertices using the defined vertex and fragment shaders, and apply model, view, projection matrices
		shader->Use();
		shader->SetMat44("model", model);
		shader->SetMat44("view", view);
		shader->SetMat44("projection", projection);
		shader->SetMat33("normalModelMatrix", normalModelMatrix);

		//Setting Object Material properties
		shader->SetVec3("material.ambient", objectColor);
		shader->SetVec3("material.diffuse", objectColor);
		shader->SetVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		shader->SetFloat("material.shininess", 32.0f);

		//Setting Light struct properties
		shader->SetVec3("light.sourcePos", lightCubePos);
		shader->SetVec3("light.ambient", 0.2f * lightColor);
		shader->SetVec3("light.diffuse", 0.5f * lightColor);
		shader->SetVec3("light.specular", 1.0f * lightColor);

		//Set the viewer (Camera) world position
		shader->SetVec3("cameraPos", camera->GetCameraLocation());

		//Bind the Object VAO
		glBindVertexArray(VAO);
		//Draw triangle using previous data
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);


		//Use the Light Shader Program ID to draw the light Cube, and apply model, view, projection matrices
		lightShader->Use();
		lightShader->SetMat44("model", lightModel);
		lightShader->SetMat44("view", view);
		lightShader->SetMat44("projection", projection);
		lightShader->SetVec3("lightColor", lightColor);

		//Bind the Object VAO
		glBindVertexArray(lightVAO);
		//Draw triangle using previous data
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
	}
}