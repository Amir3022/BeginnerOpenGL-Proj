#include "GameModules/DeferredLightingGame.h"
#include <random>

DeferredLightingGame::DeferredLightingGame(int in_width, int in_height)
	: Game(in_width, in_height)
{
	fragmentShaderPath = "Shaders/DeferredLightingScene/FragmentShader.glsl";
	vertexShaderPath = "Shaders/DeferredLightingScene/VertexShader.glsl";

	quadFragmentShaderPath = "Shaders/DeferredLightingScene/QuadFragmentShader.glsl";
	quadVertexShaderPath = "Shaders/DeferredLightingScene/QuadVertexShader.glsl";

	debugFragmentShaderPath = "Shaders/DeferredLightingScene/DebugFragmentShader.glsl";
	debugVertexShaderPath = "Shaders/DeferredLightingScene/DebugVertexShader.glsl";

	DrawMode = 0;
}

bool DeferredLightingGame::Init()
{
	if (!Game::Init())
		return false;

	try
	{
		//Create Shader Program from Class
		shader = std::make_shared<Shader>(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

		//Create Shader to draw on the screen quad
		debugQuadShader = std::make_shared<Shader>(debugVertexShaderPath.c_str(), debugFragmentShaderPath.c_str());

		//Create Shader to Draw Quad with deferred lighting calculations on Screen
		DLQuadShader = std::make_shared<Shader>(quadVertexShaderPath.c_str(), quadFragmentShaderPath.c_str());

		//Change camera initial location
		camera->SetCameraLocation(glm::vec3(0.0f, 5.0f, 15.0f));
		camera->SetCameraRotation(camera->GetCameraRotation() + glm::vec3(0.0f, -15.0f, 0.0f));

		//Create a 9 Model Instances at a 9 grid shape
		model = std::make_shared<TBNModel>("Assets/Meshes/backpack/backpack.obj");
		modelPositions = 
		{
			glm::vec3(-5.0f, 0.0f, -5.0f),
			glm::vec3(0.0f, 0.0f, -5.0f),
			glm::vec3(5.0f, 0.0f, -5.0f),

			glm::vec3(-5.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(5.0f, 0.0f, 0.0f),

			glm::vec3(-5.0f, 0.0f, 5.0f),
			glm::vec3(0.0f, 0.0f, 5.0f),
			glm::vec3(5.0f, 0.0f, 5.0f),
		};

		//Generate GBuffer Framebuffer and attach to it 3 color attachments for position, normal, albedoSpecular buffers
		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		//Generate Position Buffer
		glGenTextures(1, &positionBuffer);
		glBindTexture(GL_TEXTURE_2D, positionBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, GetWidth(), GetHeight(), 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, positionBuffer, 0);

		//Generate Normal Buffer
		glGenTextures(1, &normalBuffer);
		glBindTexture(GL_TEXTURE_2D, normalBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, GetWidth(), GetHeight(), 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalBuffer, 0);

		//Generate Albedo Specular Buffer
		glGenTextures(1, &albedoSpecBuffer);
		glBindTexture(GL_TEXTURE_2D, albedoSpecBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GetWidth(), GetHeight(), 0, GL_RGBA, GL_UNSIGNED_INT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedoSpecBuffer, 0);

		//Create RenderBuffer for depth and stencil Info
		unsigned int RBO;
		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, GetWidth(), GetHeight());
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
		
		//Check the Framebuffer is properly attached
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Failed to create gBuffer Framebuffer" << std::endl;
			return false;
		}
		//Unbind framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		//Create VAO for the Quad having only a single quad taking the whole screen real state
		glGenVertexArrays(1, &quadVAO);
		glBindVertexArray(quadVAO);
		//Create Vertex Buffer and Elements Buffer and fill with a single Quad data
		float quadVertices[] = {
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		};

		unsigned int quadIndices[] =
		{
			0, 1, 3,
			0, 3, 2,
		};
		//Create vertex and Element buffers
		unsigned int ppVBO, ppEBO;
		glGenBuffers(1, &ppVBO);
		glBindBuffer(GL_ARRAY_BUFFER, ppVBO);
		glGenBuffers(1, &ppEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ppEBO);
		//Assign data to the buffers
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
		//Declate Vertex attributes
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		//Unbind the PPVAO
		glBindVertexArray(0);

		//Generate random variables for point light positions and colors
		NR_LIGHTS = 32;
		std::mt19937 rng(glfwGetTime());
		std::uniform_real_distribution<float> posDist(-5.0f, 5.0f);
		std::uniform_real_distribution<float> colorDist(0.5f, 1.0f);
		for (unsigned int i = 0; i < NR_LIGHTS; i++)
		{
			// calculate slightly random offsets
			float xPos = posDist(rng);
			float yPos = posDist(rng);
			float zPos = posDist(rng);
			pointLightPositions.push_back(glm::vec3(xPos, yPos, zPos));
			// also calculate random color
			float rColor = colorDist(rng);
			float gColor = colorDist(rng);
			float bColor = colorDist(rng);
			pointLightColors.push_back(glm::vec3(rColor, gColor, bColor));
		}

		return true;
	}
	catch (std::exception e)
	{
		std::cout << "Shader creation threw an exception!" << std::endl;
		glfwTerminate();
		return false;
	}
}

void DeferredLightingGame::Terminate()
{
	Game::Terminate();
}

void DeferredLightingGame::UpdateGame(float deltaTime)
{
	Game::UpdateGame(deltaTime);
}

void DeferredLightingGame::DrawFrame()
{
	Game::DrawFrame();
	//Bind the GBuffer to draw into the Color Buffers attached
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	//Set the Clear color and Clear the color and depth buffers
	glClearColor(0.03f, 0.03f, 0.03f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Enable Depth test
	glEnable(GL_DEPTH_TEST);
	//Enable Multi Render Target (MRT) on all the attached color buffer attachments
	unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
	//Draw the Main scene into the attached buffers
	DrawMainScene();

	//Bind the main framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//Set the Clear color and Clear the color and depth buffers
	glClearColor(0.03f, 0.03f, 0.03f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Disable Depth test
	glDisable(GL_DEPTH_TEST);

	//Draw Deferred Lighting Quad
	DrawDeferredLightingQuad();

	////Draw Debug render quad
	//DrawDebugRenderQuad();
}

void DeferredLightingGame::DrawMainScene()
{
	//If model is valid, draw it
	if (shader && model)
	{
		//Use the Shader Program to draw Vertices using the defined vertex and fragment shaders, and apply model, view, projection matrices
		shader->Use();
		//Create the view matrix using camera lookAt target point
		glm::mat4 view = camera->GetLookAtMat(camera->GetCameraLocation() + camera->GetCameraForwardDir());
		//Create the projection matrix to project the view space to NDC
		glm::mat4 projection = glm::perspective(glm::radians(camera->GetCameraFOV()), (float)GetWidth() / (float)GetHeight(), 0.1f, 100.0f);

		//Set the View and projection matrices on all to be drawn models
		shader->SetMat44("view", view);
		shader->SetMat44("projection", projection);

		//Set the viewer (Camera) world position
		shader->SetVec3("cameraPos", camera->GetCameraLocation());

		//Iterate over all model instances and draw each one of them
		for (const glm::vec3& modelPosition : modelPositions)
		{
			//Create the model matrix to rotate the object in world space
			glm::mat4 modelMat = glm::identity<glm::mat4>();
			modelMat = glm::translate(modelMat, modelPosition);
			modelMat = glm::rotate(modelMat, glm::radians(model->GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
			modelMat = glm::rotate(modelMat, glm::radians(model->GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
			modelMat = glm::rotate(modelMat, glm::radians(model->GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
			modelMat = glm::scale(modelMat, model->GetScale());
			//Create the Normal Model Matrix to convert normal from local space to World coordinates while respecting scale
			glm::mat3 normalModelMatrix = glm::mat3(glm::transpose(glm::inverse(modelMat)));

			//Set the model and normal Model Matrix in the shader
			shader->SetMat44("model", modelMat);
			shader->SetMat33("normalModelMatrix", normalModelMatrix);

			//Draw Model Instance
			model->Draw(shader);
		}
	}
}

void DeferredLightingGame::DrawDeferredLightingQuad()
{
	//Check if QuadShader is valid
	if (DLQuadShader)
	{
		//Bind the VAO holding the quad info
		glBindVertexArray(quadVAO);
		//Use the QuadShader Program to draw the desired color texture on the quad
		DLQuadShader->Use();
		//Attach geometry layer buffers to different textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, positionBuffer);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalBuffer);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, albedoSpecBuffer);
		DLQuadShader->SetInt("positionBuffer", 0);
		DLQuadShader->SetInt("normalBuffer", 1);
		DLQuadShader->SetInt("albedoSpecBuffer", 2);

		//Set the Camera position uniform
		DLQuadShader->SetVec3("cameraPos", camera->GetCameraLocation());

		//Set point Light properties uniform in Shader
		for (int i = 0; i < NR_LIGHTS; i++)
		{
			DLQuadShader->SetVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
			DLQuadShader->SetVec3("pointLights[" + std::to_string(i) + "].color", pointLightColors[i]);
			DLQuadShader->SetFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
			DLQuadShader->SetFloat("pointLights[" + std::to_string(i) + "].linear", 0.7f);
			DLQuadShader->SetFloat("pointLights[" + std::to_string(i) + "].quadratic", 1.8f);
		}

		//Draw the quad using Shaders
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

		//Unbind the texture and the VAO
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}
}

void DeferredLightingGame::DrawDebugRenderQuad()
{
	//Check if QuadShader is valid
	if (debugQuadShader)
	{
		//Bind the VAO holding the quad info
		glBindVertexArray(quadVAO);
		//Use the QuadShader Program to draw the desired color texture on the quad
		debugQuadShader->Use();
		//Attach geometry layer buffers to different textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, positionBuffer);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalBuffer);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, albedoSpecBuffer);
		debugQuadShader->SetInt("positionBuffer", 0);
		debugQuadShader->SetInt("normalBuffer", 1);
		debugQuadShader->SetInt("albedoSpecBuffer", 2);
		//Set the current buffer to be drawn
		debugQuadShader->SetInt("DrawMode", DrawMode);

		//Draw the quad using Shaders
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

		//Unbind the texture and the VAO
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}
}

void DeferredLightingGame::ProcessInput(GLFWwindow* window)
{
	Game::ProcessInput(window);

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		DrawMode = 0;
	}
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		DrawMode = 1;
	}
	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		DrawMode = 2;
	}
	else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		DrawMode = 3;
	}
}
