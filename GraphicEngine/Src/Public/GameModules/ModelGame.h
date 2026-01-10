#pragma once
#include "Game.h"
#include "Components/Model.h"

class ModelGame : public Game
{
public:
	ModelGame(int in_width, int in_height);

	virtual bool Init() override;

	virtual void Terminate() override;

	virtual void UpdateGame(float deltaTime) override;

	virtual void DrawFrame() override;

private:
	//Model Variables
	std::shared_ptr<Model> model;

	//Light shader
	std::unique_ptr<Shader> lightShader;

	//Light Shaders
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
	std::vector<glm::vec3> pointLightColors;
	glm::vec3 spotLightColor;
	glm::vec3 dirLightColor;
};