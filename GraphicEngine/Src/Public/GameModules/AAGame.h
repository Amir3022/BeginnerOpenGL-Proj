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

protected:
	virtual void ProcessInput(GLFWwindow* window) override;

private:
	//Cube Rendering Variables
	unsigned int VAO;

	//Input Variables
	bool bWasChangeMSAAPressed;
	bool bUseMSAA;
};