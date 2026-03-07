#include "GameModules/BloomGame.h"
#include "Utilities/EngineUtilities.h"

BloomGame::BloomGame(int in_width, int in_height)
	: Game(in_width, in_height)
{
	vertexShaderPath = "Shaders/BloomScene/VertexShader.glsl";
	fragmentShaderPath = "Shaders/BloomScene/FragmentShader.glsl";

	ppVertexShaderPath = "Shaders/BloomScene/ppVertexShader.glsl";
	ppFragmentShaderPath = "Shaders/BloomScene/ppFragmentShader.glsl";

	lightFragmentShaderPath = "Shaders/BloomScene/LightFragmentShader.glsl";
	lightVertexShaderPath = "Shaders/BloomScene/LightVertexShader.glsl";

	bUseHDR = false;
	exposureValue = 1.0f;

	bHDRTogglePressed = false;

	//Initialize Light Variables
	pointLightsPos =
	{
		glm::vec3(0.0f, 4.5f, -2.0f),
		glm::vec3(-3.0f, 2.0f, -2.0f),
		glm::vec3(2.0f, 1.5f, 3.0f),
	};
	pointLightsColor =
	{
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
	};
}

bool BloomGame::Init()
{
	if (!Game::Init())
		return false;

	try
	{
		//Create Shader Program from Class
		shader = std::make_shared<Shader>(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

		//Create Simple Shadow Shader 
		ppShader = std::make_shared<Shader>(ppVertexShaderPath.c_str(), ppFragmentShaderPath.c_str());

		//Create Light Shader to render point lights
		lightShader = std::make_shared<Shader>(lightVertexShaderPath.c_str(), lightFragmentShaderPath.c_str());

		//Change Camera transform
		camera->SetCameraLocation(camera->GetCameraLocation() + glm::vec3(0.0f, 0.0f, 5.0f));
		camera->SetCameraRotation(camera->GetCameraRotation() + glm::vec3(0.0f, 0.0f, 0.0f));

		//Create a  cube vertices array   (Vertex Location, Vertex Normal, Texture Coordinate)
		std::vector<Vertex> vertices =
		{
			{glm::vec3(-1.0f, -1.0f, 1.0f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f)},		//0		//0
			{glm::vec3(-1.0f, -1.0f, 1.0f),  glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)},	//1
			{glm::vec3(-1.0f, -1.0f, 1.0f),  glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},		//2

			{glm::vec3(1.0f, -1.0f, 1.0f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f)},		//3		//1
			{glm::vec3(1.0f, -1.0f, 1.0f),  glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f)},		//4
			{glm::vec3(1.0f, -1.0f, 1.0f),  glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f)},		//5

			{glm::vec3(-1.0f,  1.0f, 1.0f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f)},		//6		//2
			{glm::vec3(-1.0f,  1.0f, 1.0f),  glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f)},	//7
			{glm::vec3(-1.0f,  1.0f, 1.0f),  glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},		//8

			{glm::vec3(1.0f,  1.0f, 1.0f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f)},		//9		//3
			{glm::vec3(1.0f,  1.0f, 1.0f),  glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)},		//10
			{glm::vec3(1.0f,  1.0f, 1.0f),  glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f)},		//11

			{glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},		//12	//4
			{glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f)},	//13
			{glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},		//14

			{glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f)},		//15	//5
			{glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)},		//16
			{glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f)},		//17

			{glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f)},		//18	//6
			{glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)},	//19
			{glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},		//20

			{glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f)},		//21	//7
			{glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f)},		//22
			{glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f)},		//23
		};

		std::vector<unsigned int> indices =
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

		//Declare Vertices for a 2D plane
		std::vector<Vertex> planeVertices =
		{
			{glm::vec3(-1.0f, -1.0f, 0.0f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f)},		//0		//0
			{glm::vec3(-1.0, -1.0f, -0.0001f),  glm::vec3(0.0f,  0.0f,  -1.0f), glm::vec2(0.0f, 0.0f)},	//1	

			{glm::vec3(1.0f, -1.0f, 0.0f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f)},		//2		//1
			{glm::vec3(1.0f, -1.0f, -0.0001f),  glm::vec3(0.0f,  0.0f,  -1.0f), glm::vec2(1.0f, 0.0f)},	//3

			{glm::vec3(-1.0f,  1.0f, 0.0f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f)},		//4		//2
			{glm::vec3(-1.0f,  1.0f, -0.0001f),  glm::vec3(0.0f,  0.0f,  -1.0f), glm::vec2(0.0f, 1.0f)},//5

			{glm::vec3(1.0f,  1.0f, 0.0f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f)},		//6		//3
			{glm::vec3(1.0f,  1.0f, -0.0001f),  glm::vec3(0.0f,  0.0f,  -1.0f), glm::vec2(1.0f, 1.0f)},	//7
		};
		std::vector<unsigned int> planeIndices =
		{
			0, 2, 4,
			4, 2, 6,

			1, 3, 5,
			5, 3, 7
		};

		//Load Wooden floor Image into texture
		unsigned int texture1 = EngineUtilities::LoadImageIntoTexture("Assets/Textures/WoodenFloor.png", false, true);

		//Load textures for wooden box with metal frame
		unsigned int texture2 = EngineUtilities::LoadImageIntoTexture("Assets/Textures/container2.png", false, true);
		unsigned int texture3 = EngineUtilities::LoadImageIntoTexture("Assets/Textures/container2_specular.png", false, true);

		//Create Plane Mesh to be used as floor 
		//Create Plane Texture Object
		Texture texture_Plane_diffuse;
		texture_Plane_diffuse.texIndex = texture1;
		texture_Plane_diffuse.texType = ETextureType::diffuse;
		texture_Plane_diffuse.path = "Assets/Textures/WoodenFloor.png";
		std::vector<Texture> textures_Plane{ texture_Plane_diffuse };
		planeMesh = std::make_shared<Mesh>(planeVertices, planeIndices, textures_Plane);
		//Set Plane Transform
		planeMesh->SetTransform(glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(90.0f, 0.0f, 0.0f), glm::vec3(10.0f));

		//Create texture box for the wooden boxes
		Texture texture_2_diffuse;
		texture_2_diffuse.texIndex = texture2;
		texture_2_diffuse.texType = ETextureType::diffuse;
		texture_2_diffuse.path = "Assets/Textures/container2.png";
		Texture texture_2_specular;
		texture_2_specular.texIndex = texture3;
		texture_2_specular.texType = ETextureType::specular;
		texture_2_specular.path = "Assets/Textures/container2_specular.png";
		std::vector<Texture>textures_2{ texture_2_diffuse, texture_2_specular };

		//Declare cubes positions
		std::vector<glm::vec3> cubePositions
		{
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(-2.0f, 0.0f, -5.0f),
			glm::vec3(0.0f, 3.0f, 0.0f),
		};

		//Declare cubes rotations
		std::vector<glm::vec3> cubeRotations
		{
			glm::vec3(14.5f,  137.0f,  -78.0f),
			glm::vec3(-62.0f,  -19.5f,   91.0f),
			glm::vec3(38.0f, -164.0f,   22.5f),
		};

		//Create Cube Meshes
		for (int i = 0; i < (int)cubePositions.size(); i++)
		{
			std::shared_ptr<Mesh> cubeMesh = std::make_shared<Mesh>(vertices, indices, textures_2);
			//Set First Cube Transform
			cubeMesh->SetTransform(cubePositions[i], cubeRotations[i], glm::vec3(1.0f));
			//Add First cube to meshes Vector
			cubeMeshes.push_back(cubeMesh);
		}


		//Create a cube mesh to represent the point light position
		lightCubeMesh = std::make_shared<Mesh>(vertices, indices, textures_2);
		//Set lightCubeMesh transform
		lightCubeMesh->SetTransform(glm::vec3(0.0f), glm::vec3(0.f), glm::vec3(0.25f, 0.25f, 0.25f));	//Scale the cube down so it doesn't take much screen realstate

		//Create framebuffer with 2 color buffers to hold the output color and the brightness color buffers
		glGenFramebuffers(1, &mainFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
		//Generate 2 Textures for the output and brightness color buffers
		glGenTextures(2, colorBuffers);
		for (int i = 0; i < 2; i++)
		{
			glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, GetWidth(), GetHeight(), 0, GL_RGBA, GL_FLOAT, nullptr);

			//Set Bound textures parameteres
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			//Bind the texture to the framebuffer to be used as color attachment
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, colorBuffers[i], 0);
		}
		//Generate Depth Stencil Renderbuffer
		unsigned int RBO;
		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		//Reserve Renderbuffer storage
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, GetWidth(), GetHeight());
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
		//Check if the Framebuffer is properly attached
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Failed to create color and brightness Framebuffer" << std::endl;
			return false;
		}
		//Unbind framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

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

		return true;
	}
	catch (std::exception e)
	{
		std::cout << "Shader creation threw an exception!" << std::endl;
		glfwTerminate();
		return false;
	}
}

void BloomGame::Terminate()
{
	Game::Terminate();
}

void BloomGame::UpdateGame(float deltaTime)
{
	Game::UpdateGame(deltaTime);
}

void BloomGame::DrawFrame()
{
	Game::DrawFrame();

	//Bind the shadow Map Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
	//Enable Depth Testing, and clear color and depth buffers
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//Enable to draw in 2 Color Buffers (Using Multi Render Targets MTR)
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	//Draw the main scene to the color and brightness buffers
	DrawMainScene();

	//Bind the main framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//Enable Depth Testing, and clear color and depth buffers
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//Draw the color Texture on the PP Quad
	glEnable(GL_DEPTH_TEST);
	DrawPPScene();
}

void BloomGame::DrawMainScene()
{
	//Check if the meshes is valid
	if (shader)
	{
		//Create the View matrix to see the plane model through the camera position
		glm::mat4 view = camera->GetLookAtMat(camera->GetCameraLocation() + camera->GetCameraForwardDir());
		//Create projection matrix to project the scene with depth through camera lens field of view and project view space to NDC
		glm::mat4 projection = glm::perspective(glm::radians(camera->GetCameraFOV()), (float)GetWidth() / (float)GetHeight(), 0.1f, 1000.0f);

		//Use the shader program, and set the matrices
		shader->Use();
		shader->SetMat44("view", view);
		shader->SetMat44("projection", projection);

		//Set the viewer (Camera) world position
		shader->SetVec3("cameraPos", camera->GetCameraLocation());

		//Rendering point Lights
		for (int i = 0; i < (int)pointLightsPos.size(); i++)
		{
			shader->SetVec3("pointLights[" + std::to_string(i) + "].sourcePos", pointLightsPos[i]);
			shader->SetVec3("pointLights[" + std::to_string(i) + "].light.ambient", 0.2f * glm::normalize(pointLightsColor[i]));
			shader->SetVec3("pointLights[" + std::to_string(i) + "].light.diffuse", 0.75f * pointLightsColor[i]);
			shader->SetVec3("pointLights[" + std::to_string(i) + "].light.specular", 1.0f * pointLightsColor[i]);
			shader->SetFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);	//Attenuation constants for a light source that covers and outer radius on 50 units
			shader->SetFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
			shader->SetFloat("pointLights[" + std::to_string(i) + "].quad", 0.032f);
		}

		//Rendering cube Meshes
		for (int i = 0; i < cubeMeshes.size(); i++)
		{
			std::shared_ptr<Mesh> cubeMesh = cubeMeshes[i];
			if (cubeMesh)
			{
				//Create a model matrix to set plane location in world coordinates
				glm::mat4 modelMat = glm::identity < glm::mat4>();
				modelMat = glm::translate(modelMat, cubeMesh->GetPosition());
				modelMat = glm::rotate(modelMat, glm::radians(cubeMesh->GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
				modelMat = glm::rotate(modelMat, glm::radians(cubeMesh->GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
				modelMat = glm::rotate(modelMat, glm::radians(cubeMesh->GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
				modelMat = glm::scale(modelMat, cubeMesh->GetScale());
				//Create normal model matrix to transform fragment normals
				glm::mat3 normalModelMatrix = glm::mat3(glm::transpose(glm::inverse(modelMat)));

				//Set changing Model and normal model matrices
				shader->SetMat44("model", modelMat);
				shader->SetMat33("normalModelMatrix", normalModelMatrix);

				//Disable using tiling uniform for all cube meshes
				shader->SetBool("bUseTiling", false);

				//Draw the Wooden floor mesh
				cubeMesh->Draw(shader);
			}
		}

		//Rendering Plane Mesh
		if (planeMesh)
		{
			//Create a model matrix to set plane location in world coordinates
			glm::mat4 modelMat = glm::identity < glm::mat4>();
			modelMat = glm::translate(modelMat, planeMesh->GetPosition());
			modelMat = glm::rotate(modelMat, glm::radians(planeMesh->GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
			modelMat = glm::rotate(modelMat, glm::radians(planeMesh->GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
			modelMat = glm::rotate(modelMat, glm::radians(planeMesh->GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
			modelMat = glm::scale(modelMat, planeMesh->GetScale());
			//Create normal model matrix to transform fragment normals
			glm::mat3 normalModelMatrix = glm::mat3(glm::transpose(glm::inverse(modelMat)));

			//Set changing Model and normal model matrices
			shader->SetMat44("model", modelMat);
			shader->SetMat33("normalModelMatrix", normalModelMatrix);

			//Enable using tiling uniform for all wall meshes
			shader->SetBool("bUseTiling", true);

			//Draw the Wooden floor mesh
			planeMesh->Draw(shader);
		}

		//Rendering point light cube
		//Enable the Light Shader program
		if (lightShader && lightCubeMesh)
		{
			lightShader->Use();

			lightShader->SetMat44("view", view);
			lightShader->SetMat44("projection", projection);

			for (int i = 0; i < (int)pointLightsPos.size(); i++)
			{
				//Create a model matrix to set plane location in world coordinates
				glm::mat4 modelMat = glm::identity < glm::mat4>();
				modelMat = glm::translate(modelMat, pointLightsPos[i]);
				modelMat = glm::rotate(modelMat, glm::radians(lightCubeMesh->GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
				modelMat = glm::rotate(modelMat, glm::radians(lightCubeMesh->GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
				modelMat = glm::rotate(modelMat, glm::radians(lightCubeMesh->GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
				modelMat = glm::scale(modelMat, lightCubeMesh->GetScale());

				//Set the matrices uniforms in the Light Shader
				lightShader->SetMat44("model", modelMat);

				//Set the point color in the light Shader
				lightShader->SetVec3("lightColor", pointLightsColor[i]);

				//Draw The lightCubeMesh
				lightCubeMesh->Draw(lightShader);
			}
		}
	}
}

void BloomGame::DrawPPScene()
{
	if (ppShader)
	{
		//Draw the quad from it's vertices, and use the color texture from our framebuffer as a texture sampled in the fragment shader
		glBindVertexArray(ppVAO);
		ppShader->Use();
		//Activate a texture unit and bind the color texture to it
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
		ppShader->SetInt("colorTexture", 0);
		ppShader->SetBool("UseHDR", bUseHDR);
		ppShader->SetFloat("exposure", exposureValue);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
		//Unbind the texture and the VAO
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}
}

void BloomGame::ProcessInput(GLFWwindow* window)
{
	Game::ProcessInput(window);

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		exposureValue += GetDeltaTime() * 0.5f;
	}
	else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		exposureValue -= GetDeltaTime() * 0.5f;
	}

	exposureValue = glm::clamp<float>(exposureValue, 0.0f, 3.0f);

	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		if (!bHDRTogglePressed)
		{
			bHDRTogglePressed = true;
			bUseHDR = !bUseHDR;
		}
	}
	else
	{
		bHDRTogglePressed = false;
	}
}