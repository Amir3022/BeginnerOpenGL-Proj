#pragma once
#include "Game.h"
#include "Components/Mesh.h"

class CubemapGame : public Game
{
public:
	CubemapGame(int in_width, int in_height);

	virtual bool Init() override;

	virtual void Terminate() override;

	virtual void UpdateGame(float deltaTime) override;

	virtual void DrawFrame() override;

protected:
	virtual void ProcessInput(GLFWwindow* window) override;

private:
	void DrawMainScene();
	void DrawCubemap();

private:
	//Cubemap Shader Variables
	std::shared_ptr<Shader> cubemapShader;
	std::string cubemapVertexShaderPath;
	std::string cubemapFragmentShaderPath;

	//Cubemap Arrays variables
	unsigned int cubemapVAO;
	unsigned int cubemapTexture;

	//Meshes Variables
	std::vector<std::shared_ptr<Mesh>> meshes;

	//Light source variables
	glm::vec3 dirLightOrient;
	glm::vec3 dirLightColor;
	bool bSceneLit;

	//Input Handling variable
	bool bSwitchLightWasPressed;
};
