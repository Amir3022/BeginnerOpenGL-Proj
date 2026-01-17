#include "GameModules/DepthTestGame.h"
#include "Utilities/EngineUtilities.h"

DepthTestGame::DepthTestGame(int in_width, int in_height)
: Game(in_width, in_height)
{
	fragmentShaderPath = "Shaders/MeshScene/FragmentShader.glsl";
	vertexShaderPath = "Shaders/MeshScene/VertexShader.glsl";

	lightFragmentShaderPath = "Shaders/MeshScene/LightFragmentShader.glsl";
	lightVertexShaderPath = "Shaders/MeshScene/LightVertexShader.glsl";

	dirLightColor = glm::vec3(0.98f, 0.98f, 0.98f) * 2.0f;
	dirLightOrient = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));

	bSceneLit = true;
	bSwitchLightWasPressed = false;
}

bool DepthTestGame::Init()
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

			1, 3, 5,
			5, 3, 7
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
		std::vector<Texture>textures_2{texture_2_diffuse, texture_2_specular};
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

		return true;
	}
	catch (std::exception e)
	{
		std::cout << "Shader creation threw an exception!" << std::endl;
		glfwTerminate();
		return false;
	}
}

void DepthTestGame::Terminate()
{
	Game::Terminate();
}

void DepthTestGame::UpdateGame(float deltaTime)
{
	Game::UpdateGame(deltaTime);

	//Update the meshes location if needed
}

void DepthTestGame::DrawFrame()
{
	Game::DrawFrame();

	glClearColor(0.03f, 0.03f, 0.03f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//If model is valid, draw it
	if (meshes.size() > 0)
	{
		for (int i = 0; i < meshes.size(); i++)
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
}

void DepthTestGame::ProcessInput(GLFWwindow* window)
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