#include "GameModules/AsteroidsGame.h"
#include "Utilities/EngineUtilities.h"

AsteroidsGame::AsteroidsGame(int in_width, int in_height)
	: Game(in_width, in_height)
{
	fragmentShaderPath = "Shaders/AsteroidsScene/FragmentShader.glsl";
	vertexShaderPath = "Shaders/AsteroidsScene/VertexShader.glsl";
}

bool AsteroidsGame::Init()
{
	if (!Game::Init())
		return false;

	try
	{
		//Create Shader Program from Class
		shader = std::make_shared<Shader>(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

		//Create Planet Model Instance
		planetModel = std::make_shared<Model>("Assets/Meshes/Planet/planet.obj");

		//Create Asteroid Model Instance
		asteroidModel = std::make_shared<Model>("Assets/Meshes/Asteroid/rock.obj");
		//Set inital asteroid model location
		if (asteroidModel)
		{
			asteroidModel->SetTransform(glm::vec3(-10.0f, 0.0, 0.0f), glm::vec3(0.0f), glm::vec3(0.2f));
		}

		//Set the Camera Location further backwards to see the planet
		if (camera)
		{
			camera->SetCameraLocation(glm::vec3(0.0f, 0.0f, 20.0f));
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

void AsteroidsGame::Terminate()
{
	Game::Terminate();
}

void AsteroidsGame::UpdateGame(float deltaTime)
{
	Game::UpdateGame(deltaTime);
	//Rotate the Planet around it's axis(For simplicity, assume it rotates around the y-axis)
	if (planetModel)
	{
		float planetRotationRate = 45.0f;
		planetModel->SetTransform(planetModel->GetPosition(), planetModel->GetRotation() + glm::vec3(0.0f, deltaTime * planetRotationRate, 0.0f), planetModel->GetScale());
	}
}

void AsteroidsGame::DrawFrame()
{
	Game::DrawFrame();

	glClearColor(0.03f, 0.03f, 0.03f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//If model is valid, draw it
	if (planetModel)
	{
		// Create Transform matrix to transform the drawn image
		//Create the model matrix to rotate the object in world space
		glm::mat4 modelMat = glm::identity<glm::mat4>();
		modelMat = glm::rotate(modelMat, glm::radians(planetModel->GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
		modelMat = glm::rotate(modelMat, glm::radians(planetModel->GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMat = glm::rotate(modelMat, glm::radians(planetModel->GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
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

		planetModel->Draw(shader);
	}
	//Draw the asteroid model
	if (asteroidModel)
	{
		// Create Transform matrix to transform the drawn image
		//Create the model matrix to rotate the object in world space
		glm::mat4 modelMat = glm::identity<glm::mat4>();
		modelMat = glm::translate(modelMat, asteroidModel->GetPosition());
		modelMat = glm::scale(modelMat, asteroidModel->GetScale());
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

		asteroidModel->Draw(shader);
	}
}

void AsteroidsGame::ProcessInput(GLFWwindow* window)
{
	Game::ProcessInput(window);
}