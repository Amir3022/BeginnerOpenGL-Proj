#pragma once
#include "Game.h"

class AAGame : public Game
{
public:
	AAGame(int in_width, int in_height);

	virtual bool Init() override;

	virtual void Terminate() override;

	virtual void UpdateGame(float deltaTime) override;

	virtual void DrawFrame() override;

private:
	//Framebuffer creation function
	bool CreateFramebuffer(int n_samples);
	//Drawing Functions
	void DrawMainScene();
	void DrawPPScene();

	//Input Handling Functions
	void ProcessInput(GLFWwindow* window);

private:
	//Cube Rendering Variables
	unsigned int VAO;

	//Quad Rendering Variables
	unsigned int ppVAO;
	std::string ppVertexShaderPath;
	std::string ppFragmentShaderPath;
	std::shared_ptr<Shader> quadShader;

	//Framebuffer Variables
	unsigned int msFBO;
	unsigned int interimFBO;
	unsigned int screenTexture;

	//Input Variables
	unsigned int currentSamplesNum;
	unsigned int prevSamplesNum;
};