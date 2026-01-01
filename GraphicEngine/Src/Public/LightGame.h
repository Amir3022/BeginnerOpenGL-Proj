#pragma once
#include "Game.h"

class LightGame : public Game
{
public:
	LightGame(int in_width, int in_height)
		: Game(in_width, in_height)
	{
		fragmentShaderPath = "Shaders/LightScene/FragmentShader.glsl";
		vertexShaderPath = "Shaders/LightScene/VertexShader.glsl";

		lightFragmentShaderPath = "Shaders/LightScene/LightFragmentShader.glsl";
		lightVertexShaderPath = "Shaders/LightScene/LightVertexShader.glsl";

		lightCubePos = glm::vec3(0.5f, 1.0f, 2.0f);
	}

	virtual bool Init() override;

	virtual void UpdateGame(float deltaTime) override;

	virtual void DrawFrame() override;

private:
	//Light shader
	std::unique_ptr<Shader> lightShader;

	//Vertex Array Objects for object and light
	unsigned int VAO;
	unsigned int lightVAO;

	std::string lightVertexShaderPath;
	std::string lightFragmentShaderPath;

	glm::vec3 lightCubePos;
	float lightMoveDir = 1;
};