#include "GameModules/GLSLGame.h"
#include "Utilities/EngineUtilities.h"

GLSLGame::GLSLGame(int in_width, int in_height)
	:Game(in_width, in_height)
{
	fragmentShaderPath = "Shaders/AGLSLScene/FragmentShader.glsl";
	vertexShaderPath = "Shaders/AGLSLScene/VertexShader.glsl";

	dirLightColor = glm::vec3(0.98f, 0.98f, 0.98f) * 2.0f;
	dirLightOrient = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));

	bSceneLit = false;
	bSwitchLightWasPressed = false;
}

bool GLSLGame::Init()
{
	if (!Game::Init())
		return false;

	try
	{
		//Create Shader Program from Class
		shader = std::make_shared<Shader>(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

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

		//Load two Images into 2 different textures
		unsigned int texture1 = EngineUtilities::LoadImageIntoTexture("Assets/Textures/container.jpg");

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

		return true;
	}
	catch (std::exception e)
	{
		std::cout << "Shader creation threw an exception!" << std::endl;
		glfwTerminate();
		return false;
	}
}

void GLSLGame::Terminate()
{
	Game::Terminate();
}

void GLSLGame::UpdateGame(float deltaTime)
{
	Game::UpdateGame(deltaTime);

	//Update the meshes location if needed
}

void GLSLGame::DrawFrame()
{
	Game::DrawFrame();

	glEnable(GL_STENCIL_TEST);
	glClearColor(0.03f, 0.03f, 0.03f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_PROGRAM_POINT_SIZE);
	//If model is valid, draw it
	if (meshes.size() > 0)
	{
		//For first cube with index 0, draw an white outline
		for (int i = meshes.size() - 1; i >= 0; i--)
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

void GLSLGame::ProcessInput(GLFWwindow* window)
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