#include "GameModules/ModelGame.h"

ModelGame::ModelGame(int in_width, int in_height)
	: Game(in_width, in_height)
{
	fragmentShaderPath = "Shaders/MeshScene/FragmentShader.glsl";
	vertexShaderPath = "Shaders/MeshScene/VertexShader.glsl";

	lightFragmentShaderPath = "Shaders/MeshScene/LightFragmentShader.glsl";
	lightVertexShaderPath = "Shaders/MeshScene/LightVertexShader.glsl";

	moveLightSource = false;
	changeLightColor = false;

	spotLightColor = glm::vec3(2.0f, 2.0f, 2.0f);
	dirLightColor = glm::vec3(0.98f, 0.98f, 0.5f) * 0.4f;
	dirLightOrient = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
}

bool ModelGame::Init()
{
	if (!Game::Init())
		return false;

	try
	{
		//Create Shader Program from Class
		shader = std::make_shared<Shader>(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

		//Create Light Shader to render the Light Cube
		lightShader = std::make_unique<Shader>(lightVertexShaderPath.c_str(), lightFragmentShaderPath.c_str());

		//Create a Model Instance
		model = std::make_shared<Model>("Assets/Meshes/backpack/backpack.obj");

		return true;
	}
	catch (std::exception e)
	{
		std::cout << "Shader creation threw an exception!" << std::endl;
		glfwTerminate();
		return false;
	}
}

void ModelGame::Terminate()
{
	Game::Terminate();
}

void ModelGame::UpdateGame(float deltaTime)
{
	Game::UpdateGame(deltaTime);
}

void ModelGame::DrawFrame()
{
	Game::DrawFrame();

	glClearColor(0.03f, 0.03f, 0.03f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//If model is valid, draw it
	if (model)
	{
		// Create Transform matrix to transform the drawn image
		//Create the model matrix to rotate the object in world space
		glm::mat4 modelMat = glm::identity<glm::mat4>();
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

		model->Draw(shader);
	}
}
