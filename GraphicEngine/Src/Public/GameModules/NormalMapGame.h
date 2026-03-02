#pragma once
#include "Game.h"
#include "Components/Mesh.h"

class NormalMapGame : public Game
{
public:
	NormalMapGame(int in_width, int in_height);

	virtual bool Init() override;

	virtual void Terminate() override;

	virtual void UpdateGame(float deltaTime) override;

	virtual void DrawFrame() override;

protected:
	void DrawMainScene();

private:
	//Mesh Variables
	std::shared_ptr<Mesh> wallMesh;

	//Light Variables
	glm::vec3 pointLightPos;
	glm::vec3 pointLightColor;

	//Rendering Light Variables
	std::shared_ptr<Shader> lightShader;
	std::shared_ptr<Mesh> lightCubeMesh;
	std::string lightVertexShaderPath;
	std::string lightFragmentShaderPath;
};