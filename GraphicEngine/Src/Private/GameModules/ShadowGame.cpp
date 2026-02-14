#include "GameModules/ShadowGame.h"
#include "Utilities/EngineUtilities.h"

ShadowGame::ShadowGame(int in_width, int in_height)
	: Game(in_width, in_height)
{
	vertexShaderPath = "Shaders/ShadowScene/VertexShader.glsl";
	fragmentShaderPath = "Shaders/ShadowScene/FragmentShader.glsl";

	dirLightDirection = glm::normalize(glm::vec3(1.0f, -1.0, -1.0f));
	dirLightColor = glm::vec3(0.98f, 0.98f, 0.98f) * 0.8f;
}

bool ShadowGame::Init()
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
		unsigned int texture1 = EngineUtilities::LoadImageIntoTexture("Assets/Textures/WoodenFloor.png", false, true);

		//Create Plane
		//Create Plane Texture Object
		Texture texture_Plane_diffuse;
		texture_Plane_diffuse.texIndex = texture1;
		texture_Plane_diffuse.texType = ETextureType::diffuse;
		texture_Plane_diffuse.path = "Assets/Textures/WoodenFloor.png";
		std::vector<Texture> textures_Plane{ texture_Plane_diffuse };
		//Create Plane Mesh
		std::shared_ptr<Mesh> planeMesh = std::make_shared<Mesh>(planeVertices, planeIndices, textures_Plane);
		//Set Plane Transform
		planeMesh->SetTransform(glm::vec3(0.0f), glm::vec3(90.0, 0.0f, 0.0f), glm::vec3(20.0f));
		//Add the plane mesh to meshes vector
		meshes.push_back(planeMesh);
		bUseTiling.push_back(true);

		//Load textures for wooden box with metal frame
		unsigned int texture2 = EngineUtilities::LoadImageIntoTexture("Assets/Textures/container2.png", false, true);
		unsigned int texture3 = EngineUtilities::LoadImageIntoTexture("Assets/Textures/container2_specular.png", false, true);

		//Create texture box for the wooden boxes
		Texture texture_2_diffuse;
		texture_2_diffuse.texIndex = texture2;
		texture_2_diffuse.texType = ETextureType::diffuse;
		texture_2_diffuse.path = "Assets/Textures/container2.png";
		Texture texture_2_specular;
		texture_2_specular.texIndex = texture3;
		texture_2_specular.texType = ETextureType::specular;
		texture_2_specular.path = "Assets/Textures/container2_specular.png";
		std::vector<Texture>textures_2{ texture_2_diffuse, texture_2_specular };

		//Create First Cube Mesh
		std::shared_ptr<Mesh> cube_1 = std::make_shared<Mesh>(vertices, indices, textures_2);
		//Set First Cube Transform
		cube_1->SetTransform(glm::vec3(0.0f, 0.5f, 0.0f));
		//Add First cube to meshes Vector
		meshes.push_back(cube_1);
		bUseTiling.push_back(false);

		//Create Second Cube Mesh 
		std::shared_ptr<Mesh> cube_2 = std::make_shared<Mesh>(vertices, indices, textures_2);
		//Set Second Cube Transform
		cube_2->SetTransform(glm::vec3(4.0f, 0.5f, -4.0f));
		//Add second cube to meshes Vector
		meshes.push_back(cube_2);
		bUseTiling.push_back(false);


		//Create Third Cube Mesh 
		std::shared_ptr<Mesh> cube_3 = std::make_shared<Mesh>(vertices, indices, textures_2);
		//Set Second Cube Transform
		cube_3->SetTransform(glm::vec3(2.0f, 3.0f, -2.0f), glm::vec3(15.0f, 30.0f, 0.0f));
		//Add second cube to meshes Vector
		meshes.push_back(cube_3);
		bUseTiling.push_back(false);


		return true;
	}
	catch (std::exception e)
	{
		std::cout << "Shader creation threw an exception!" << std::endl;
		glfwTerminate();
		return false;
	}
}

void ShadowGame::Terminate()
{
	Game::Terminate();
}

void ShadowGame::UpdateGame(float deltaTime)
{
	Game::UpdateGame(deltaTime);

	//Update the meshes location if needed
}

void ShadowGame::DrawFrame()
{
	Game::DrawFrame();

	//Bind the main framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//Enable Depth Testing, and clear color and depth buffers
	glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Enable Gamme Correction using sRGB Colors in Framebuffers
	glEnable(GL_FRAMEBUFFER_SRGB);

	//Draw the Main scene with the Wooden floor and the Point light source
	DrawMainScene();
}

void ShadowGame::DrawMainScene()
{
	//Check if the meshes is valid
	if (shader && meshes.size() > 0)
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

		//Rendering directional Light
		shader->SetVec3("dirLight.sourceDir", dirLightDirection);
		shader->SetVec3("dirLight.light.ambient", 0.1f * dirLightColor);
		shader->SetVec3("dirLight.light.diffuse", 0.75f * dirLightColor);
		shader->SetVec3("dirLight.light.specular", 1.0f * dirLightColor);

		for (int i = 0; i < meshes.size(); i++)
		{
			std::shared_ptr<Mesh> mesh = meshes[i];
			if (mesh)
			{
				//Create a model matrix to set plane location in world coordinates
				glm::mat4 modelMat = glm::identity < glm::mat4>();
				modelMat = glm::translate(modelMat, mesh->GetPosition());
				modelMat = glm::rotate(modelMat, glm::radians(mesh->GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
				modelMat = glm::rotate(modelMat, glm::radians(mesh->GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
				modelMat = glm::rotate(modelMat, glm::radians(mesh->GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
				modelMat = glm::scale(modelMat, mesh->GetScale());
				//Create normal model matrix to transform fragment normals
				glm::mat3 normalModelMatrix = glm::mat3(glm::transpose(glm::inverse(modelMat)));

				//Set changing Model and normal model matrices
				shader->SetMat44("model", modelMat);
				shader->SetMat33("normalModelMatrix", normalModelMatrix);

				//Set using tiling uniform
				shader->SetBool("bUseTiling", bUseTiling[i]);
				
				//Draw the Wooden floor mesh
				mesh->Draw(shader);
			}
		}
	}
}