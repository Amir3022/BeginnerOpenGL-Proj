#pragma once
#include "Game.h"

class LightGame : public Game
{
public:
	LightGame(int in_width, int in_height);

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
	
	//Colors Variables
	glm::vec3 objectColor;
	glm::vec3 lightColor;
};