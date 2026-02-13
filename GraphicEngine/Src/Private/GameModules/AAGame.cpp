#include "GameModules/AAGame.h"

AAGame::AAGame(int in_width, int in_height)
	: Game(in_width, in_height)
{
	fragmentShaderPath = "Shaders/AAScene/FragmentShader.glsl";
	vertexShaderPath = "Shaders/AAScene/VertexShader.glsl";

	ppFragmentShaderPath = "Shaders/AAScene/PPFragmentShader.glsl";
	ppVertexShaderPath = "Shaders/AAScene/PPVertexShader.glsl";

	currentSamplesNum = 1;
	prevSamplesNum = currentSamplesNum;
}

bool AAGame::Init()
{	
	if (!Game::Init())
		return false;

	try
	{
		//Create Shader Program from Class
		shader = std::make_shared<Shader>(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

		//Create PostProcess Quad Shader to render the Light Cube
		quadShader = std::make_unique<Shader>(ppVertexShaderPath.c_str(), ppFragmentShaderPath.c_str());

		if (!CreateFramebuffer(currentSamplesNum))
		{
			return false;
		}

		//Generate and bind the Intermediate Framebuffer
		glGenFramebuffers(1, &interimFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, interimFBO);
		//Generate Texture buffer to be used as Color Buffer for the FB
		glGenTextures(1, &screenTexture);
		glBindTexture(GL_TEXTURE_2D, screenTexture);
		//Init the Texture Buffer with empty data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GetWidth(), GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//Attach the texture as the color buffer of the Framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);
		//Unbind the generated color buffer texture
		glBindTexture(GL_TEXTURE_2D, 0);
		//Check if Framebuffer status is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Failed to initialize the Framebuffer properly" << std::endl;
			glBindBuffer(GL_FRAMEBUFFER, 0);
			return false;
		}
		//Unbind the Framebuffer
		glBindBuffer(GL_FRAMEBUFFER, 0);

		//Create VAO for the Post Process quad having only a single quad taking the whole screen real state
		glGenVertexArrays(1, &ppVAO);
		glBindVertexArray(ppVAO);
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
	if (currentSamplesNum != prevSamplesNum)
	{
		prevSamplesNum = currentSamplesNum;
		if (!CreateFramebuffer(currentSamplesNum))
			throw std::exception();
	}
}

void AAGame::DrawFrame()
{
	Game::DrawFrame();

	//Draw the main scene to the created Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, msFBO);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.03f, 0.03f, 0.03f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	DrawMainScene();

	//Blit the Multisample color texture from msFBO to normal 2D color texture in intermediate FBO
	glBindFramebuffer(GL_READ_FRAMEBUFFER, msFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, interimFBO);
	glBlitFramebuffer(0, 0, GetWidth(), GetHeight(), 0, 0, GetWidth(), GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

	//Bind the Main framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//Disable depth test
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawPPScene();
}

bool AAGame::CreateFramebuffer(int n_samples)
{
	//Generate and bind the Multisample Framebuffer
	glGenFramebuffers(1, &msFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, msFBO);
	if (n_samples > 1)
	{
		//Generate Texture buffer to be used as Color Buffer for the FB
		unsigned int msColorTexture;
		glGenTextures(1, &msColorTexture);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msColorTexture);
		//Init the Texture Buffer with empty data
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, n_samples, GL_RGB, GetWidth(), GetHeight(), GL_TRUE);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//Attach the texture as the color buffer of the Framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, msColorTexture, 0);
		//Unbind the generated color buffer texture
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		//Generate RenderBuffer to be used as Depth and stencil buffer for the FB
		unsigned int msRBO;
		glGenRenderbuffers(1, &msRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, msRBO);
		//Reserve memory for the RenderBuffer
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, n_samples, GL_DEPTH24_STENCIL8, GetWidth(), GetHeight());
		//Attach Renderbuffer to Framebuffer
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msRBO);
		//Unbind the generated Render Buffer
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
	else
	{
		//Generate Texture buffer to be used as Color Buffer for the FB
		unsigned int msColorTexture;
		glGenTextures(1, &msColorTexture);
		glBindTexture(GL_TEXTURE_2D, msColorTexture);
		//Init the Texture Buffer with empty data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GetWidth(), GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//Attach the texture as the color buffer of the Framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, msColorTexture, 0);
		//Unbind the generated color buffer texture
		glBindTexture(GL_TEXTURE_2D, 0);

		//Generate RenderBuffer to be used as Depth and stencil buffer for the FB
		unsigned int msRBO;
		glGenRenderbuffers(1, &msRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, msRBO);
		//Reserve memory for the RenderBuffer
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, GetWidth(), GetHeight());
		//Attach Renderbuffer to Framebuffer
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msRBO);
		//Unbind the generated Render Buffer
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	//Check if Framebuffer status is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Failed to initialize the Framebuffer properly" << std::endl;
		return false;
	}
	//Unbind the Framebuffer
	glBindBuffer(GL_FRAMEBUFFER, 0);
	return true;
}

void AAGame::DrawMainScene()
{
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

void AAGame::DrawPPScene()
{
	//Draw the quad from it's vertices, and use the color texture from our framebuffer as a texture sampled in the fragment shader
	//Check if the Quad Shader is Valid
	if (quadShader)
	{
		glBindVertexArray(ppVAO);
		quadShader->Use();
		//Activate a texture unit and bind the color texture to it
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, screenTexture);
		quadShader->SetInt("screenTexture", 0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
		//Unbind the texture and the VAO
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}
}

void AAGame::ProcessInput(GLFWwindow* window)
{
	Game::ProcessInput(window);
	//Change the MSAA samples count
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		currentSamplesNum = 1;
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		currentSamplesNum = 2;
	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		currentSamplesNum = 4;
	else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		currentSamplesNum = 8;
}