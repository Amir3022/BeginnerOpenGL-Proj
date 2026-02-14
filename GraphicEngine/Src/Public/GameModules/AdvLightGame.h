#pragma once
#include "Game.h"
#include "Components/Mesh.h"

class AdvLightGame : public Game
{
public:
	AdvLightGame(int in_width, int in_height);

	virtual bool Init() override;

	virtual void Terminate() override;

	virtual void UpdateGame(float deltaTime) override;

	virtual void DrawFrame() override;


protected:
	virtual void ProcessInput(GLFWwindow* window) override;

	void DrawMainScene();

private:
	//Plane model
	std::shared_ptr<Mesh> planeMesh;

	//Light Variables
	glm::vec3 pointLightPos;
	glm::vec3 pointLightColor;

	//Input Variables
	bool bUseBlinn;
	bool bSwitchLightModelWasPressed;
};