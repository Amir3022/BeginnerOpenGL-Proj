#pragma once
#include "Game.h"
#include "Components/Model.h"

class AsteroidsGame : public Game
{
public:
	AsteroidsGame(int in_width, int in_height);

	virtual bool Init() override;

	virtual void Terminate() override;

	virtual void UpdateGame(float deltaTime) override;

	virtual void DrawFrame() override;

protected:
	virtual void ProcessInput(GLFWwindow* window) override;

private:
	//Model Variables
	std::shared_ptr<Model> planetModel;
	std::shared_ptr<Model> asteroidModel;
};