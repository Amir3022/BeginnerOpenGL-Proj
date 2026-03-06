#pragma once
#include "Game.h"
#include "Components/Mesh.h"

class ParallaxMapGame : public Game
{
public:
	ParallaxMapGame(int in_width, int in_height);

	virtual bool Init() override;

	virtual void Terminate() override;

	virtual void UpdateGame(float deltaTime) override;

	virtual void DrawFrame() override;

protected:
	void DrawMainScene();

	void ProcessInput(GLFWwindow* window);

private:
	//Rendering Variables
	unsigned int VAO;
	unsigned int diffuseTexture;
	unsigned int normalTexture;

	//Wall Transforms
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	//Light Variables
	glm::vec3 pointLightPos;
	glm::vec3 pointLightColor;

	//Rendering Light Variables
	std::shared_ptr<Shader> lightShader;
	std::shared_ptr<Mesh> lightCubeMesh;
	std::string lightVertexShaderPath;
	std::string lightFragmentShaderPath;

	//Input Variables
	bool bUseNormalMap;
	bool bUseNormalMapSwitchPressed;
};