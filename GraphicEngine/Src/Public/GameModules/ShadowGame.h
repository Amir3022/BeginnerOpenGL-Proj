#pragma once
#include "Game.h"
#include "Components/Mesh.h"

class ShadowGame : public Game
{
public:
	ShadowGame(int in_width, int in_height);

	virtual bool Init() override;

	virtual void Terminate() override;

	virtual void UpdateGame(float deltaTime) override;

	virtual void DrawFrame() override;

protected:
	void RegisterShadowMap();
	void DrawMainScene();
	void DrawShadowMapRepresentation();

private:
	//Meshes Variables
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<bool> bUseTiling;

	//Shadow Mapping Variables
	unsigned int shadowFBO;
	unsigned int shadowMap;
	std::shared_ptr<Shader> shadowSimpleShader;
	std::string shadowVertexShaderPath;
	std::string shadowFragmentShaderPath;
	int shadowMapWidth;
	int shadowMapHeight;

	//Shadow Map Representation Variables
	unsigned int ppVAO;
	std::shared_ptr<Shader> ppShader;
	std::string ppVertexShaderPath;
	std::string ppFragmentShaderPath;

	//Light Variables
	glm::vec3 dirLightDirection;
	glm::vec3 dirLightColor;
};
