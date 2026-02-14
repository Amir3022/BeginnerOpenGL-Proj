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
	void DrawMainScene();

private:
	//Meshes Variables
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<bool> bUseTiling;

	//Light Variables
	glm::vec3 dirLightDirection;
	glm::vec3 dirLightColor;
};
