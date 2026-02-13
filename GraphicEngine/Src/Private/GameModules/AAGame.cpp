#include "GameModules/AAGame.h"

AAGame::AAGame(int in_width, int in_height)
	: Game(in_width, in_height)
{
	fragmentShaderPath = "Shaders/AAScene/FragmentShader.glsl";
	vertexShaderPath = "Shaders/AAScene/VertexShader.glsl";

	ppFragmentShaderPath = "Shaders/AAScene/PPFragmentShader.glsl";
	ppVertexShaderPath = "Shaders/AAScene/PPVertexShader.glsl";

	bWasChangeMSAAPressed = false;
	bUseMSAA = true;
}

bool AAGame::Init()
{	
	if (!Game::Init())
		return false;

	try
	{
		//Create Shader Program from Class
		shader = std::make_shared<Shader>(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

		//Create Post Process Shader
		ppShader = std::make_shared<Shader>(ppVertexShaderPath.c_str(), ppFragmentShaderPath.c_str());

		//Generate Multisample Framebuffer with 4 samples
		glGenFramebuffers(1, &msFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, msFBO);

		//Generate multisampled Texture2D and RenderBuffer object to be used with the multisampled FBO
		unsigned int msTex;
		glGenTextures(1, &msTex);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msTex);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, GetWidth(), GetHeight(), GL_TRUE);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//Bind the Multisampled Texture to the FrameBuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, msTex, 0);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		//Generate Render Buffer Object
		unsigned int msRBO;
		glGenRenderbuffers(1, &msRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, msRBO);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, GetWidth(), GetHeight());
		//Bind the Multisampled RenderTarget to the Framebuffer
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Failed to initialize multisample framebuffer" << std::endl;
			return false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

		//Create Interim Framebuffer used to Blit the multisample framebuffer
		glGenFramebuffers(1, &interimFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, interimFBO);
		//Generate Texture 2D to be used as color attachment from Framebuffer
		glGenTextures(1, &outTex);
		glBindTexture(GL_TEXTURE_2D, outTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GetWidth(), GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outTex, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		//Generate RenderBuffer to be used as Depth, Stencil buffer for the Interim Framebuffer
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Failed to initialize multisample framebuffer" << std::endl;
			return false;
		}
		//Unbind the interim Framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Create VAO for the main scene containing the lime green cube
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
}

void AAGame::DrawFrame()
{
	Game::DrawFrame();

	//Draw the Main scene in the Multisampled Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, msFBO);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	DrawMainScene();

	//Blit the Multsampled color buffer to the interim color buffer to have a texture output able to be sampled in main framebuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, msFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, interimFBO);
	glBlitFramebuffer(0, 0, GetWidth(), GetHeight(), 0, 0, GetWidth(), GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

	//Bind the Main Framebuffer and sample the output texture to the Fragment Shader
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//Disable depth test
	//glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	//Draw the post process Quad
	DrawPPScene();
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

		//Unbind VAO
		glBindVertexArray(0);
	}
}

void AAGame::DrawPPScene()
{
	//Draw the quad with the multisampled texture
	//Check if the PP Shader is valid
	if (ppShader)
	{
		//Bind the ppVAO
		glBindVertexArray(ppVAO);

		//Use the Post Process Shader Program
		ppShader->Use();

		//Activate Texture Location 0, and bind the output texture to it
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, outTex);

		//Set the Texture location 0 sample data
		ppShader->SetInt("PPTex", 0);

		//Draw the Quad using it's elements
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

		//Unbind the Texture
		glBindTexture(GL_TEXTURE_2D, 0);

		//Unbind the ppVAO
		glBindVertexArray(0);
	}
}
