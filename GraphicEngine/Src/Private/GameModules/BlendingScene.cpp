#include "GameModules/BlendingScene.h"
#include "Utilities/EngineUtilities.h"
#include <algorithm>

BlendingGame::BlendingGame(int in_width, int in_height)
	: Game(in_width, in_height)
{
	fragmentShaderPath = "Shaders/BlendingScene/FragmentShader.glsl";
	vertexShaderPath = "Shaders/BlendingScene/VertexShader.glsl";

	lightFragmentShaderPath = "Shaders/BlendingScene/LightFragmentShader.glsl";
	lightVertexShaderPath = "Shaders/BlendingScene/LightVertexShader.glsl";

	dirLightColor = glm::vec3(0.98f, 0.98f, 0.98f) * 2.0f;
	dirLightOrient = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));

	bSceneLit = true;
	bSwitchLightWasPressed = false;
}

bool BlendingGame::Init()
{
	if (!Game::Init())
		return false;

	try
	{
		//Create Shader Program from Class
		shader = std::make_shared<Shader>(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

		//Create Light Shader to render the Light Cube
		lightShader = std::make_unique<Shader>(lightVertexShaderPath.c_str(), lightFragmentShaderPath.c_str());

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

			3, 1, 5,
			5, 7, 3
		};

		//Load two Images into 2 different textures
		unsigned int texture1 = EngineUtilities::LoadImageIntoTexture("Assets/Textures/container.jpg");
		unsigned int texture2 = EngineUtilities::LoadImageIntoTexture("Assets/Textures/container2.png");
		unsigned int texture3 = EngineUtilities::LoadImageIntoTexture("Assets/Textures/container2_specular.png");
		unsigned int texture4 = EngineUtilities::LoadImageIntoTexture("Assets/Textures/Ground.jpg");

		//Create 2 cubes meshes
		//Create First Texture object
		Texture texture_1;
		texture_1.texIndex = texture1;
		texture_1.texType = ETextureType::diffuse;
		texture_1.path = "Assets/Textures/container.jpg";
		std::vector<Texture> textures_1{ texture_1 };
		//Create First Cube Mesh
		std::shared_ptr<Mesh> cube_1 = std::make_shared<Mesh>(vertices, indices, textures_1);
		//Set First Cube Transform
		cube_1->SetTransform(glm::vec3(0.0f, 0.5f, 0.0f));
		//Add first cube to Meshes vector
		meshes.push_back(cube_1);

		//Create Second Texture Object
		Texture texture_2_diffuse;
		texture_2_diffuse.texIndex = texture2;
		texture_2_diffuse.texType = ETextureType::diffuse;
		texture_2_diffuse.path = "Assets/Textures/container2.png";
		Texture texture_2_specular;
		texture_2_specular.texIndex = texture3;
		texture_2_specular.texType = ETextureType::specular;
		texture_2_specular.path = "Assets/Textures/container2_specular.png";
		std::vector<Texture>textures_2{ texture_2_diffuse, texture_2_specular };
		//Create Second Cube Mesh
		std::shared_ptr<Mesh> cube_2 = std::make_shared<Mesh>(vertices, indices, textures_2);
		//Set Second Cube Transform
		cube_2->SetTransform(glm::vec3(2.0f, 0.5f, -2.0f));
		//Add second cube to meshes Vector
		meshes.push_back(cube_2);

		//Create Plane
		//Create Plane Texture Object
		Texture texture_Plane_diffuse;
		texture_Plane_diffuse.texIndex = texture4;
		texture_Plane_diffuse.texType = ETextureType::diffuse;
		texture_Plane_diffuse.path = "Assets/Textures/Ground.jpg";
		std::vector<Texture> textures_Plane{ texture_Plane_diffuse };
		//Create Plane Mesh
		std::shared_ptr<Mesh> planeMesh = std::make_shared<Mesh>(planeVertices, planeIndices, textures_Plane);
		//Set Plane Transform
		planeMesh->SetTransform(glm::vec3(0.0f), glm::vec3(90.0, 0.0f, 0.0f), glm::vec3(6.0f));
		//Add Plane to meshes vector
		meshes.push_back(planeMesh);

		//Create Vegetation planes
		//Positions for vegetation plane
		std::vector<glm::vec3> vegetationPositions;
		vegetationPositions.push_back(glm::vec3(1.5f, 0.5f, 0.48f));
		vegetationPositions.push_back(glm::vec3(-1.5f, 0.5f, -0.51f));
		vegetationPositions.push_back(glm::vec3(-1.0f, 0.5f, 2.5f));
		vegetationPositions.push_back(glm::vec3(-0.75f, 0.5f, 1.9f));
		vegetationPositions.push_back(glm::vec3(-0.3f, 0.5f, -0.7f));
		vegetationPositions.push_back(glm::vec3(0.3f, 0.5f, 2.3f));
		vegetationPositions.push_back(glm::vec3(-0.5f, 0.5f, 0.6f));
		//Loading the grass texture
		unsigned int grass_texture = EngineUtilities::LoadImageIntoTexture("Assets/Textures/grass.png", true);
		//Creating grass texture instance
		Texture grass_texture_diffuse;
		grass_texture_diffuse.texIndex = grass_texture;
		grass_texture_diffuse.texType = ETextureType::diffuse;
		grass_texture_diffuse.path = "Assets/Textures/grass.png";
		std::vector<Texture> grass_textures{ grass_texture_diffuse };
		for (glm::vec3 vegetationPosition : vegetationPositions)
		{
			//Create Plane Mesh
			std::shared_ptr<Mesh> grassPlaneMesh = std::make_shared<Mesh>(planeVertices, planeIndices, grass_textures);
			//Set Grass plane transform
			grassPlaneMesh->SetTransform(vegetationPosition);
			//Add Plane to meshes vector
			vegetationPlanes.push_back(grassPlaneMesh);
		}
		
		//Create Window planes
		//Positions for vegetation plane
		std::vector<glm::vec3> windowPositions;
		windowPositions.push_back(glm::vec3(-1.5f, 0.5f, -0.25f));
		windowPositions.push_back(glm::vec3(1.5f, 0.5f, 0.51f));
		windowPositions.push_back(glm::vec3(0.3f, 0.5f, 1.2f));
		windowPositions.push_back(glm::vec3(-0.3f, 0.5f, -2.3f));
		windowPositions.push_back(glm::vec3(0.5f, 0.5f, -0.6f));
		//Loading the grass texture
		unsigned int window_texture = EngineUtilities::LoadImageIntoTexture("Assets/Textures/blending_transparent_window.png", true);
		//Creating grass texture instance
		Texture window_texture_diffuse;
		window_texture_diffuse.texIndex = window_texture;
		window_texture_diffuse.texType = ETextureType::diffuse;
		window_texture_diffuse.path = "Assets/Textures/grass.png";
		std::vector<Texture> window_textures{ window_texture_diffuse };
		for (glm::vec3 windowPosition : windowPositions)
		{
			//Create Plane Mesh
			std::shared_ptr<Mesh> windowPlaneMesh = std::make_shared<Mesh>(planeVertices, planeIndices, window_textures);
			//Set Grass plane transform
			windowPlaneMesh->SetTransform(windowPosition);
			//Add Plane to meshes vector
			alphaPlanes.push_back(windowPlaneMesh);
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

void BlendingGame::Terminate()
{
	Game::Terminate();
}

void BlendingGame::UpdateGame(float deltaTime)
{
	Game::UpdateGame(deltaTime);

	//Update the Transparent alpha planes sorting to draw the furthest plane from camera position first
	std::sort(alphaPlanes.begin(), alphaPlanes.end(),
		[this](std::shared_ptr<Mesh> first, std::shared_ptr<Mesh> second)
		{
			float distanceToFirst = glm::distance(camera->GetCameraLocation(), first->GetPosition());
			float distanceToSecond = glm::distance(camera->GetCameraLocation(), second->GetPosition());
			return distanceToFirst > distanceToSecond;
		});
}

void BlendingGame::DrawFrame()
{
	Game::DrawFrame();

	glClearColor(0.03f, 0.03f, 0.03f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//Enable Blending
	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	//Enable Face Culling
	glEnable(GL_CULL_FACE);

	//Draw first all Opaque meshes
	if ((int)meshes.size() > 0)
	{
		for (int i = 0; i < (int)meshes.size(); i++)
		{
			//Get reference to the current mesh
			std::shared_ptr<Mesh> mesh = meshes[i];

			// Create Transform matrix to transform the drawn image
			//Create the model matrix to transform the object in world space
			glm::mat4 modelMat = glm::identity<glm::mat4>();
			modelMat = glm::translate(modelMat, mesh->GetPosition());
			modelMat = glm::rotate(modelMat, glm::radians(mesh->GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
			modelMat = glm::rotate(modelMat, glm::radians(mesh->GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
			modelMat = glm::rotate(modelMat, glm::radians(mesh->GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
			modelMat = glm::scale(modelMat, mesh->GetScale());
			//Create the Normal Model Matrix to convert normal from local space to World coordinates while respecting scale
			glm::mat3 normalModelMatrix = glm::mat3(glm::transpose(glm::inverse(modelMat)));

			//Create the view matrix using camera lookAt target point
			glm::mat4 view = camera->GetLookAtMat(camera->GetCameraLocation() + camera->GetCameraForwardDir());

			//Create the projection matrix to project the view space to NDC
			glm::mat4 projection = glm::perspective(glm::radians(camera->GetCameraFOV()), (float)GetWidth() / (float)GetHeight(), 0.1f, 100.0f);

			//Use the Shader Program to draw Vertices using the defined vertex and fragment shaders, and apply model, view, projection matrices
			shader->Use();
			shader->SetMat44("model", modelMat);
			shader->SetMat44("view", view);
			shader->SetMat44("projection", projection);
			shader->SetMat33("normalModelMatrix", normalModelMatrix);

			//Set the viewer (Camera) world position
			shader->SetVec3("cameraPos", camera->GetCameraLocation());

			//Rendering directional Light
			shader->SetVec3("dirLight.sourceDir", dirLightOrient);
			shader->SetVec3("dirLight.light.ambient", 0.1f * dirLightColor);
			shader->SetVec3("dirLight.light.diffuse", 0.75f * dirLightColor);
			shader->SetVec3("dirLight.light.specular", 1.0f * dirLightColor);

			//Set the Lit Mode variable
			shader->SetBool("bLit", bSceneLit);
			
			mesh->Draw(shader);
		}
	}
	//Draw the vegetation planes
	if ((int)vegetationPlanes.size() > 0)
	{
		for (int i = 0; i < (int)vegetationPlanes.size(); i++)
		{
			//Get reference to the current mesh
			std::shared_ptr<Mesh> vegetationPlane = vegetationPlanes[i];

			// Create Transform matrix to transform the drawn image
			//Create the model matrix to transform the object in world space
			glm::mat4 modelMat = glm::identity<glm::mat4>();
			modelMat = glm::translate(modelMat, vegetationPlane->GetPosition());
			modelMat = glm::rotate(modelMat, glm::radians(vegetationPlane->GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
			modelMat = glm::rotate(modelMat, glm::radians(vegetationPlane->GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
			modelMat = glm::rotate(modelMat, glm::radians(vegetationPlane->GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
			modelMat = glm::scale(modelMat, vegetationPlane->GetScale());
			//Create the Normal Model Matrix to convert normal from local space to World coordinates while respecting scale
			glm::mat3 normalModelMatrix = glm::mat3(glm::transpose(glm::inverse(modelMat)));

			//Create the view matrix using camera lookAt target point
			glm::mat4 view = camera->GetLookAtMat(camera->GetCameraLocation() + camera->GetCameraForwardDir());

			//Create the projection matrix to project the view space to NDC
			glm::mat4 projection = glm::perspective(glm::radians(camera->GetCameraFOV()), (float)GetWidth() / (float)GetHeight(), 0.1f, 100.0f);

			//Use the Shader Program to draw Vertices using the defined vertex and fragment shaders, and apply model, view, projection matrices
			shader->Use();
			shader->SetMat44("model", modelMat);
			shader->SetMat44("view", view);
			shader->SetMat44("projection", projection);
			shader->SetMat33("normalModelMatrix", normalModelMatrix);

			//Set the viewer (Camera) world position
			shader->SetVec3("cameraPos", camera->GetCameraLocation());

			//Rendering directional Light
			shader->SetVec3("dirLight.sourceDir", dirLightOrient);
			shader->SetVec3("dirLight.light.ambient", 0.1f * dirLightColor);
			shader->SetVec3("dirLight.light.diffuse", 0.75f * dirLightColor);
			shader->SetVec3("dirLight.light.specular", 1.0f * dirLightColor);

			//Set the Lit Mode variable
			shader->SetBool("bLit", bSceneLit);

			vegetationPlane->Draw(shader);
		}
	}

	//Draw all windows with alpha afterwards
	if ((int)alphaPlanes.size() > 0)
	{
		for (int i = 0; i < (int)alphaPlanes.size(); i++)
		{
			//Get reference to the current mesh
			std::shared_ptr<Mesh> alphaPlane = alphaPlanes[i];

			// Create Transform matrix to transform the drawn image
			//Create the model matrix to transform the object in world space
			glm::mat4 modelMat = glm::identity<glm::mat4>();
			modelMat = glm::translate(modelMat, alphaPlane->GetPosition());
			modelMat = glm::rotate(modelMat, glm::radians(alphaPlane->GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
			modelMat = glm::rotate(modelMat, glm::radians(alphaPlane->GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
			modelMat = glm::rotate(modelMat, glm::radians(alphaPlane->GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
			modelMat = glm::scale(modelMat, alphaPlane->GetScale());
			//Create the Normal Model Matrix to convert normal from local space to World coordinates while respecting scale
			glm::mat3 normalModelMatrix = glm::mat3(glm::transpose(glm::inverse(modelMat)));

			//Create the view matrix using camera lookAt target point
			glm::mat4 view = camera->GetLookAtMat(camera->GetCameraLocation() + camera->GetCameraForwardDir());

			//Create the projection matrix to project the view space to NDC
			glm::mat4 projection = glm::perspective(glm::radians(camera->GetCameraFOV()), (float)GetWidth() / (float)GetHeight(), 0.1f, 100.0f);

			//Use the Shader Program to draw Vertices using the defined vertex and fragment shaders, and apply model, view, projection matrices
			shader->Use();
			shader->SetMat44("model", modelMat);
			shader->SetMat44("view", view);
			shader->SetMat44("projection", projection);
			shader->SetMat33("normalModelMatrix", normalModelMatrix);

			//Set the viewer (Camera) world position
			shader->SetVec3("cameraPos", camera->GetCameraLocation());

			//Rendering directional Light
			shader->SetVec3("dirLight.sourceDir", dirLightOrient);
			shader->SetVec3("dirLight.light.ambient", 0.1f * dirLightColor);
			shader->SetVec3("dirLight.light.diffuse", 0.75f * dirLightColor);
			shader->SetVec3("dirLight.light.specular", 1.0f * dirLightColor);

			//Set the Lit Mode variable
			shader->SetBool("bLit", false);

			alphaPlane->Draw(shader);
		}
	}
}

void BlendingGame::ProcessInput(GLFWwindow* window)
{
	Game::ProcessInput(window);
	//Cycle between Lit and Unlit Modes in Rendering Model when pressing P
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		if (!bSwitchLightWasPressed)
		{
			bSwitchLightWasPressed = true;
			bSceneLit = !bSceneLit;
		}
	}
	else
	{
		bSwitchLightWasPressed = false;
	}
}