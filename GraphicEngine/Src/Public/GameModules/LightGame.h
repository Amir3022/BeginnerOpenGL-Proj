#pragma once
#include "Game.h"

class LightGame : public Game
{
public:
	LightGame(int in_width, int in_height);

	virtual bool Init() override;

	virtual void Terminate() override;

	virtual void UpdateGame(float deltaTime) override;

	virtual void DrawFrame() override;

private:
	//Light shader
	std::unique_ptr<Shader> lightShader;

	//Vertex Array Objects for object and light
	unsigned int VAO;
	unsigned int lightVAO;
	unsigned int VBO;

	std::string lightVertexShaderPath;
	std::string lightFragmentShaderPath;

	//Light source variables
	std::vector<glm::vec3> lightCubesPos;
	std::vector<float> lightMoveDirs;
	std::vector<float> lightMoveSpeeds;
	bool moveLightSource;
	bool changeLightColor;
	glm::vec3 dirLightOrient;
	
	//Colors Variables
	glm::vec3 objectColor;
	std::vector<glm::vec3> pointLightColors;
	glm::vec3 spotLightColor;
	glm::vec3 dirLightColor;

	//Game Instance Specific variables
	std::vector<PosOrientPair> cubeTransforms;
};