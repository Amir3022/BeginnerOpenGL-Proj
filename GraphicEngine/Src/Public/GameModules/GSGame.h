#pragma once

#include "Game.h"

class GSGame : public Game
{
public:
	GSGame(int in_width, int in_height);

	virtual bool Init() override;

	virtual void Terminate() override;

	virtual void UpdateGame(float deltaTime) override;

	virtual void DrawFrame() override;

private:
	//Geometry shader path
	std::string geometryShaderPath;

	//Rendering Variables
	unsigned int VAO;
};