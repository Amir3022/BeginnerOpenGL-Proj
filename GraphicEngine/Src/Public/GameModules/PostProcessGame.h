#pragma once
#include "Game.h"
#include "Components/Mesh.h"

class PostProcessGame : public Game
{
public:
	PostProcessGame(int in_width, int in_height);

	virtual bool Init() override;

	virtual void Terminate() override;

	virtual void UpdateGame(float deltaTime) override;

	virtual void DrawFrame() override;

protected:
	virtual void ProcessInput(GLFWwindow* window) override;

private:
	void DrawMainScene();

private:
	//FrameBuffer variables
	unsigned int FBO;
	unsigned int RBO;
	unsigned int colorTexture;

	//Screen Quad render variables
	unsigned int ppVAO;
	std::shared_ptr<Shader> ppShader;
	std::string ppVertexShaderPath;
	std::string ppFragmentShaderPath;

	//Meshes Variables
	std::vector<std::shared_ptr<Mesh>> meshes;

	//Light source variables
	glm::vec3 dirLightOrient;
	glm::vec3 dirLightColor;
	bool bSceneLit;

	//Input Handling variable
	bool bSwitchLightWasPressed;
};