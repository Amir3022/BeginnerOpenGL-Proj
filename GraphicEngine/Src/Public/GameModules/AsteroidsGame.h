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
	void GenerateAsteroidsTransforms(int amount);
	void UpdateAsteroidsPositions();

private:
	//Model Variables
	std::shared_ptr<Model> planetModel;
	std::shared_ptr<Model> asteroidModel;

	//Asteroids Transforms
	std::vector<glm::mat4> asteroidsTransforms;
	int fixedSeed;		//Fixed seed to ensure same random values each run

	//Instancing Shader Variables
	std::shared_ptr<Shader> instancedShader;
	std::string instancedVertexShaderPath;

	//Instanced Array Buffer Index
	unsigned int IBO;

	//Cubemap Shader Variables
	std::shared_ptr<Shader> cubemapShader;
	std::string cubemapVertexShaderPath;
	std::string cubemapFragmentShaderPath;

	//Cubemap Arrays variables
	unsigned int cubemapVAO;
	unsigned int cubemapTexture;
};