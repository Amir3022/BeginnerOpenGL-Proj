#include "GameModules/CubemapGame.h"
#include "Utilities/EngineUtilities.h"

CubemapGame::CubemapGame(int in_width, int in_height)
	:Game(in_width, in_height)
{
	fragmentShaderPath = "Shaders/CubemapScene/FragmentShader.glsl";
	vertexShaderPath = "Shaders/CubemapScene/VertexShader.glsl";

	cubemapFragmentShaderPath = "Shaders/CubemapScene/CubemapFragmentShader.glsl";
	cubemapVertexShaderPath = "Shaders/CubemapScene/CubemapVertexShader.glsl";

	dirLightColor = glm::vec3(0.98f, 0.98f, 0.98f) * 2.0f;
	dirLightOrient = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));

	bSceneLit = true;
	bSwitchLightWasPressed = false;
}

bool CubemapGame::Init()
{
	if (!Game::Init())
		return false;

	try
	{
		//Create Shader Program from Class
		shader = std::make_shared<Shader>(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

		//Create CubeMap Shader
		cubemapShader = std::make_unique<Shader>(cubemapVertexShaderPath.c_str(), cubemapFragmentShaderPath.c_str());

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

		//Create Cube mesh
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
			"Assets/Textures/Cubemaps/skybox/right.jpg",
			"Assets/Textures/Cubemaps/skybox/left.jpg",
			"Assets/Textures/Cubemaps/skybox/top.jpg",
			"Assets/Textures/Cubemaps/skybox/bottom.jpg",
			"Assets/Textures/Cubemaps/skybox/front.jpg",
			"Assets/Textures/Cubemaps/skybox/back.jpg",
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

void CubemapGame::Terminate()
{
	Game::Terminate();
}

void CubemapGame::UpdateGame(float deltaTime)
{
	Game::UpdateGame(deltaTime);

	//Update the meshes location if needed
}

void CubemapGame::DrawFrame()
{
	Game::DrawFrame();

	//Draw the cube in World
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.03f, 0.03f, 0.03f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	DrawMainScene();

	//Draw the Cubemap
	glDepthFunc(GL_LEQUAL);	//Change the Depth Test function to less than or equal, to make sure the cube map is rendered behind all other meshes in the scene
	DrawCubemap();
}

void CubemapGame::ProcessInput(GLFWwindow* window)
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

void CubemapGame::DrawMainScene()
{
	//If meshes isn't empty, iterate through them and draw each mesh
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

			//Draw the Mesh
			mesh->Draw(shader);
		}
	}
}

void CubemapGame::DrawCubemap()
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