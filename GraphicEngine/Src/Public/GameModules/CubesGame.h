#pragma once
#include "Game.h"

class CubesGame : public Game
{
public:
	CubesGame(int in_width, int in_height)
		: Game(in_width, in_height)
	{
		fragmentShaderPath = "Shaders/FragmentShader.glsl";
		vertexShaderPath = "Shaders/VertexShader.glsl";

		texturesMixAlpha = 0.5f;

		objectXRotation = 0.0f;
		objectYRotation = 0.0f;

		cubeTransforms.clear();
	}

	virtual bool Init() override;

	virtual void UpdateGame(float deltaTime) override;

	virtual void DrawFrame() override;

private:
	virtual void ProcessInput(GLFWwindow* window) override;

private:
	//Game Instance Specific variables
	std::vector<PosOrientPair> cubeTransforms;

	//Rendering mode and texture variables
	float texturesMixAlpha;

	//Cached Current cube rotation
	float objectXRotation;
	float objectYRotation;
};
