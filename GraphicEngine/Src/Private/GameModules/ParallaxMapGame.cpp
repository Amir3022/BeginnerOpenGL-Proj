#include "GameModules/ParallaxMapGame.h"
#include "Utilities/EngineUtilities.h"

ParallaxMapGame::ParallaxMapGame(int in_width, int in_height)
	: Game(in_width, in_height)
{
	vertexShaderPath = "Shaders/ParallaxMapScene/VertexShader.glsl";
	fragmentShaderPath = "Shaders/ParallaxMapScene/FragmentShader.glsl";

	lightFragmentShaderPath = "Shaders/ParallaxMapScene/LightFragmentShader.glsl";
	lightVertexShaderPath = "Shaders/ParallaxMapScene/LightVertexShader.glsl";

	//Initialize Light Variables
	pointLightPos = glm::vec3(0.0f, 0.0f, 0.3f);
	pointLightColor = glm::vec3(1.0f);

	bUseNormalMap = false;
}

bool ParallaxMapGame::Init()
{
	if (!Game::Init())
		return false;

	try
	{
		//Create Shader Program from Class
		shader = std::make_shared<Shader>(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

		//Create Light Shader to render point lights
		lightShader = std::make_shared<Shader>(lightVertexShaderPath.c_str(), lightFragmentShaderPath.c_str());

		//Change Camera transform
		camera->SetCameraLocation(camera->GetCameraLocation() + glm::vec3(0.0f, 0.0f, 5.0f));
		camera->SetCameraRotation(camera->GetCameraRotation() + glm::vec3(0.0f, 0.0f, 0.0f));

		//Declare Vertices for a 2D plane (Position, Normal, Tangent, Tex Coord)
		std::vector<float> planeVertices =
		{
			-1.0f, -1.0f, 0.0f,		0.0f, 0.0f, 1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f,			//0		//0
			-1.0, -1.0f, -0.0001f,  0.0f, 0.0f, -1.0f,	-1.0f, 0.0f, 0.0f,	0.0f, 0.0f,			//1	

			1.0f, -1.0f, 0.0f,		0.0f, 0.0f, 1.0f,	1.0f, 0.0f, 0.0f,	1.0f, 0.0f,			//2		//1
			1.0f, -1.0f, -0.0001f,	0.0f, 0.0f, -1.0f,	-1.0f, 0.0f, 0.0f,	1.0f, 0.0f,			//3

			-1.0f,  1.0f, 0.0f,		0.0f, 0.0f, 1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 1.0f,			//4		//2
			-1.0f,  1.0f, -0.0001f, 0.0f, 0.0f, -1.0f,	-1.0f, 0.0f, 0.0f,	0.0f, 1.0f,			//5

			1.0f,  1.0f, 0.0f,		0.0f, 0.0f, 1.0f,	1.0f, 0.0f, 0.0f,	1.0f, 1.0f,			//6		//3
			1.0f,  1.0f, -0.0001f,	0.0f, 0.0f, -1.0f,	-1.0f, 0.0f, 0.0f,	1.0f, 1.0f,			//7
		};
		std::vector<unsigned int> planeIndices =
		{
			0, 2, 4,
			4, 2, 6,

			1, 3, 5,
			5, 3, 7
		};

		//Create VAO and bind
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		//Create VBO and EBO
		unsigned int VBO, EBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, planeVertices.size() * sizeof(float), planeVertices.data(), GL_STATIC_DRAW);
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, planeIndices.size() * sizeof(unsigned int), planeIndices.data(), GL_STATIC_DRAW);
		//Assign Vertex Attribute Pointers
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
		//Unbind the VAO
		glBindVertexArray(0);


		//Load Brick Wall textures
		diffuseTexture = EngineUtilities::LoadImageIntoTexture("Assets/Textures/bricks2.jpg", false, true, false);
		normalTexture = EngineUtilities::LoadImageIntoTexture("Assets/Textures/bricks2_normal.jpg", false, false, false);	//Disable sRGB for normal maps since it messes up the RGB values
		displacementTexture = EngineUtilities::LoadImageIntoTexture("Assets/Textures/bricks2_disp.jpg", false, false, false);	//Disable sRGB for normal maps since it messes up the RGB values

		//Set Plane Transform
		position = glm::vec3(0.0f);
		rotation = glm::vec3(0.0f);
		scale = glm::vec3(4.0f);

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

		//Create a cube mesh to represent the point light position
		lightCubeMesh = std::make_shared<Mesh>(vertices, indices, std::vector<Texture>{});
		//Set lightCubeMesh transform
		lightCubeMesh->SetTransform(pointLightPos, glm::vec3(0.f), glm::vec3(0.025f));	//Scale the cube down so it doesn't take much screen realstate

		return true;
	}
	catch (std::exception e)
	{
		std::cout << "Shader creation threw an exception!" << std::endl;
		glfwTerminate();
		return false;
	}
}

void ParallaxMapGame::Terminate()
{
	Game::Terminate();
}

void ParallaxMapGame::UpdateGame(float deltaTime)
{
	Game::UpdateGame(deltaTime);

	//Update the point light position
	if (lightCubeMesh)
	{
		pointLightPos.y = glm::sin(glfwGetTime()) * 3.0f;
		lightCubeMesh->SetTransform(pointLightPos, glm::vec3(0.0f), lightCubeMesh->GetScale());
	}
}

void ParallaxMapGame::DrawFrame()
{
	Game::DrawFrame();

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

void ParallaxMapGame::DrawMainScene()
{
	//Check if the wall Mesh is valid
	if (shader)
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

		//Rendering point Light
		shader->SetVec3("pointLightPos", pointLightPos);
		shader->SetVec3("pointLightAmbient", 0.2f * glm::normalize(pointLightColor));
		shader->SetVec3("pointLightDiffuse", 0.75f * pointLightColor);
		shader->SetVec3("pointLightSpecular", 1.0f * pointLightColor);
		//shader->SetFloat("pointLights[" + std::to_string(0) + "].constant", 1.0f);	//Attenuation constants for a light source that covers and outer radius on 50 units
		//shader->SetFloat("pointLights[" + std::to_string(0) + "].linear", 0.35f);
		//shader->SetFloat("pointLights[" + std::to_string(0) + "].quad", 0.44f);

		//Rendering Wall Plane
		//Create a model matrix to set plane location in world coordinates
		glm::mat4 modelMat = glm::identity < glm::mat4>();
		modelMat = glm::translate(modelMat, position);
		modelMat = glm::rotate(modelMat, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		modelMat = glm::rotate(modelMat, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMat = glm::rotate(modelMat, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		modelMat = glm::scale(modelMat, scale);
		//Create normal model matrix to transform fragment normals
		glm::mat3 normalModelMatrix = glm::mat3(glm::transpose(glm::inverse(modelMat)));

		//Set changing Model and normal model matrices
		shader->SetMat44("model", modelMat);
		shader->SetMat33("normalModelMatrix", normalModelMatrix);

		//Enable using tiling uniform for all wall meshes
		shader->SetBool("bUseTiling", false);

		//Enable using Normal map for the Wall Mesh
		shader->SetBool("bUseNormalMap", bUseNormalMap);

		//Bind textures to Texture Parameters
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalTexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, displacementTexture);

		//Set the Textures variables in the Fragment Shader
		shader->SetInt("diffuseTexture", 0);
		shader->SetInt("normalTexture", 1);
		shader->SetInt("displacementTexture", 2);

		//Set displacement Height Scale Value
		shader->SetFloat("displacementHeightScale", 0.1f);


		//Bind the VAO holding the Wall vertices
		glBindVertexArray(VAO);
		//Draw the Wooden floor mesh
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);

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
	}
}

void ParallaxMapGame::ProcessInput(GLFWwindow* window)
{
	Game::ProcessInput(window);
	//Cycle between Lit and Unlit Modes in Rendering Model when pressing P
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		if (!bUseNormalMapSwitchPressed)
		{
			bUseNormalMapSwitchPressed = true;
			bUseNormalMap = !bUseNormalMap;
		}
	}
	else
	{
		bUseNormalMapSwitchPressed = false;
	}
}