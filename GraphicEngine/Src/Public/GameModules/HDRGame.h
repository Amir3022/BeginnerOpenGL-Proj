#pragma once
#include "Game.h"
#include "Components/Mesh.h"

class HDRGame : public Game
{
public:
	HDRGame(int in_width, int in_height);

	virtual bool Init() override;

	virtual void Terminate() override;

	virtual void UpdateGame(float deltaTime) override;

	virtual void DrawFrame() override;

protected:
	void DrawMainScene();
	void DrawHDRScene();

	virtual void ProcessInput(GLFWwindow* window) override;

private:
	//Meshes Variables
	std::vector<std::shared_ptr<Mesh>> wallMeshes;
	bool bRenderModel;

	//HDR Framebuffer
	unsigned int FBO;
	unsigned int colorTexture;
	std::shared_ptr<Shader> HDRShader;
	std::string HDRVertexShaderPath;
	std::string HDRFragmentShaderPath;

	//HDR Quad variables
	unsigned int HDRVAO;
	float exposureValue;
	bool bUseHDR;

	//Input variables
	bool bHDRTogglePressed;

	//Light Variables
	std::vector<glm::vec3> pointLightsPos;
	std::vector<glm::vec3> pointLightsColor;
};