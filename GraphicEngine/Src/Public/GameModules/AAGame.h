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
	void DrawMainScene();
	void DrawPPScene();

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
};