#pragma once
#include "Game.h"

class InstancingGame : public Game
{
public:
	InstancingGame(int in_width, int in_height);

	virtual bool Init() override;

	virtual void Terminate() override;

	virtual void UpdateGame(float deltaTime) override;

	virtual void DrawFrame() override;

private:
	//GL variables
	unsigned int VAO;

	//Instances Variables
	std::vector<glm::vec2> offsets;
};