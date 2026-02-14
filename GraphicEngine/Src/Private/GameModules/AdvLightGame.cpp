#include "GameModules/AdvLightGame.h"
#include "Utilities/EngineUtilities.h"

AdvLightGame::AdvLightGame(int in_width, int in_height)
	: Game(in_width, in_height)
{
	vertexShaderPath = "Shaders/AdvLightScene/VertexShader.glsl";
	fragmentShaderPath = "Shaders/AdvLightScene/FragmentShader.glsl";

	bUseBlinn = false;
	bSwitchLightModelWasPressed = false;

	pointLightPos = glm::vec3(0.0f, 1.0f, 0.0f);
	pointLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
}

bool AdvLightGame::Init()
{
	if (!Game::Init())
		return false;

	try
	{
		//Create Shader Program from Class
		shader = std::make_shared<Shader>(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

		//Change Camera transform
		camera->SetCameraLocation(camera->GetCameraLocation() + glm::vec3(0.0f, 2.0f, 2.0f));
		camera->SetCameraRotation(camera->GetCameraRotation() + glm::vec3(0.0f, -15.0f, 0.0f));

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

		//Load Wooden floor Image into texture
		unsigned int texture1 = EngineUtilities::LoadImageIntoTexture("Assets/Textures/WoodenFloor.png");

		//Create Plane
		//Create Plane Texture Object
		Texture texture_Plane_diffuse;
		texture_Plane_diffuse.texIndex = texture1;
		texture_Plane_diffuse.texType = ETextureType::diffuse;
		texture_Plane_diffuse.path = "Assets/Textures/WoodenFloor.png";
		std::vector<Texture> textures_Plane{ texture_Plane_diffuse };
		//Create Plane Mesh
		planeMesh = std::make_shared<Mesh>(planeVertices, planeIndices, textures_Plane);
		//Set Plane Transform
		planeMesh->SetTransform(glm::vec3(0.0f), glm::vec3(90.0, 0.0f, 0.0f), glm::vec3(20.0f));

		return true;
	}
	catch (std::exception e)
	{
		std::cout << "Shader creation threw an exception!" << std::endl;
		glfwTerminate();
		return false;
	}
}

void AdvLightGame::Terminate()
{
	Game::Terminate();
}

void AdvLightGame::UpdateGame(float deltaTime)
{
	Game::UpdateGame(deltaTime);

	//Update the meshes location if needed
}

void AdvLightGame::DrawFrame()
{
	Game::DrawFrame();

	//Bind the main framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//Enable Depth Testing, and clear color and depth buffers
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw the Main scene with the Wooden floor and the Point light source
	DrawMainScene();
}

void AdvLightGame::DrawMainScene()
{
	//Check if the planeMesh is valid
	if (planeMesh)
	{
		//Create a model matrix to set plane location in world coordinates
		glm::mat4 modelMat = glm::identity < glm::mat4>();
		modelMat = glm::translate(modelMat, planeMesh->GetPosition());
		modelMat = glm::rotate(modelMat, glm::radians(planeMesh->GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
		modelMat = glm::rotate(modelMat, glm::radians(planeMesh->GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMat = glm::rotate(modelMat, glm::radians(planeMesh->GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
		modelMat = glm::scale(modelMat, planeMesh->GetScale());
		//Create normal model matrix to transform fragment normals
		glm::mat3 normalModelMatrix = glm::mat3(glm::transpose(glm::inverse(modelMat)));
		//Create the View matrix to see the plane model through the camera position
		glm::mat4 view = camera->GetLookAtMat(camera->GetCameraLocation() + camera->GetCameraForwardDir());
		//Create projection matrix to project the scene with depth through camera lens field of view and project view space to NDC
		glm::mat4 projection = glm::perspective(glm::radians(camera->GetCameraFOV()), (float)GetWidth() / (float)GetHeight(), 0.1f, 1000.0f);

		//Use the shader program, and set the matrices
		shader->Use();
		shader->SetMat44("model", modelMat);
		shader->SetMat33("normalModelMatrix", normalModelMatrix);
		shader->SetMat44("view", view);
		shader->SetMat44("projection", projection);

		//Set the viewer (Camera) world position
		shader->SetVec3("cameraPos", camera->GetCameraLocation());

		//Setting point light properties in the Fragment shader
		shader->SetVec3("pointLights[" + std::to_string(0) + "].sourcePos", pointLightPos);
		shader->SetVec3("pointLights[" + std::to_string(0) + "].light.ambient", 0.1f * glm::normalize(pointLightColor));
		shader->SetVec3("pointLights[" + std::to_string(0) + "].light.diffuse", 0.75f * pointLightColor);
		shader->SetVec3("pointLights[" + std::to_string(0) + "].light.specular", 1.0f * pointLightColor);
		shader->SetFloat("pointLights[" + std::to_string(0) + "].constant", 1.0f);	//Attenuation constants for a light source that covers and outer radius on 50 units
		shader->SetFloat("pointLights[" + std::to_string(0) + "].linear", 0.09f);
		shader->SetFloat("pointLights[" + std::to_string(0) + "].quad", 0.032f);

		//Set the Lighting Mode used
		shader->SetBool("bBlinn", bUseBlinn);

		//Draw the Wooden floor mesh
		planeMesh->Draw(shader);
	}
}

void AdvLightGame::ProcessInput(GLFWwindow* window)
{
	Game::ProcessInput(window);

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		if (!bSwitchLightModelWasPressed)
		{
			bSwitchLightModelWasPressed = true;
			bUseBlinn = !bUseBlinn;
		}
	}
	else
	{
		bSwitchLightModelWasPressed = false;
	}
}