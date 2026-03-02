#include "GameModules/NormalMapGame.h"
#include "Utilities/EngineUtilities.h"

NormalMapGame::NormalMapGame(int in_width, int in_height)
	: Game(in_width, in_height)
{
	vertexShaderPath = "Shaders/NormalMapScene/VertexShader.glsl";
	fragmentShaderPath = "Shaders/NormalMapScene/FragmentShader.glsl";

	lightFragmentShaderPath = "Shaders/NormalMapScene/LightFragmentShader.glsl";
	lightVertexShaderPath = "Shaders/NormalMapScene/LightVertexShader.glsl";

	//Initialize Light Variables
	pointLightPos = glm::vec3(0.0f, 0.0f, 1.0f);
	pointLightColor = glm::vec3(1.0f);

	bUseNormalMap = false;
}

bool NormalMapGame::Init()
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
		camera->SetCameraLocation(camera->GetCameraLocation() + glm::vec3(0.0f, 2.0f, 2.0f));
		camera->SetCameraRotation(camera->GetCameraRotation() + glm::vec3(0.0f, -15.0f, 0.0f));

		//Declare Vertices for a 2D plane
		std::vector<Vertex> planeVertices =
		{
			{glm::vec3(-1.0f, -1.0f, 0.0f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f)},		//0		//0
			{glm::vec3(-1.0, -1.0f, -0.0001f),  glm::vec3(0.0f,  0.0f,  -1.0f), glm::vec2(0.0f, 0.0f)},	//1	

			{glm::vec3(1.0f, -1.0f, 0.0f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f)},		//2		//1
			{glm::vec3(1.0f, -1.0f, -0.0001f),  glm::vec3(0.0f,  0.0f,  -1.0f), glm::vec2(1.0f, 0.0f)},	//3

			{glm::vec3(-1.0f,  1.0f, 0.0f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f)},		//4		//2
			{glm::vec3(-1.0f,  1.0f, -0.0001f),  glm::vec3(0.0f,  0.0f,  -1.0f), glm::vec2(0.0f, 1.0f)},//5

			{glm::vec3(1.0f,  1.0f, 0.0f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f)},		//6		//3
			{glm::vec3(1.0f,  1.0f, -0.0001f),  glm::vec3(0.0f,  0.0f,  -1.0f), glm::vec2(1.0f, 1.0f)},	//7
		};
		std::vector<unsigned int> planeIndices =
		{
			0, 2, 4,
			4, 2, 6,

			1, 3, 5,
			5, 3, 7
		};

		//Load Brick Wall textures
		unsigned int texture1 = EngineUtilities::LoadImageIntoTexture("Assets/Textures/brickwall.jpg", false, true);
		unsigned int texture2 = EngineUtilities::LoadImageIntoTexture("Assets/Textures/brickwall_normal.jpg", false, true);


		//Create Plane Mesh to be used as Wall
		//Create Plane Texture Object
		Texture texture_Plane_diffuse;
		texture_Plane_diffuse.texIndex = texture1;
		texture_Plane_diffuse.texType = ETextureType::diffuse;
		texture_Plane_diffuse.path = "Assets/Textures/brickwall.jpg";

		Texture texture_Plane_Normal;
		texture_Plane_Normal.texIndex = texture2;
		texture_Plane_Normal.texType = ETextureType::normal;
		texture_Plane_Normal.path = "Assets/Textures/brickwall_normal.jpg";
		
		std::vector<Texture> textures_Plane{ texture_Plane_diffuse, texture_Plane_Normal };

		wallMesh = std::make_shared<Mesh>(planeVertices, planeIndices, textures_Plane);
		//Set Plane Transform
		wallMesh->SetTransform(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(4.0f));


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
		lightCubeMesh->SetTransform(pointLightPos, glm::vec3(0.f), glm::vec3(0.1f));	//Scale the cube down so it doesn't take much screen realstate

		return true;
	}
	catch (std::exception e)
	{
		std::cout << "Shader creation threw an exception!" << std::endl;
		glfwTerminate();
		return false;
	}
}

void NormalMapGame::Terminate()
{
	Game::Terminate();
}

void NormalMapGame::UpdateGame(float deltaTime)
{
	Game::UpdateGame(deltaTime);

	//Update the point light position
	if (lightCubeMesh)
	{
		pointLightPos = glm::vec3(0.0f, glm::sin(glfwGetTime()) * 3.0f, 1.0f);
		lightCubeMesh->SetTransform(pointLightPos, glm::vec3(0.0f), lightCubeMesh->GetScale());
	}
}

void NormalMapGame::DrawFrame()
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

void NormalMapGame::DrawMainScene()
{
	//Check if the wall Mesh is valid
	if (shader && wallMesh)
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
		shader->SetVec3("pointLights[" + std::to_string(0) + "].sourcePos", pointLightPos);
		shader->SetVec3("pointLights[" + std::to_string(0) + "].light.ambient", 0.2f * glm::normalize(pointLightColor));
		shader->SetVec3("pointLights[" + std::to_string(0) + "].light.diffuse", 0.75f * pointLightColor);
		shader->SetVec3("pointLights[" + std::to_string(0) + "].light.specular", 1.0f * pointLightColor);
		shader->SetFloat("pointLights[" + std::to_string(0) + "].constant", 1.0f);	//Attenuation constants for a light source that covers and outer radius on 50 units
		shader->SetFloat("pointLights[" + std::to_string(0) + "].linear", 0.35f);
		shader->SetFloat("pointLights[" + std::to_string(0) + "].quad", 0.44f);

		//Rendering Wall Plane
		//Create a model matrix to set plane location in world coordinates
		glm::mat4 modelMat = glm::identity < glm::mat4>();
		modelMat = glm::translate(modelMat, wallMesh->GetPosition());
		modelMat = glm::rotate(modelMat, glm::radians(wallMesh->GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
		modelMat = glm::rotate(modelMat, glm::radians(wallMesh->GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMat = glm::rotate(modelMat, glm::radians(wallMesh->GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
		modelMat = glm::scale(modelMat, wallMesh->GetScale());
		//Create normal model matrix to transform fragment normals
		glm::mat3 normalModelMatrix = glm::mat3(glm::transpose(glm::inverse(modelMat)));

		//Set changing Model and normal model matrices
		shader->SetMat44("model", modelMat);
		shader->SetMat33("normalModelMatrix", normalModelMatrix);

		//Enable using tiling uniform for all wall meshes
		shader->SetBool("bUseTiling", true);

		//Enable using Normal map for the Wall Mesh
		shader->SetBool("bUseNormalMap", bUseNormalMap);

		//Draw the Wooden floor mesh
		wallMesh->Draw(shader);


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

void NormalMapGame::ProcessInput(GLFWwindow* window)
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