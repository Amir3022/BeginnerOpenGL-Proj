#pragma once
#include "Game.h"
#include "Components/Model.h"

class GSModelGame : public Game
{
public:
	GSModelGame(int in_width, int in_height);

	virtual bool Init() override;

	virtual void Terminate() override;

	virtual void UpdateGame(float deltaTime) override;

	virtual void DrawFrame() override;

protected:
	virtual void ProcessInput(GLFWwindow* window) override;

private:
	//Model Variables
	std::shared_ptr<Model> model;

	//Normal Draw shader
	std::shared_ptr<Shader> normalShader;

	//Extra Shaders
	std::string geometryShaderPath;
	std::string normalVertexShaderPath;
	std::string normalFragmentShaderPath;
	std::string normalGeometryShaderPath;

	//Light source variables
	glm::vec3 dirLightOrient;
	bool bSceneLit;

	//Colors Variables
	glm::vec3 spotLightColor;
	glm::vec3 dirLightColor;

	//Input Handling variable
	bool bSwitchLightWasPressed;
};