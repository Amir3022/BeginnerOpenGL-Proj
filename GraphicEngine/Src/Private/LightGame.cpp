#include "LightGame.h"

LightGame::LightGame(int in_width, int in_height)
	: Game(in_width, in_height)
{
	fragmentShaderPath = "Shaders/LightScene/FragmentShader.glsl";
	vertexShaderPath = "Shaders/LightScene/VertexShader.glsl";

	lightFragmentShaderPath = "Shaders/LightScene/LightFragmentShader.glsl";
	lightVertexShaderPath = "Shaders/LightScene/LightVertexShader.glsl";

	moveLightSource = true;
	changeLightColor = false;

	objectColor = glm::vec3(1.0f, 0.5f, 0.31f);
	spotLightColor = glm::vec3(2.0f, 2.0f, 2.0f);
	dirLightColor = glm::vec3(0.98f, 0.98f, 0.5f) * 0.4f;
	dirLightOrient = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
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
	   //Create a vertices array   (Vertex Location, normal direction, texture coordinates)
		float vertices[] =
		{
		   -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,	//0		//0
		   -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	//1
		   -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,	//2

		   0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,	1.0f, 0.0f,		//3		//1
		   0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,	0.0f, 0.0f,		//4
		   0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,	//5

		   -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,	0.0f, 1.0f,		//6		//2
		   -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,	//7
		   -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,	0.0f, 0.0f,		//8

		   0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,		//9		//3
		   0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,		//10
		   0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,		//11

		   -0.5f, -0.5f, -0.5, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,	//12	//4
		   -0.5f, -0.5f, -0.5, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,	//13
		   -0.5f, -0.5f, -0.5, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,	//14

		   0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,	//15	//5
		   0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	//16
		   0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,	//17

		   -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,	//18	//6
		   -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	//19
		   -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,	//20

		   0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,	//21	//7
		   0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,	1.0f, 1.0f,		//22
		   0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,	1.0f, 1.0f,		//23

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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //0 Attribute in the vertex shader, 3 vec3 has 3 float values, GL_FLOAT each input is a float value, GL_FALSE no normalization to be used, size of each stride to move to the next vertex, pointer to the first element in the Vertex Buffer
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		//Loading the Crate diffuse and specular textrue and Bind to Texture Buffer
		unsigned int diffuseMapTex = LoadImageIntoTexture("Assets/Textures/container2.png");
		unsigned int specularMapTex = LoadImageIntoTexture("Assets/Textures/container2_specular.png");
		unsigned int emissiveMapTex = LoadImageIntoTexture("Assets/Textures/matrix.jpg");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMapTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMapTex);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, emissiveMapTex);

		//Create a new Vertex Array Object for the Light Cube
		glGenVertexArrays(1, &lightVAO);
		glBindVertexArray(lightVAO);

		//Bind the same created VBO and EBO to the light VAO, no need to move data as it already was added to the container
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		//Set Vertex Attribute Data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //0 Attribute in the vertex shader, 3 vec3 has 3 float values, GL_FLOAT each input is a float value, GL_FALSE no normalization to be used, size of each stride to move to the next vertex, pointer to the first element in the Vertex Buffer
		glEnableVertexAttribArray(0);

		//Enable Depth Test to allow usage of Z-Buffer
		glEnable(GL_DEPTH_TEST);

		//Array for locations, rotations for 10 random cubes(x, y, z, rot orientation)
		cubeTransforms =
		{
		   {glm::vec3(-4.0f,  -0.8f,  -9.0f), glm::vec3(1.7f, -0.4f, 0.9f)},
		   {glm::vec3(2.0f,  5.0f, -6.0f), glm::vec3(-1.3f, 1.8f, -2.0f)},
		   {glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(0.5f, -1.9f, 1.2f)},
		   {glm::vec3(-3.8f, -2.0f, -5.3), glm::vec3(-0.8f, 0.3f, -1.5f)},
		   {glm::vec3(2.4f, -0.4f, -3.5f), glm::vec3(2.0f, 1.1f, -0.7f)},
		   {glm::vec3(-1.7f,  3.0f, -7.5f),glm::vec3(-1.6f, -1.0f, 1.4f)},
		   {glm::vec3(1.3f, -2.0f, -2.5f), glm::vec3(0.2f, 1.9f, -0.1f)},
		   {glm::vec3(1.5f,  2.0f, -2.5f), glm::vec3(1.5f, -2.0f, 0.6f)},
		   {glm::vec3(1.5f,  0.2f, -1.5f), glm::vec3(-0.9f, 0.0f, 1.7f)},
		   {glm::vec3(-1.3f,  1.0f, -1.5f), glm::vec3(1.2f, -1.4f, -0.5f)}
		};

		//Array of Locations for Point lights, movement directions, colors
		lightCubesPos =
		{
			glm::vec3(0.0f, 1.5f, 2.0f),
			glm::vec3(0.0f, 1.5f, -7.0f),
			glm::vec3(3.5f, 1.5f, -3.0f),
			glm::vec3(-3.5f, 1.5f, -3.0f),
		};

		lightMoveDirs =
		{
			1.0f, -1.0f, -1.0f, 1.0f,
		};

		lightMoveSpeeds =
		{
			1.0f, 5.0f, 0.5f, 2.0f
		};

		pointLightColors =
		{
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.5f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.5f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.5f),
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

void LightGame::Terminate()
{
	//De-allocate all used resources by vertex arrays and buffers
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteVertexArrays(1, &VAO);
}

void LightGame::UpdateGame(float deltaTime)
{
	Game::UpdateGame(deltaTime);

	//Update Light Source properties
	if (moveLightSource)
	{
		for (int i = 0; i < lightCubesPos.size(); i++)
		{
			glm::vec3& lightCubePos = lightCubesPos[i];
			float& lightMoveDir = lightMoveDirs[i];

			lightCubePos += glm::vec3(0.0f, 1.0f, 0.0f) * lightMoveDir * GetDeltaTime() * lightMoveSpeeds[i];
			if (lightCubePos.y > 2.5f)
				lightMoveDir = -1;
			else if (lightCubePos.y < -2.5f)
				lightMoveDir = 1;
		}
	}
	/*if (changeLightColor)
	{
		pointLightColor.x = (float)glm::abs(sin(glfwGetTime() * 0.7f));
		pointLightColor.y = (float)glm::abs(sin(glfwGetTime() * 1.0f));
		pointLightColor.z = (float)glm::abs(sin(glfwGetTime() * 3.1f));
	}*/
}

void LightGame::DrawFrame()
{
	Game::DrawFrame();

	glClearColor(0.03f, 0.03f, 0.03f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw triangle from vertices
	if (shader && lightShader && camera)
	{
		//Create Transform matrix to transform the drawn image
		//Create the model matrix to rotate the object in world space
		glm::mat4 model = glm::identity<glm::mat4>();

		//Create the Normal Model Matrix to convert normal from local space to World coordinates while respecting scale
		glm::mat3 normalModelMatrix = glm::mat3(glm::transpose(glm::inverse(model)));

		//Create the view matrix using camera lookAt target point
		glm::mat4 view = camera->GetLookAtMat(camera->GetCameraLocation() + camera->GetCameraForwardDir());

		//Create the projection matrix to project the view space to NDC
		glm::mat4 projection = glm::perspective(glm::radians(camera->GetCameraFOV()), (float)GetWidth() / (float)GetHeight(), 0.1f, 100.0f);


		//Use the Shader Program to draw Vertices using the defined vertex and fragment shaders, and apply model, view, projection matrices
		shader->Use();
		shader->SetMat44("model", model);
		shader->SetMat44("view", view);
		shader->SetMat44("projection", projection);
		shader->SetMat33("normalModelMatrix", normalModelMatrix);

		//Setting Object Material properties
		shader->SetInt("material.diffuse", 0);
		shader->SetInt("material.specular", 1);
		//shader->SetInt("material.emissive", 2);
		//shader->SetFloat("material.emissiveAmount", 1.0f);
		shader->SetFloat("material.shininess", 32.0f);

		//Set the viewer (Camera) world position
		shader->SetVec3("cameraPos", camera->GetCameraLocation());

		//Setting Light struct properties (Setup the light Source as a flashlight, a spotlight originating from camera position)
		shader->SetVec3("spotLight.sourcePos", camera->GetCameraLocation());
		shader->SetVec3("spotLight.sourceDir", camera->GetCameraForwardDir());
		shader->SetFloat("spotLight.innerRadiusCos", glm::cos(glm::radians(12.5f)));
		shader->SetFloat("spotLight.outerRadiusCos", glm::cos(glm::radians(15.0f)));
		shader->SetVec3("spotLight.light.ambient", 0.1f * spotLightColor);
		shader->SetVec3("spotLight.light.diffuse", 0.75f * spotLightColor);
		shader->SetVec3("spotLight.light.specular", 1.0f * spotLightColor);
		shader->SetFloat("spotLight.constant", 1.0f);	//Attenuation constants for a light source that covers and outer radius on 50 units
		shader->SetFloat("spotLight.linear", 0.09f);
		shader->SetFloat("spotLight.quad", 0.032f);

		//Rendering point lights
		for (int i = 0; i < lightCubesPos.size(); i++)
		{
			shader->SetVec3("pointLights[" + std::to_string(i) + "].sourcePos", lightCubesPos[i]);
			shader->SetVec3("pointLights[" + std::to_string(i) + "].light.ambient", 0.1f * glm::normalize(pointLightColors[i]));
			shader->SetVec3("pointLights[" + std::to_string(i) + "].light.diffuse", 0.75f * pointLightColors[i]);
			shader->SetVec3("pointLights[" + std::to_string(i) + "].light.specular", 1.0f * pointLightColors[i]);
			shader->SetFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);	//Attenuation constants for a light source that covers and outer radius on 50 units
			shader->SetFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
			shader->SetFloat("pointLights[" + std::to_string(i) + "].quad", 0.032f);
		}

		//Rendering directional Light
		shader->SetVec3("dirLight.sourceDir", dirLightOrient);
		shader->SetVec3("dirLight.light.ambient", 0.1f * dirLightColor);
		shader->SetVec3("dirLight.light.diffuse", 0.75f * dirLightColor);
		shader->SetVec3("dirLight.light.specular", 1.0f * dirLightColor);

		//Bind the Object VAO
		glBindVertexArray(VAO);
		//Draw triangle using previous data
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);

		//Draw other 9 cubes
		for (int i = 1; i < cubeTransforms.size(); i++)
		{
			model = glm::identity<glm::mat4>();
			float angle = (i + 1) * 20.0f * glfwGetTime();
			model = glm::translate(model, cubeTransforms[i].pos);
			model = glm::rotate(model, glm::radians(angle), cubeTransforms[i].orient);
			normalModelMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
			shader->SetMat44("model", model);
			shader->SetMat33("normalModelMatrix", normalModelMatrix);

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
		}


		//Use the Light Shader Program ID to draw the light Cube, and apply model, view, projection matrices
		//Bind the Object VAO
		glBindVertexArray(lightVAO);
		lightShader->Use();
		for (int i = 0; i < lightCubesPos.size(); i++)
		{
			glm::vec3 lightCubePos = lightCubesPos[i];
			//Create the model matrix for the light cube
			glm::vec3 lightCubeScale = glm::vec3(0.2f);
			glm::mat4 lightModel = glm::identity<glm::mat4>();
			lightModel = glm::translate(lightModel, lightCubePos);
			lightModel = glm::scale(lightModel, lightCubeScale);

			lightShader->SetMat44("model", lightModel);
			lightShader->SetMat44("view", view);
			lightShader->SetMat44("projection", projection);
			lightShader->SetVec3("lightColor", pointLightColors[i]);

			//Draw triangle using previous data
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
		}
	}
}