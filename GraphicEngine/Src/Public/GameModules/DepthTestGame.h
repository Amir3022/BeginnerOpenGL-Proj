#pragma once
#include "Game.h"
#include "Components/Mesh.h"

class DepthTestGame : public Game
{
public:
	DepthTestGame(int in_width, int in_height);

	virtual bool Init() override;

	virtual void Terminate() override;

	virtual void UpdateGame(float deltaTime) override;

	virtual void DrawFrame() override;

protected:
	virtual void ProcessInput(GLFWwindow* window) override;

private:
	//Meshes Variables
	std::vector<std::shared_ptr<Mesh>> meshes;

	//Light shader
	std::unique_ptr<Shader> lightShader;

	//Light Shaders
	std::string lightVertexShaderPath;
	std::string lightFragmentShaderPath;

	//Light source variables
	glm::vec3 dirLightOrient;
	glm::vec3 dirLightColor;
	bool bSceneLit;

	//Input Handling variable
	bool bSwitchLightWasPressed;
};
