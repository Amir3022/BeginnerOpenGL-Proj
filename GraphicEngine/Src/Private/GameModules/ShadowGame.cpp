#include "GameModules/ShadowGame.h"
#include "Utilities/EngineUtilities.h"

ShadowGame::ShadowGame(int in_width, int in_height)
	: Game(in_width, in_height)
{
	vertexShaderPath = "Shaders/ShadowScene/VertexShader.glsl";
	fragmentShaderPath = "Shaders/ShadowScene/FragmentShader.glsl";

	shadowVertexShaderPath = "Shaders/ShadowScene/ShadowVertexShader.glsl";
	shadowFragmentShaderPath = "Shaders/ShadowScene/ShadowFragmentShader.glsl";

	ppFragmentShaderPath = "Shaders/ShadowScene/PPFragmentShader.glsl";
	ppVertexShaderPath = "Shaders/ShadowScene/PPVertexShader.glsl";

	dirLightDirection = glm::normalize(glm::vec3(1.0f, -1.0, -1.0f));
	dirLightColor = glm::vec3(0.98f, 0.98f, 0.98f) * 0.8f;

	shadowMapWidth = 1024;
	shadowMapHeight = 1024;
}

bool ShadowGame::Init()
{
	if (!Game::Init())
		return false;

	try
	{
		//Create Shader Program from Class
		shader = std::make_shared<Shader>(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

		//Create Simple Shadow Shader 
		shadowSimpleShader = std::make_shared<Shader>(shadowVertexShaderPath.c_str(), shadowFragmentShaderPath.c_str());

		//Create ppShader
		ppShader = std::make_shared<Shader>(ppVertexShaderPath.c_str(), ppFragmentShaderPath.c_str());

		//Change Camera transform
		camera->SetCameraLocation(camera->GetCameraLocation() + glm::vec3(0.0f, 2.0f, 2.0f));
		camera->SetCameraRotation(camera->GetCameraRotation() + glm::vec3(0.0f, -15.0f, 0.0f));

		//Create a vertices array   (Vertex Location, Vertex Normal, Texture Coordinate)
		std::vector<Vertex> vertices =
		{
			{glm::vec3(-0.5f, -0.5f, 0.5f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f)},		//0		//0
			{glm::vec3(-0.5f, -0.5f, 0.5f),  glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)},	//1
			{glm::vec3(-0.5f, -0.5f, 0.5f),  glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},		//2

			{glm::vec3(0.5f, -0.5f, 0.5f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f)},		//3		//1
			{glm::vec3(0.5f, -0.5f, 0.5f),  glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f)},		//4
			{glm::vec3(0.5f, -0.5f, 0.5f),  glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f)},		//5

			{glm::vec3(-0.5f,  0.5f, 0.5f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f)},		//6		//2
			{glm::vec3(-0.5f,  0.5f, 0.5f),  glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f)},	//7
			{glm::vec3(-0.5f,  0.5f, 0.5f),  glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},		//8

			{glm::vec3(0.5f,  0.5f, 0.5f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f)},		//9		//3
			{glm::vec3(0.5f,  0.5f, 0.5f),  glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)},		//10
			{glm::vec3(0.5f,  0.5f, 0.5f),  glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f)},		//11

			{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},		//12	//4
			{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f)},	//13
			{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},		//14

			{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f)},		//15	//5
			{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)},		//16
			{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f)},		//17

			{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f)},		//18	//6
			{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)},	//19
			{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},		//20

			{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f)},		//21	//7
			{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f)},		//22
			{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f)},		//23
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
			{glm::vec3(-0.5f, -0.5f, 0.0f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f)},	//0		//0
			{glm::vec3(-0.5f, -0.5f, -0.0001f),  glm::vec3(0.0f,  0.0f,  -1.0f), glm::vec2(0.0f, 0.0f)},//1	

			{glm::vec3(0.5f, -0.5f, 0.0f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f)},	//2		//1
			{glm::vec3(0.5f, -0.5f, -0.0001f),  glm::vec3(0.0f,  0.0f,  -1.0f), glm::vec2(1.0f, 0.0f)},	//3

			{glm::vec3(-0.5f,  0.5f, 0.0f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f)},	//4		//2
			{glm::vec3(-0.5f,  0.5f, -0.0001f),  glm::vec3(0.0f,  0.0f,  -1.0f), glm::vec2(0.0f, 1.0f)},//5

			{glm::vec3(0.5f,  0.5f, 0.0f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f)},	//6		//3
			{glm::vec3(0.5f,  0.5f, -0.0001f),  glm::vec3(0.0f,  0.0f,  -1.0f), glm::vec2(1.0f, 1.0f)},	//7
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

		//Create Plane
		//Create Plane Texture Object
		Texture texture_Plane_diffuse;
		texture_Plane_diffuse.texIndex = texture1;
		texture_Plane_diffuse.texType = ETextureType::diffuse;
		texture_Plane_diffuse.path = "Assets/Textures/WoodenFloor.png";
		std::vector<Texture> textures_Plane{ texture_Plane_diffuse };
		//Create Plane Mesh
		std::shared_ptr<Mesh> planeMesh = std::make_shared<Mesh>(planeVertices, planeIndices, textures_Plane);
		//Set Plane Transform
		planeMesh->SetTransform(glm::vec3(0.0f), glm::vec3(90.0, 0.0f, 0.0f), glm::vec3(20.0f));
		//Add the plane mesh to meshes vector
		meshes.push_back(planeMesh);
		bUseTiling.push_back(true);

		//Load textures for wooden box with metal frame
		unsigned int texture2 = EngineUtilities::LoadImageIntoTexture("Assets/Textures/container2.png", false, true);
		unsigned int texture3 = EngineUtilities::LoadImageIntoTexture("Assets/Textures/container2_specular.png", false, true);

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

		//Create First Cube Mesh
		std::shared_ptr<Mesh> cube_1 = std::make_shared<Mesh>(vertices, indices, textures_2);
		//Set First Cube Transform
		cube_1->SetTransform(glm::vec3(0.0f, 0.5f, 0.0f));
		//Add First cube to meshes Vector
		meshes.push_back(cube_1);
		bUseTiling.push_back(false);

		//Create Second Cube Mesh 
		std::shared_ptr<Mesh> cube_2 = std::make_shared<Mesh>(vertices, indices, textures_2);
		//Set Second Cube Transform
		cube_2->SetTransform(glm::vec3(4.0f, 0.5f, -4.0f));
		//Add second cube to meshes Vector
		meshes.push_back(cube_2);
		bUseTiling.push_back(false);


		//Create Third Cube Mesh 
		std::shared_ptr<Mesh> cube_3 = std::make_shared<Mesh>(vertices, indices, textures_2);
		//Set Second Cube Transform
		cube_3->SetTransform(glm::vec3(-1.0f, 2.0f, -4.0f), glm::vec3(15.0f, 0.0f, 0.0f));
		//Add second cube to meshes Vector
		meshes.push_back(cube_3);
		bUseTiling.push_back(false);

		//Create a framebuffer to hold depth map for shadow scene
		glGenFramebuffers(1, &shadowFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
		//Generate Texture to be used as the depth buffer for the shadow framebuffer
		glGenTextures(1, &shadowMap);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
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

void ShadowGame::Terminate()
{
	Game::Terminate();
}

void ShadowGame::UpdateGame(float deltaTime)
{
	Game::UpdateGame(deltaTime);

	//Update the meshes location if needed
}

void ShadowGame::DrawFrame()
{
	Game::DrawFrame();

	//Bind the shadow Map Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	//Change Viewport to match shadow map resolution
	glViewport(0, 0, shadowMapWidth, shadowMapHeight);
	//Enable Depth Testing, and clear color and depth buffers
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
}

void ShadowGame::DrawMainScene()
{
	//Check if the meshes is valid
	if (shader && meshes.size() > 0)
	{
		//Create the View matrix to see the plane model through the camera position
		glm::mat4 view = camera->GetLookAtMat(camera->GetCameraLocation() + camera->GetCameraForwardDir());
		//Create projection matrix to project the scene with depth through camera lens field of view and project view space to NDC
		glm::mat4 projection = glm::perspective(glm::radians(camera->GetCameraFOV()), (float)GetWidth() / (float)GetHeight(), 0.1f, 1000.0f);

		//Use the shader program, and set the matrices
		shader->Use();
		shader->SetMat44("view", view);
		shader->SetMat44("projection", projection);

		//Set the Light Space transformation matrics
		shader->SetMat44("lightSpaceTransformMat", lightSpaceTransformMat);

		//Set the viewer (Camera) world position
		shader->SetVec3("cameraPos", camera->GetCameraLocation());

		//Set the Shadow map sampler to be used
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		shader->SetInt("DirLightShadowMap", 8);

		//Rendering directional Light
		shader->SetVec3("dirLight.sourceDir", dirLightDirection);
		shader->SetVec3("dirLight.light.ambient", 0.05f * dirLightColor);
		shader->SetVec3("dirLight.light.diffuse", 0.75f * dirLightColor);
		shader->SetVec3("dirLight.light.specular", 1.0f * dirLightColor);

		for (int i = 0; i < meshes.size(); i++)
		{
			std::shared_ptr<Mesh> mesh = meshes[i];
			if (mesh)
			{
				//Create a model matrix to set plane location in world coordinates
				glm::mat4 modelMat = glm::identity < glm::mat4>();
				modelMat = glm::translate(modelMat, mesh->GetPosition());
				modelMat = glm::rotate(modelMat, glm::radians(mesh->GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
				modelMat = glm::rotate(modelMat, glm::radians(mesh->GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
				modelMat = glm::rotate(modelMat, glm::radians(mesh->GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
				modelMat = glm::scale(modelMat, mesh->GetScale());
				//Create normal model matrix to transform fragment normals
				glm::mat3 normalModelMatrix = glm::mat3(glm::transpose(glm::inverse(modelMat)));

				//Set changing Model and normal model matrices
				shader->SetMat44("model", modelMat);
				shader->SetMat33("normalModelMatrix", normalModelMatrix);

				//Set using tiling uniform
				shader->SetBool("bUseTiling", bUseTiling[i]);
				
				//Draw the Wooden floor mesh
				mesh->Draw(shader);
			}
		}
		//Unbind the shadow map from it's texture Category
		glActiveTexture(GL_TEXTURE15);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(0);
	}
}

void ShadowGame::RegisterShadowMap()
{
	//Check if the meshes is valid
	if (shader && meshes.size() > 0)
	{
		//Use a point in the direction of the directional light to be used as the virtual camera location
		glm::vec3 lightPosition = -dirLightDirection * 5.0f;
		//Create the View matrix to see the plane model through the Light position
		glm::mat4 view = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//Create orthographic projection matrix from the light position
		glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 15.0f);

		//Set the Directional Light LightSpaceTransformMat
		lightSpaceTransformMat = projection * view;

		//Use the shader program, and set the matrices
		shadowSimpleShader->Use();
		shadowSimpleShader->SetMat44("lightSpaceMat", lightSpaceTransformMat);


		for (int i = 0; i < meshes.size(); i++)
		{
			std::shared_ptr<Mesh> mesh = meshes[i];
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
				shadowSimpleShader->SetMat44("model", modelMat);

				//Draw the Wooden floor mesh
				mesh->Draw(shadowSimpleShader);
			}
		}
	}
}

void ShadowGame::DrawShadowMapRepresentation()
{
	if (ppShader)
	{
		//Draw the quad from it's vertices, and use the color texture from our framebuffer as a texture sampled in the fragment shader
		glBindVertexArray(ppVAO);
		ppShader->Use();
		//Activate a texture unit and bind the color texture to it
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		ppShader->SetInt("screenTexture", 0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
		//Unbind the texture and the VAO
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}
}