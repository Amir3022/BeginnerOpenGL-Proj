#include "GameModules/GSModelGame.h"

GSModelGame::GSModelGame(int in_width, int in_height)
	: Game(in_width, in_height)
{
	fragmentShaderPath = "Shaders/GSModelScene/FragmentShader.glsl";
	vertexShaderPath = "Shaders/GSModelScene/VertexShader.glsl";
	geometryShaderPath = "Shaders/GSModelScene/GeometryShader.glsl";

	normalVertexShaderPath = "Shaders/GSModelScene/NormalVertexShader.glsl";
	normalFragmentShaderPath = "Shaders/GSModelScene/NormalFragmentShader.glsl";
	normalGeometryShaderPath = "Shaders/GSModelScene/NormalGeometryShader.glsl";

	spotLightColor = glm::vec3(2.0f, 2.0f, 2.0f);
	dirLightColor = glm::vec3(0.98f, 0.98f, 0.5f) * 0.75f;
	dirLightOrient = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));

	bSceneLit = true;
	bSwitchLightWasPressed = false;

	bShowNormals = false;
	bToggleNormalWasPressed = false;
}

bool GSModelGame::Init()
{
	if (!Game::Init())
		return false;

	try
	{
		//Create Shader Program from Class
		shader = std::make_shared<Shader>(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

		//Try attaching Geometry Shader to the main program
		if (!shader->AttachGeometryShader(geometryShaderPath.c_str()))
		{
			std::cout << "Failed to Attach Geometry Shader" << std::endl;
			throw std::exception();
		}

		//Create Normal Shader Program to draw normal directions
		normalShader = std::make_shared<Shader>(normalVertexShaderPath.c_str(), normalFragmentShaderPath.c_str());
		//Try attaching Geometry Shader to Normal Shader Program
		if (!normalShader->AttachGeometryShader(normalGeometryShaderPath.c_str()))
		{
			std::cout << "Failed to Attach Geometry Shader" << std::endl;
			throw std::exception();
		}

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

void GSModelGame::Terminate()
{
	Game::Terminate();
}

void GSModelGame::UpdateGame(float deltaTime)
{
	Game::UpdateGame(deltaTime);
}

void GSModelGame::DrawFrame()
{
	Game::DrawFrame();

	glClearColor(0.03f, 0.03f, 0.03f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw the Main Model Scene
	DrawMainScene();

	//Draw the Normals on top of the previous drew scene
	if (bShowNormals)
	{
		DrawNormals();
	}
}

void GSModelGame::DrawMainScene()
{
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

		//Set the viewer (Camera) world position
		shader->SetVec3("cameraPos", camera->GetCameraLocation());

		//Setting Light struct properties (Setup the light Source as a flashlight, a spotlight originating from camera position)
		shader->SetVec3("spotLight.sourcePos", camera->GetCameraLocation());
		shader->SetVec3("spotLight.sourceDir", camera->GetCameraForwardDir());
		shader->SetFloat("spotLight.innerRadiusCos", glm::cos(glm::radians(12.5f)));
		shader->SetFloat("spotLight.outerRadiusCos", glm::cos(glm::radians(15.0f)));
		shader->SetVec3("spotLight.light.ambient", 0.1f * spotLightColor);
		shader->SetVec3("spotLight.light.diffuse", 0.75f * spotLightColor);
		shader->SetVec3("spotLight.light.specular", 1.0f * spotLightColor);
		shader->SetFloat("spotLight.constant", 1.0f);	//Attenuation constants for a light source that covers and outer radius on 50 units
		shader->SetFloat("spotLight.linear", 0.09f);
		shader->SetFloat("spotLight.quad", 0.032f);

		//Rendering directional Light
		shader->SetVec3("dirLight.sourceDir", dirLightOrient);
		shader->SetVec3("dirLight.light.ambient", 0.1f * dirLightColor);
		shader->SetVec3("dirLight.light.diffuse", 0.75f * dirLightColor);
		shader->SetVec3("dirLight.light.specular", 1.0f * dirLightColor);

		//Set the Lit Mode variable
		shader->SetBool("bLit", bSceneLit);

		//Set time variable used in Geometry Shader
		shader->SetFloat("time", glfwGetTime());

		model->Draw(shader);
	}
}

void GSModelGame::DrawNormals()
{
	//Check if the Normal Shader is valid
	if (normalShader)
	{
		// Create Transform matrix to transform the drawn image
		//Create the model matrix to rotate the object in world space
		glm::mat4 modelMat = glm::identity<glm::mat4>();
		//Create the view matrix using camera lookAt target point
		glm::mat4 view = camera->GetLookAtMat(camera->GetCameraLocation() + camera->GetCameraForwardDir());
		//Create the projection matrix to project the view space to NDC
		glm::mat4 projection = glm::perspective(glm::radians(camera->GetCameraFOV()), (float)GetWidth() / (float)GetHeight(), 0.1f, 100.0f);

		//Use the Shader Program to draw Vertices using the defined vertex and fragment shaders, and apply model, view, projection matrices
		normalShader->Use();
		normalShader->SetMat44("model", modelMat);
		normalShader->SetMat44("view", view);
		normalShader->SetMat44("projection", projection);

		//Draw the normal direction vectors on top of the previous scene
		model->Draw(normalShader);
	}
}


void GSModelGame::ProcessInput(GLFWwindow* window)
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

	//Toggle Normals rendering when pressing N
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
	{
		if (!bToggleNormalWasPressed)
		{
			bToggleNormalWasPressed = true;
			bShowNormals = !bShowNormals;
		}
	}
	else
	{
		bToggleNormalWasPressed = false;
	}

}
