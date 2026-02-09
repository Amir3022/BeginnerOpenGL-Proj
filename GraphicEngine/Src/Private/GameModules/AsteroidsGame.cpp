#include "GameModules/AsteroidsGame.h"
#include "Utilities/EngineUtilities.h"
#include <random>

AsteroidsGame::AsteroidsGame(int in_width, int in_height)
	: Game(in_width, in_height)
{
	fragmentShaderPath = "Shaders/AsteroidsScene/FragmentShader.glsl";
	vertexShaderPath = "Shaders/AsteroidsScene/VertexShader.glsl";
	instancedVertexShaderPath = "Shaders/AsteroidsScene/InstancedVertexShader.glsl";

	cubemapFragmentShaderPath = "Shaders/CubemapScene/CubemapFragmentShader.glsl";
	cubemapVertexShaderPath = "Shaders/CubemapScene/CubemapVertexShader.glsl";

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

		//Create Instanced Shader Program
		instancedShader = std::make_shared<Shader>(instancedVertexShaderPath.c_str(), fragmentShaderPath.c_str());

		//Create CubeMap Shader
		cubemapShader = std::make_unique<Shader>(cubemapVertexShaderPath.c_str(), cubemapFragmentShaderPath.c_str());

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
			camera->SetCamMoveSpeed(50.0f);
		}

		//Generate transforms for 1000 asteroid to be drawn
		if (asteroidModel)
		{
			GenerateAsteroidsTransforms(10000);
		}

		//Generate Instanced Array Buffer, and add data to it
		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ARRAY_BUFFER, IBO);
		glBufferData(GL_ARRAY_BUFFER, asteroidsTransforms.size() * sizeof(glm::mat4), asteroidsTransforms.data(), GL_STATIC_DRAW);

		//Cycle through all meshes of the astronaut model, and bind the instanced array buffer object holding the asteroid instances transforms
		for (int i = 0; i < asteroidModel->GetMeshes().size(); i++)
		{
			//Bind the mesh VAO
			unsigned int currentVAO = asteroidModel->GetMeshes()[i].GetVAO();
			glBindVertexArray(currentVAO);
		
			//Assign Vertex Attribute Pointer(Mat4 data pointer should be added as 4 vec4 data)
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)0);
			glVertexAttribDivisor(3, 1);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(sizeof(glm::vec4)));
			glVertexAttribDivisor(4, 1);
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec4)));
			glVertexAttribDivisor(5, 1);
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(3 * sizeof(glm::vec4)));
			glVertexAttribDivisor(6, 1);

			//Unbind the current bound mesh VAO
			glBindVertexArray(0);
		}

		//Create VAO for the Post Process quad having only a single quad taking the whole screen real state
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

		//Load the Cubemap texture
		std::vector<std::string> cubemapImagePaths{
			"Assets/Textures/Cubemaps/SpaceBox/right.png",
			"Assets/Textures/Cubemaps/SpaceBox/left.png",
			"Assets/Textures/Cubemaps/SpaceBox/top.png",
			"Assets/Textures/Cubemaps/SpaceBox/bottom.png",
			"Assets/Textures/Cubemaps/SpaceBox/front.png",
			"Assets/Textures/Cubemaps/SpaceBox/back.png",
		};
		cubemapTexture = EngineUtilities::LoadImagesIntoCubemap(cubemapImagePaths);

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
		//Use the Instanced Shader program
		instancedShader->Use();
		//Create the view matrix using camera lookAt target point
		glm::mat4 view = camera->GetLookAtMat(camera->GetCameraLocation() + camera->GetCameraForwardDir());
		//Create the projection matrix to project the view space to NDC
		glm::mat4 projection = glm::perspective(glm::radians(camera->GetCameraFOV()), (float)GetWidth() / (float)GetHeight(), 0.1f, 1000.0f);

		instancedShader->SetMat44("view", view);
		instancedShader->SetMat44("projection", projection);

		//Cycle through all meshes, bind their VAO and draw elements instanced
		for (int i = 0; i < asteroidModel->GetMeshes().size(); i++)
		{
			glBindVertexArray(asteroidModel->GetMeshes()[i].GetVAO());
			unsigned int nr_diffuse = 1;
			unsigned int nr_specular = 1;
			//Cycle through all Textures, and set their Data in Fragment Shader
			for (int i = 0; i < asteroidModel->GetMeshes()[i].GetTextures().size(); i++)
			{
				Texture texture = asteroidModel->GetMeshes()[i].GetTextures()[i];
				//Set Active Texture Unit
				glActiveTexture(GL_TEXTURE0 + i);
				//Bind the Texture to the Active Texture Unit
				glBindTexture(GL_TEXTURE_2D, texture.texIndex);
				//Get the correct texture name to set in the Data in the Fragment Shader
				std::string textureName = "material.texture_";
				if (texture.texType == ETextureType::diffuse)
					textureName += "diffuse_" + std::to_string(nr_diffuse++);
				else if (texture.texType == ETextureType::specular)
					textureName += "specular_" + std::to_string(nr_specular++);
				instancedShader->SetInt(textureName, i);
			}
			glDrawElementsInstanced(GL_TRIANGLES, asteroidModel->GetMeshes()[i].GetIndices().size(), GL_UNSIGNED_INT, 0, asteroidsTransforms.size());

			//Unbind all bound textures
			for (int i = 0; i < asteroidModel->GetMeshes()[i].GetTextures().size(); i++)
			{
				//Set Active Texture Unit
				glActiveTexture(GL_TEXTURE0 + i);
				//Unbind texture in the Active Texture Unit
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			//Deactivate the last used Texture
			glActiveTexture(0);

			//Unbind the VAO
			glBindVertexArray(0);
		}
	}

	//Draw the Cubemap
	glDepthFunc(GL_LEQUAL);	//Change the Depth Test function to less than or equal, to make sure the cube map is rendered behind all other meshes in the scene
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
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
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


	//Bind Instanced Array Buffer, and update its data
	glBindBuffer(GL_ARRAY_BUFFER, IBO);
	glBufferData(GL_ARRAY_BUFFER, asteroidsTransforms.size() * sizeof(glm::mat4),nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, asteroidsTransforms.size() * sizeof(glm::mat4), asteroidsTransforms.data());
}
