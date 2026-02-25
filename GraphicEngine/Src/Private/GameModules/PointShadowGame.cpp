#include "GameModules/PointShadowGame.h"
#include "Utilities/EngineUtilities.h"

PointShadowGame::PointShadowGame(int in_width, int in_height)
	: Game(in_width, in_height)
{
	vertexShaderPath = "Shaders/PointShadowScene/VertexShader.glsl";
	fragmentShaderPath = "Shaders/PointShadowScene/FragmentShader.glsl";

	shadowVertexShaderPath = "Shaders/PointShadowScene/ShadowVertexShader.glsl";
	shadowGeometryShaderPath = "Shaders/PointShadowScene/ShadowGeometryShader.glsl";
	shadowFragmentShaderPath = "Shaders/PointShadowScene/ShadowFragmentShader.glsl";

	lightFragmentShaderPath = "Shaders/PointShadowScene/LightFragmentShader.glsl";
	lightVertexShaderPath = "Shaders/PointShadowScene/LightVertexShader.glsl";

	cubemapFragmentShaderPath = "Shaders/PointShadowScene/CubemapFragmentShader.glsl";
	cubemapVertexShaderPath = "Shaders/PointShadowScene/CubemapVertexShader.glsl";

	//Initialize Light Variables
	pointLightPos = glm::vec3(0.0f, 0.0f, 5.0f);
	pointLightColor = glm::vec3(1.0f);

	//Init Shadow Map resolution
	shadowMapWidth = 1024;
	shadowMapHeight = 1024;
	farPlaneDistance = 50.0f;
	bRenderModel = true;
}

bool PointShadowGame::Init()
{
	if (!Game::Init())
		return false;

	try
	{
		//Create Shader Program from Class
		shader = std::make_shared<Shader>(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

		//Create Simple Shadow Shader 
		pointShadowShader = std::make_shared<Shader>(shadowVertexShaderPath.c_str(), shadowFragmentShaderPath.c_str());
		//Bind the Shadow Geometry Shader
		pointShadowShader->AttachGeometryShader(shadowGeometryShaderPath.c_str());

		//Create Light Shader to render point lights
		lightShader = std::make_shared<Shader>(lightVertexShaderPath.c_str(), lightFragmentShaderPath.c_str());

		//Create CubeMap Shader
		cubemapShader = std::make_unique<Shader>(cubemapVertexShaderPath.c_str(), cubemapFragmentShaderPath.c_str());

		//Change Camera transform
		camera->SetCameraLocation(camera->GetCameraLocation() + glm::vec3(0.0f, 2.0f, 2.0f));
		camera->SetCameraRotation(camera->GetCameraRotation() + glm::vec3(0.0f, -15.0f, 0.0f));

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
			glm::vec3(10.0f, 0.0f, 0.0f),
			glm::vec3(-10.0f, 0.0f, 0.0f),

			glm::vec3(0.0f, 10.0f, 0.0f),
			glm::vec3(0.0f, -10.0f, 0.0f),

			glm::vec3(0.0f, 0.0f, 10.0f),
			glm::vec3(0.0f, 0.0f, -10.0f),
		};
		//Declare wall pieces rotations
		std::vector<glm::vec3> wallRotations
		{
			glm::vec3(0.0f, 90.0f, 0.0f),
			glm::vec3(0.0f, 90.0f, 0.0f),

			glm::vec3(90.0f, 0.0f, 0.0f),
			glm::vec3(90.0f, 0.0f, 0.0f),

			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
		};
		//Create Plane Meshes
		for (int i = 0; i < 6; i++)
		{
			std::shared_ptr<Mesh> planeMesh = std::make_shared<Mesh>(planeVertices, planeIndices, textures_Plane);
			//Set Plane Transform
			planeMesh->SetTransform(wallPositions[i], wallRotations[i], glm::vec3(10.0f));
			//Add the plane mesh to meshes vector
			wallMeshes.push_back(planeMesh);
		}

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
			glm::vec3(-6.80f,   5.10f,  -7.40f),
			glm::vec3(7.20f,  -4.90f,   6.30f),
			glm::vec3(-3.10f,   4.80f,   2.50f),
			glm::vec3(4.60f,  -3.70f,  -5.90f),
			glm::vec3(5.40f,   3.20f,  -1.80f),
			glm::vec3(7.16f,  -6.61f,  -1.48f),
			glm::vec3(6.28f,   6.42f,  -7.39f),
			glm::vec3(0.58f,  -2.68f,   5.63f),
			glm::vec3(-2.45f,  -2.60f,  -2.66f),
			glm::vec3(-4.08f,  -5.97f,  -1.02f)
		};

		//Declare cubes rotations
		std::vector<glm::vec3> cubeRotations
		{
			glm::vec3(14.5f,  137.0f,  -78.0f),
			glm::vec3(-62.0f,  -19.5f,   91.0f),
			glm::vec3(38.0f, -164.0f,   22.5f),
			glm::vec3(-77.5f,   84.0f, -143.0f),
			glm::vec3(51.0f,  179.0f,  -34.5f),
			glm::vec3(14.5f,  137.0f,  -78.0f),
			glm::vec3(-62.0f,  -19.5f,   91.0f),
			glm::vec3(38.0f, -164.0f,   22.5f),
			glm::vec3(-77.5f,   84.0f, -143.0f),
			glm::vec3(51.0f,  179.0f,  -34.5f)
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

		//Create the Model instance
		model = std::make_shared<Model>("Assets/Meshes/backpack/backpack.obj", true);

		//Create a cube mesh to represent the point light position
		lightCubeMesh = std::make_shared<Mesh>(vertices, indices, textures_2);
		//Set lightCubeMesh transform
		lightCubeMesh->SetTransform(pointLightPos, glm::vec3(0.f), glm::vec3(0.25f, 0.25f, 0.25f));	//Scale the cube down so it doesn't take much screen realstate

		//Create a framebuffer to hold depth map for shadow scene
		glGenFramebuffers(1, &shadowFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
		//Generate Texture to be used as the depth buffer for the shadow framebuffer
		glGenTextures(1, &shadowMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);	//The generated texture will be a cubemap to capture depth from the whole surrounding scene
		//Generate 6 images for each side of the shadowMap
		for (int i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		}
		//Set Bound textures parameteres
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//Bind the cubemap to the framebuffer to be used as the depth buffer
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMap, 0);
		//Set the Color Read and Draw buffers as empty
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		//Check if the Framebuffer is properly attached
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Failed to create shadow map framebuffer" << std::endl;
			return false;
		}
		//Unbind framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		//Create VAO for the Cubemap
		glGenVertexArrays(1, &cubemapVAO);
		glBindVertexArray(cubemapVAO);

		//Create a unit cube at Origin, consisting only of vertex locations
		float cubemapVertices[] = {
			-1.0f, -1.0f, 1.0f,		//0
			1.0f, -1.0f, 1.0f,		//1
			-1.0f, 1.0f, 1.0f,		//2
			1.0f, 1.0f, 1.0f,		//3

			-1.0f, -1.0f, -1.0f,	//4
			1.0f, -1.0f, -1.0f,		//5
			-1.0f, 1.0f, -1.0f,		//6
			1.0f, 1.0f, -1.0f,		//7
		};

		unsigned int cubemapIndices[] =
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
		unsigned int cubemapVBO, cubemapEBO;
		glGenBuffers(1, &cubemapVBO);
		glBindBuffer(GL_ARRAY_BUFFER, cubemapVBO);
		glGenBuffers(1, &cubemapEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubemapEBO);
		//Assign data to the buffers
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapVertices), cubemapVertices, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubemapIndices), cubemapIndices, GL_STATIC_DRAW);
		//Declate Vertex attributes
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		//Unbind the cubemapVAO
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

void PointShadowGame::Terminate()
{
	Game::Terminate();
}

void PointShadowGame::UpdateGame(float deltaTime)
{
	Game::UpdateGame(deltaTime);

	//Update the point light position
	if (lightCubeMesh)
	{
		pointLightPos = glm::vec3(0.0f, glm::sin(glfwGetTime() * 0.25f) * 7.5f, 5.0f);
		lightCubeMesh->SetTransform(pointLightPos, glm::vec3(0.0f), lightCubeMesh->GetScale());
	}
}

void PointShadowGame::DrawFrame()
{
	Game::DrawFrame();

	//Bind the shadow Map Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	//Change Viewport to match shadow map resolution
	glViewport(0, 0, shadowMapWidth, shadowMapHeight);
	//Enable Depth Testing, and clear color and depth buffers
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//Draw the shadow map by drawing the depth value of the main scene from the directional light point of view
	RegisterShadowMap();

	//Bind the main framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//Change Viewport to proper resolution
	glViewport(0, 0, GetWidth(), GetHeight());
	//Enable Depth Testing, and clear color and depth buffers
	glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Enable Gamme Correction using sRGB Colors in Framebuffers
	glEnable(GL_FRAMEBUFFER_SRGB);
	//Draw the Scene with shadows using the ShadowMap
	glEnable(GL_DEPTH_TEST);
	DrawMainScene();

	//Draw the Cubemap
	//glDepthFunc(GL_LEQUAL);	//Change the Depth Test function to less than or equal, to make sure the cube map is rendered behind all other meshes in the scene
	//DrawPointShadowCubemap();
}

void PointShadowGame::DrawMainScene()
{
	//Check if the meshes is valid
	if (shader && (cubeMeshes.size() > 0 || wallMeshes.size() > 0))
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

		//Set the Shadow map sampler to be used
		glActiveTexture(GL_TEXTURE15);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);
		shader->SetInt("PointLightShadowMap", 15);
		//Set the Far Plane value used in shadow registering
		shader->SetFloat("far_Plane", farPlaneDistance);

		//Rendering point Light
		shader->SetVec3("pointLights[" + std::to_string(0) + "].sourcePos", pointLightPos);
		shader->SetVec3("pointLights[" + std::to_string(0) + "].light.ambient", 0.2f * glm::normalize(pointLightColor));
		shader->SetVec3("pointLights[" + std::to_string(0) + "].light.diffuse", 0.75f * pointLightColor);
		shader->SetVec3("pointLights[" + std::to_string(0) + "].light.specular", 1.0f * pointLightColor);
		shader->SetFloat("pointLights[" + std::to_string(0) + "].constant", 1.0f);	//Attenuation constants for a light source that covers and outer radius on 50 units
		shader->SetFloat("pointLights[" + std::to_string(0) + "].linear", 0.09f);
		shader->SetFloat("pointLights[" + std::to_string(0) + "].quad", 0.032f);

		//Switch between rendering the Cube Meshes and the backpack model
		if (bRenderModel)
		{
			if (model)
			{
				//Create a model matrix to set model location in world coordinates
				glm::mat4 modelMat = glm::identity < glm::mat4>();
				modelMat = glm::translate(modelMat, model->GetPosition());
				modelMat = glm::rotate(modelMat, glm::radians(model->GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
				modelMat = glm::rotate(modelMat, glm::radians(model->GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
				modelMat = glm::rotate(modelMat, glm::radians(model->GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
				modelMat = glm::scale(modelMat, model->GetScale());
				//Create normal model matrix to transform fragment normals
				glm::mat3 normalModelMatrix = glm::mat3(glm::transpose(glm::inverse(modelMat)));

				//Set changing Model and normal model matrices
				shader->SetMat44("model", modelMat);
				shader->SetMat33("normalModelMatrix", normalModelMatrix);

				//Disable using tiling uniform for the model
				shader->SetBool("bUseTiling", false);

				//Draw the model mesh
				model->Draw(shader);
			}
		}
		else
		{
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
				shader->SetBool("bUseTiling", true);

				//Draw the Wooden floor mesh
				wallMesh->Draw(shader);
			}
		}

		//Rendering point light cube
		//Enable the Light Shader program
		if (lightShader && lightCubeMesh)
		{
			lightShader->Use();

			//Create a model matrix to set plane location in world coordinates
			glm::mat4 modelMat = glm::identity < glm::mat4>();
			modelMat = glm::translate(modelMat, lightCubeMesh->GetPosition());
			modelMat = glm::rotate(modelMat, glm::radians(lightCubeMesh->GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
			modelMat = glm::rotate(modelMat, glm::radians(lightCubeMesh->GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
			modelMat = glm::rotate(modelMat, glm::radians(lightCubeMesh->GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
			modelMat = glm::scale(modelMat, lightCubeMesh->GetScale());

			//Set the matrices uniforms in the Light Shader
			lightShader->SetMat44("model", modelMat);
			lightShader->SetMat44("view", view);
			lightShader->SetMat44("projection", projection);

			//Set the point color in the light Shader
			lightShader->SetVec3("lightColor", pointLightColor * 0.75f);

			//Draw The lightCubeMesh
			lightCubeMesh->Draw(lightShader);
		}
		//Unbind the shadow map from it's texture Category
		glActiveTexture(GL_TEXTURE15);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(0);
	}
}

void PointShadowGame::RegisterShadowMap()
{
	//Check if the meshes is valid
	if (shader && (cubeMeshes.size() > 0 || wallMeshes.size() > 0))
	{
		//Use the shader program, and set the matrices
		pointShadowShader->Use();
		//Use point light position as the virtual camera position
		glm::vec3 lightPosition = pointLightPos;
		//Create perspective projection matrix from the point light position
		glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)shadowMapWidth / (float)shadowMapHeight, 0.1f, farPlaneDistance); //point of view is set to 90.0f to cover the whole face
		//Create 6 View Matrices for each face of the cubemap using the point light position, direction from point light to the current face
		std::vector<glm::vec3> viewDirs
		{
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(-1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(0.0f, 0.0f, -1.0f),
		};
		std::vector<glm::vec3> upVectorDirs
		{
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(0.0f, 0.0f, -1.0f),
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(0.0f, -1.0f, 0.0f),
		};
		//Empty the light Space Transform Matrices vector
		lightSpaceTransformMats.clear();
		for (int i = 0; i < 6; i++)
		{
			glm::mat4 view = glm::lookAt(lightPosition, lightPosition + viewDirs[i], upVectorDirs[i]);
			lightSpaceTransformMats.push_back(projection * view);	//Add the light space transformation matrix for the indexed face
			pointShadowShader->SetMat44("lightSpaceMat[" + std::to_string(i) + "]", lightSpaceTransformMats[i]);	//Set the light Space Transforms for each index
		}
		
		//Set the Far Plane distance used
		pointShadowShader->SetFloat("far_Plane", farPlaneDistance);

		//Set the point light Position
		pointShadowShader->SetVec3("pointLightPos", pointLightPos);

		//Switch between rendering cubes and the model
		if (bRenderModel)
		{
			if (model)
			{
				//Create a model matrix to set plane location in world coordinates
				glm::mat4 modelMat = glm::identity < glm::mat4>();
				modelMat = glm::translate(modelMat, model->GetPosition());
				modelMat = glm::rotate(modelMat, glm::radians(model->GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
				modelMat = glm::rotate(modelMat, glm::radians(model->GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
				modelMat = glm::rotate(modelMat, glm::radians(model->GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
				modelMat = glm::scale(modelMat, model->GetScale());

				//Set changing Model and normal model matrices
				pointShadowShader->SetMat44("model", modelMat);

				//Draw the Wooden floor mesh
				model->Draw(pointShadowShader);
			}
		}
		else
		{
			//Register depth value for each mesh in cube meshes arrays (No need to get shadows from wall meshes for this demo)
			for (int i = 0; i < cubeMeshes.size(); i++)
			{
				std::shared_ptr<Mesh> mesh = cubeMeshes[i];
				if (mesh)
				{
					//Create a model matrix to set plane location in world coordinates
					glm::mat4 modelMat = glm::identity < glm::mat4>();
					modelMat = glm::translate(modelMat, mesh->GetPosition());
					modelMat = glm::rotate(modelMat, glm::radians(mesh->GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
					modelMat = glm::rotate(modelMat, glm::radians(mesh->GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
					modelMat = glm::rotate(modelMat, glm::radians(mesh->GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
					modelMat = glm::scale(modelMat, mesh->GetScale());

					//Set changing Model and normal model matrices
					pointShadowShader->SetMat44("model", modelMat);

					//Draw the Wooden floor mesh
					mesh->Draw(pointShadowShader);
				}
			}
		}
	}
}

void PointShadowGame::DrawPointShadowCubemap()
{
	//Check if the cubemap Shader is valid
	if (cubemapShader)
	{
		//Bind the cubemap VAO
		glBindVertexArray(cubemapVAO);

		//Create the view matrix using camera lookAt target point, need to remove the translation data from view matrix so take the 3x3 upper left matrix
		glm::mat4 view = glm::mat3(camera->GetLookAtMat(camera->GetCameraLocation() + camera->GetCameraForwardDir()));

		//Create the projection matrix to project the view space to NDC
		glm::mat4 projection = glm::perspective(glm::radians(camera->GetCameraFOV()), (float)GetWidth() / (float)GetHeight(), 0.1f, 100.0f);

		//Use the Shader Program to draw Vertices using the defined vertex and fragment shaders, view, projection matrices
		cubemapShader->Use();
		cubemapShader->SetMat44("view", view);
		cubemapShader->SetMat44("projection", projection);

		//Activate the first texture unit, and bind the cubemap texture to it
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);
		//Use the texture unit index as the active texture sampler in fragment shader
		cubemapShader->SetInt("cubemapTexture", 0);

		//Draw the cubemap
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);

		//Unbind the cubemapVAO and cubemap texture
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glActiveTexture(0);
	}
}