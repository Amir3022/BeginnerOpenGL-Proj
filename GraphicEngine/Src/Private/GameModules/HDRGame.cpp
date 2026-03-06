#include "GameModules/HDRGame.h"
#include "Utilities/EngineUtilities.h"

HDRGame::HDRGame(int in_width, int in_height)
	: Game(in_width, in_height)
{
	vertexShaderPath = "Shaders/HDRScene/VertexShader.glsl";
	fragmentShaderPath = "Shaders/HDRScene/FragmentShader.glsl";

	HDRVertexShaderPath = "Shaders/HDRScene/HDRVertexShader.glsl";
	HDRFragmentShaderPath = "Shaders/HDRScene/HDRFragmentShader.glsl";

	//Initialize Light Variables
	pointLightsPos =
	{
		glm::vec3(-2.9f, 0.0f, 0.0f),
		glm::vec3(2.9f, 0.0f, 0.0f),
		glm::vec3(0.0f, -2.9f, 0.0f),
		glm::vec3(0.0f, 0.0f, -49.9f)
	};

	pointLightsColor =
	{
		glm::vec3(0.1f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.1f, 0.0f),
		glm::vec3(0.0f, 0.f, 0.5f),
		glm::vec3(200.0f, 200.f, 200.0f),
	};
}

bool HDRGame::Init()
{
	if (!Game::Init())
		return false;

	try
	{
		//Create Shader Program from Class
		shader = std::make_shared<Shader>(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

		//Create HDR quad Shader
		HDRShader = std::make_shared<Shader>(HDRVertexShaderPath.c_str(), HDRFragmentShaderPath.c_str());

		//Change Camera transform
		camera->SetCameraLocation(camera->GetCameraLocation() + glm::vec3(0.0f, 0.0f, 2.0f));
		camera->SetCameraRotation(camera->GetCameraRotation() + glm::vec3(0.0f, 0.0f, 0.0f));

		//Declare Vertices for a 2D plane
		std::vector<Vertex> planeVertices =
		{
			{glm::vec3(-1.0f, -1.0f, 0.0f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f)},		//0		//0

			{glm::vec3(1.0f, -1.0f, 0.0f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f)},		//2		//1

			{glm::vec3(-1.0f,  1.0f, 0.0f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f)},		//4		//2

			{glm::vec3(1.0f,  1.0f, 0.0f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f)},		//6		//3
		};
		std::vector<unsigned int> planeIndices =
		{
			0, 1, 2,
			2, 1, 3,
		};

		//Load Wooden floor Image into texture
		unsigned int texture1 = EngineUtilities::LoadImageIntoTexture("Assets/Textures/WoodenFloor.png", false, true);

		//Create 6 Plane meshes to act as the closed room walls
		//Create Plane Texture Object
		Texture texture_Plane_diffuse;
		texture_Plane_diffuse.texIndex = texture1;
		texture_Plane_diffuse.texType = ETextureType::diffuse;
		texture_Plane_diffuse.path = "Assets/Textures/WoodenFloor.png";
		std::vector<Texture> textures_Plane{ texture_Plane_diffuse };
		//Declare wall pieces positions
		std::vector<glm::vec3> wallPositions
		{
			glm::vec3(3.0f, 0.0f, 0.0f),
			glm::vec3(-3.0f, 0.0f, 0.0f),

			glm::vec3(0.0f, 3.0f, 0.0f),
			glm::vec3(0.0f, -3.0f, 0.0f),

			glm::vec3(0.0f, 0.0f, 50.0f),
			glm::vec3(0.0f, 0.0f, -50.0f),
		};
		//Declare wall pieces rotations
		std::vector<glm::vec3> wallRotations
		{
			glm::vec3(0.0f, -90.0f, 0.0f),
			glm::vec3(0.0f, 90.0f, 0.0f),

			glm::vec3(90.0f, 0.0f, 0.0f),
			glm::vec3(-90.0f, 0.0f, 0.0f),

			glm::vec3(0.0f, 180.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
		};
		//Declare wall pieces scale
		std::vector<glm::vec3> wallScales
		{
			glm::vec3(50.0f, 3.0f, 1.0f),
			glm::vec3(50.0f, 3.0f, 1.0f),

			glm::vec3(3.0f, 50.0f, 1.0f),
			glm::vec3(3.0f, 50.0f, 1.0f),

			glm::vec3(3.0f, 3.0f, 1.0f),
			glm::vec3(3.0f, 3.0f, 1.0f),
		};
		//Create Plane Meshes
		for (int i = 0; i < 6; i++)
		{
			std::shared_ptr<Mesh> planeMesh = std::make_shared<Mesh>(planeVertices, planeIndices, textures_Plane);
			//Set Plane Transform
			planeMesh->SetTransform(wallPositions[i], wallRotations[i], wallScales[i]);
			//Add the plane mesh to meshes vector
			wallMeshes.push_back(planeMesh);
		}

		//Create a framebuffer to hold depth map for shadow scene
		glGenFramebuffers(1, &FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		//Generate Texture to be used as color with floating point internal format
		glGenTextures(1, &colorTexture);
		glBindTexture(GL_TEXTURE_2D, colorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, GetWidth(), GetHeight(), 0, GL_RGBA, GL_FLOAT, nullptr);

		//Set Bound textures parameteres
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		//Bind the texture to the color attachment of the framebuffer
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture, 0);

		//Create RenderBuffer to be used as depth and stencil attachment
		unsigned int RBO;
		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		//Reserve Renderbuffer storage for Depth and Stencil attachment
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, GetWidth(), GetHeight());
		//Bind the Renderbuffer to framebuffer as Depth and Stencil Attachment
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

		//Check if the Framebuffer is properly attached
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Failed to create shadow map framebuffer" << std::endl;
			return false;
		}
		//Unbind framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		//Create VAO for the Post Process quad having only a single quad taking the whole screen real state
		glGenVertexArrays(1, &HDRVAO);
		glBindVertexArray(HDRVAO);
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

void HDRGame::Terminate()
{
	Game::Terminate();
}

void HDRGame::UpdateGame(float deltaTime)
{
	Game::UpdateGame(deltaTime);
}

void HDRGame::DrawFrame()
{
	Game::DrawFrame();

	//Bind the HDR Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	//Enable Depth Testing, and clear color and depth buffers
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//Enable Gamme Correction using sRGB Colors in Framebuffers
	glEnable(GL_FRAMEBUFFER_SRGB);
	//Draw the main scene to the ColorTexture attached to the HDR Framebuffer
	DrawMainScene();

	//Bind the main framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//Enable Depth Testing, and clear color and depth buffers
	glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//Enable Gamme Correction using sRGB Colors in Framebuffers
	glEnable(GL_FRAMEBUFFER_SRGB);
	//Draw the Scene with shadows using the ShadowMap
	glEnable(GL_DEPTH_TEST);
	DrawHDRScene();
}

void HDRGame::DrawMainScene()
{
	//Check if the meshes is valid
	if (shader &&  wallMeshes.size() > 0)
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
		for (int i = 0; i < (int)pointLightsColor.size(); i++)
		{
			shader->SetVec3("pointLights[" + std::to_string(i) + "].sourcePos", pointLightsPos[i]);
			shader->SetVec3("pointLights[" + std::to_string(i) + "].light.ambient", 0.2f * glm::normalize(pointLightsColor[i]));
			shader->SetVec3("pointLights[" + std::to_string(i) + "].light.diffuse", 0.75f * pointLightsColor[i]);
			shader->SetVec3("pointLights[" + std::to_string(i) + "].light.specular", 1.0f * pointLightsColor[i]);
			shader->SetFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);	//Attenuation constants for a light source that covers and outer radius on 50 units
			shader->SetFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
			shader->SetFloat("pointLights[" + std::to_string(i) + "].quad", 0.032f);
		}

		//Rendering Wall Planes
		for (int i = 0; i < wallMeshes.size(); i++)
		{
			std::shared_ptr<Mesh> wallMesh = wallMeshes[i];
			if (wallMesh)
			{
				//Create a model matrix to set plane location in world coordinates
				glm::mat4 modelMat = glm::identity < glm::mat4>();
				modelMat = glm::translate(modelMat, wallMesh->GetPosition());
				modelMat = glm::rotate(modelMat, glm::radians(wallMesh->GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
				modelMat = glm::rotate(modelMat, glm::radians(wallMesh->GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
				modelMat = glm::rotate(modelMat, glm::radians(wallMesh->GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
				modelMat = glm::scale(modelMat, wallMesh->GetScale());
				//Create normal model matrix to transform fragment normals
				glm::mat3 normalModelMatrix = glm::mat3(glm::transpose(glm::inverse(modelMat)));

				//Set changing Model and normal model matrices
				shader->SetMat44("model", modelMat);
				shader->SetMat33("normalModelMatrix", normalModelMatrix);

				//Enable using tiling uniform for all wall meshes
				shader->SetBool("bUseTiling", false);

				//Draw the Wooden floor mesh
				wallMesh->Draw(shader);
			}
		}
	}
}

void HDRGame::DrawHDRScene()
{
	if (HDRShader)
	{
		//Draw the quad from it's vertices, and use the color texture from our framebuffer as a texture sampled in the fragment shader
		glBindVertexArray(HDRVAO);
		HDRShader->Use();
		//Activate a texture unit and bind the color texture to it
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorTexture);
		HDRShader->SetInt("colorTexture", 0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
		//Unbind the texture and the VAO
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}
}