#pragma once
#include "Game.h"
#include "Components/TBNModel.h"

class DeferredLightingGame : public Game
{
public:
	DeferredLightingGame(int in_width, int in_height);

	virtual bool Init() override;

	virtual void Terminate() override;

	virtual void UpdateGame(float deltaTime) override;

	virtual void DrawFrame() override;

protected:
	virtual void ProcessInput(GLFWwindow* window) override;

	void DrawMainScene();

	void DrawDebugRenderQuad();

	void DrawDeferredLightingQuad();

private:
	//Model Variables
	std::shared_ptr<TBNModel> model;
	std::vector<glm::vec3> modelPositions;
	
	//Geometry Buffer Variables
	unsigned int gBuffer;
	unsigned int positionBuffer;
	unsigned int normalBuffer;
	unsigned int albedoSpecBuffer;

	//Render Quad variables
	unsigned int quadVAO;
	std::string debugVertexShaderPath;
	std::string debugFragmentShaderPath;
	std::string quadVertexShaderPath;
	std::string quadFragmentShaderPath;
	std::shared_ptr<Shader> debugQuadShader;
	std::shared_ptr<Shader> DLQuadShader;

	//Lighting Variables
	unsigned int NR_LIGHTS;
	std::vector<glm::vec3> pointLightPositions;
	std::vector<glm::vec3> pointLightColors;

	//Input Variables
	int DrawMode;
};