#include "LightGame.h"

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
	   //Create a vertices array   (Vertex Location)
		float vertices[] =
		{
		   -0.5f, -0.5f, 0.5f,
		   0.5f, -0.5f, 0.5f,
		   -0.5f, 0.5f, 0.5f,
		   0.5f, 0.5f, 0.5f,

		   -0.5f, -0.5f, -0.5,
		   0.5f, -0.5f, -0.5f,
		   -0.5f, 0.5f, -0.5f,
		   0.5f, 0.5f, -0.5f,
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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); //0 Attribute in the vertex shader, 3 vec3 has 3 float values, GL_FLOAT each input is a float value, GL_FALSE no normalization to be used, size of each stride to move to the next vertex, pointer to the first element in the Vertex Buffer
		glEnableVertexAttribArray(0);

		//Create a new Vertex Array Object for the Light Cube
		glGenVertexArrays(1, &lightVAO);
		glBindVertexArray(lightVAO);

		//Bind the same created VBO and EBO to the light VAO, no need to move data as it already was added to the container
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		//Set Vertex Attribute Data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); //0 Attribute in the vertex shader, 3 vec3 has 3 float values, GL_FLOAT each input is a float value, GL_FALSE no normalization to be used, size of each stride to move to the next vertex, pointer to the first element in the Vertex Buffer
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
	if (shader && camera)
	{
		//Use the Shader Program to draw Vertices using the defined vertex and fragment shaders
		shader->Use();

		//Create Transform matrix to transform the drawn image
		//First, create the model matrix to rotate the object in world space
		glm::mat4 model = glm::identity<glm::mat4>();
		shader->SetMat44("model", model);

		//Second, create the view matrix using camera lookAt target point
		glm::mat4 view = camera->GetLookAtMat(camera->GetCameraLocation() + camera->GetCameraForwardDir());
		shader->SetMat44("view", view);

		//Third, create the projection matrix to project the view space to NDC
		glm::mat4 projection = glm::perspective(glm::radians(camera->GetCameraFOV()), float(GetWidth() / GetHeight()), 0.1f, 100.0f);
		shader->SetMat44("projection", projection);

		//Set the Object and the Light color in the Fragment Shader
		shader->SetVec3("objectColor", 1.0f, 1.0f, 0.0f);
		shader->SetVec3("lightColor", 1.0f, 1.0f, 1.0f);

		//Change the ambient scaler using a sin function
		float ambientScaler = 0.5f;
		shader->SetFloat("ambientScaler", ambientScaler);

		//Bind the Object VAO
		glBindVertexArray(VAO);

		//Draw triangle using previous data
		glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, (void*)0);


		//Use the Light Shader Program ID to draw the light Cube
		lightShader->Use();
		//Create Transform matrix to transform the drawn image
		//First, create the model matrix to rotate the object in world space
		glm::vec3 lightCubePos = glm::vec3(1.2f, 1.0f, 2.0f);
		glm::vec3 lightCubeScale = glm::vec3(0.5f);
		glm::mat4 lightModel = glm::identity<glm::mat4>();
		lightModel = glm::translate(lightModel, lightCubePos);
		lightModel = glm::scale(lightModel, lightCubeScale);
		lightShader->SetMat44("model", lightModel);

		//Second, create the view matrix using camera lookAt target point
		glm::mat4 lightView = camera->GetLookAtMat(camera->GetCameraLocation() + camera->GetCameraForwardDir());
		lightShader->SetMat44("view", lightView);

		//Third, create the projection matrix to project the view space to NDC
		glm::mat4 lightProjection = glm::perspective(glm::radians(camera->GetCameraFOV()), float(GetWidth() / GetHeight()), 0.1f, 100.0f);
		lightShader->SetMat44("projection", lightProjection);

		//Bind the Object VAO
		glBindVertexArray(lightVAO);

		//Draw triangle using previous data
		glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, (void*)0);
	}
}