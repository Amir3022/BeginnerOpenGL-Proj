#include "GameModules/AsteroidsGame.h"
#include "Utilities/EngineUtilities.h"
#include <random>

AsteroidsGame::AsteroidsGame(int in_width, int in_height)
	: Game(in_width, in_height)
{
	fragmentShaderPath = "Shaders/AsteroidsScene/FragmentShader.glsl";
	vertexShaderPath = "Shaders/AsteroidsScene/VertexShader.glsl";
	fixedSeed = 10;
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
		//Scale the Planet Model Up
		if (planetModel)
		{
			planetModel->SetTransform(planetModel->GetPosition(), planetModel->GetRotation(), glm::vec3(4.0f));
		}

		//Create Asteroid Model Instance
		asteroidModel = std::make_shared<Model>("Assets/Meshes/Asteroid/rock.obj");

		//Set the Camera Location further backwards to see the planet
		if (camera)
		{
			camera->SetCameraLocation(glm::vec3(0.0f, 0.0f, 80.0f));
		}

		//Generate transforms for 1000 asteroid to be drawn
		if (asteroidModel)
		{
			GenerateAsteroidsTransforms(10000);
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

	//Rotate the asteroids around the planet
	UpdateAsteroidsPositions();
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
		modelMat = glm::scale(modelMat, planetModel->GetScale());
		//Create the Normal Model Matrix to convert normal from local space to World coordinates while respecting scale
		glm::mat3 normalModelMatrix = glm::mat3(glm::transpose(glm::inverse(modelMat)));
		//Create the view matrix using camera lookAt target point
		glm::mat4 view = camera->GetLookAtMat(camera->GetCameraLocation() + camera->GetCameraForwardDir());
		//Create the projection matrix to project the view space to NDC
		glm::mat4 projection = glm::perspective(glm::radians(camera->GetCameraFOV()), (float)GetWidth() / (float)GetHeight(), 0.1f, 1000.0f);

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
		//Create the view matrix using camera lookAt target point
		glm::mat4 view = camera->GetLookAtMat(camera->GetCameraLocation() + camera->GetCameraForwardDir());
		//Create the projection matrix to project the view space to NDC
		glm::mat4 projection = glm::perspective(glm::radians(camera->GetCameraFOV()), (float)GetWidth() / (float)GetHeight(), 0.1f, 1000.0f);

		for (int i = 0; i < asteroidsTransforms.size(); i++)
		{
			//Create the model matrix to rotate the object in world space
			glm::mat4 modelMat = asteroidsTransforms[i];
			//Create the Normal Model Matrix to convert normal from local space to World coordinates while respecting scale
			glm::mat3 normalModelMatrix = glm::mat3(glm::transpose(glm::inverse(modelMat)));

			//Use the Shader Program to draw Vertices using the defined vertex and fragment shaders, and apply model, view, projection matrices
			shader->Use();
			shader->SetMat44("model", modelMat);
			shader->SetMat44("view", view);
			shader->SetMat44("projection", projection);
			shader->SetMat33("normalModelMatrix", normalModelMatrix);

			asteroidModel->Draw(shader);
		}
	}
}

void AsteroidsGame::ProcessInput(GLFWwindow* window)
{
	Game::ProcessInput(window);
}

void AsteroidsGame::GenerateAsteroidsTransforms(int amount)
{
	asteroidsTransforms.clear();
	asteroidsTransforms.reserve(amount);
	float radius = 60.0f;
	float offset = 10.0f;
	//Create RNG
	std::mt19937 rng(fixedSeed);
	std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
	std::uniform_real_distribution<float> scaleDist(0.05f, 0.25f);
	std::uniform_real_distribution<float> rotDist(0.0f, 1.0f);
	for (int i = 0; i < amount; i++)
	{
		glm::mat4 transform = glm::identity<glm::mat4>();
		float angle = ((float)i / amount) * 360.0f;	//Distribute the asteroids on a circle around the planet they are orbiting

		float displacement = dist(rng) * offset;	//Get Random displacement in the range of [-offset, offset]
		float x = glm::sin(glm::radians(angle)) * (radius + displacement);
		displacement = dist(rng) * offset;
		float y = displacement * 0.2f;
		displacement = dist(rng) * offset;
		float z = glm::cos(glm::radians(angle)) * (radius + displacement);
		transform = glm::translate(transform, glm::vec3(x, y, z));	//Apply displacement to the Transform Model Matrix

		
		float scale = scaleDist(rng);
		transform = glm::scale(transform, glm::vec3(scale));	//Apply random scale in the range 0.05, 0.25
		
		float xRot = rotDist(rng) * 360.0f;
		float yRot = rotDist(rng) * 360.0f;
		float zRot = rotDist(rng) * 360.0f;
		transform = glm::rotate(transform, glm::radians(xRot), glm::vec3(1.0f, 0.0f, 0.0f));
		transform = glm::rotate(transform, glm::radians(yRot), glm::vec3(0.0f, 1.0f, 0.0f));
		transform = glm::rotate(transform, glm::radians(zRot), glm::vec3(0.0f, 0.0f, 1.0f));	//Apply 3 random rotations in all 3 different rotation axes

		//Add the generated transform to the asteroids transforms vector
		asteroidsTransforms.push_back(transform);
	}
}

void AsteroidsGame::UpdateAsteroidsPositions()
{
	float radius = 60.0f;
	float offset = 10.0f;
	//Create RNG
	std::mt19937 rng(fixedSeed);
	std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
	for (int i = 0; i < asteroidsTransforms.size(); i++)
	{
		glm::mat4& transform = asteroidsTransforms[i];

		float angle = ((float)i / asteroidsTransforms.size()) * 360.0f + glfwGetTime() * 5.0f;	//Distribute the asteroids on a circle around the planet they are orbiting, add time to increase the angle each frame

		float displacement = dist(rng) * offset;	//Get Random displacement in the range of [-offset, offset]
		float x = glm::sin(glm::radians(angle)) * (radius + displacement);
		displacement = dist(rng) * offset;
		float y = transform[3][1];
		displacement = dist(rng) * offset;
		float z = glm::cos(glm::radians(angle)) * (radius + displacement);
		transform[3][0] = x;
		transform[3][1] = y;
		transform[3][2] = z;
	}
}
