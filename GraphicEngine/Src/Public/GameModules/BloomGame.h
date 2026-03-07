#pragma once
#include "Game.h"
#include "Components/Mesh.h"

class BloomGame : public Game
{
public:
	BloomGame(int in_width, int in_height);

	virtual bool Init() override;

	virtual void Terminate() override;

	virtual void UpdateGame(float deltaTime) override;

	virtual void DrawFrame() override;

protected:
	void DrawMainScene();
	void DrawPPScene();
	void ApplyGaussianBlur();
	virtual void ProcessInput(GLFWwindow* window) override;

private:
	//Meshes Variables
	std::vector<std::shared_ptr<Mesh>> cubeMeshes;
	std::shared_ptr<Mesh> planeMesh;

	//Bloom framebuffer variables
	unsigned int mainFBO;
	unsigned int colorBuffers[2];
	std::shared_ptr<Shader> ppShader;
	std::string ppVertexShaderPath;
	std::string ppFragmentShaderPath;
	unsigned int swapFBOs[2];
	unsigned int swapColorBuffers[2];
	std::shared_ptr<Shader> blurShader;
	std::string blurVertexShaderPath;
	std::string blurFragmentShaderPath;
	bool bUseHorizontalGaussianPass;
	bool bUseBloom;

	//Framebuffer Quad variables
	unsigned int ppVAO;
	float exposureValue;
	bool bUseHDR;

	//Input variables
	bool bHDRTogglePressed;
	bool bBloomTogglePressed;

	//Light Variables
	std::vector<glm::vec3> pointLightsPos;
	std::vector<glm::vec3> pointLightsColor;

	//Rendering Light Variables
	std::shared_ptr<Shader> lightShader;
	std::shared_ptr<Mesh> lightCubeMesh;
	std::string lightVertexShaderPath;
	std::string lightFragmentShaderPath;
};