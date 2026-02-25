#pragma once
#include "Game.h"
#include "Components/Mesh.h"
#include "Components/Model.h"

class PointShadowGame : public Game
{
public:
	PointShadowGame(int in_width, int in_height);

	virtual bool Init() override;

	virtual void Terminate() override;

	virtual void UpdateGame(float deltaTime) override;

	virtual void DrawFrame() override;

protected:
	void RegisterShadowMap();
	void DrawMainScene();
	void DrawPointShadowCubemap();

private:
	//Meshes Variables
	std::vector<std::shared_ptr<Mesh>> cubeMeshes;
	std::shared_ptr<Model> model;
	std::vector<std::shared_ptr<Mesh>> wallMeshes;
	bool bRenderModel;

	//Shadow Mapping Variables
	unsigned int shadowFBO;
	unsigned int shadowMap;
	std::shared_ptr<Shader> pointShadowShader;
	std::string shadowVertexShaderPath;
	std::string shadowGeometryShaderPath;
	std::string shadowFragmentShaderPath;
	int shadowMapWidth;
	int shadowMapHeight;
	std::vector<glm::mat4> lightSpaceTransformMats;		//Light Space transform matrix for each face of the point shadow map
	float farPlaneDistance;

	//Light Variables
	glm::vec3 pointLightPos;
	glm::vec3 pointLightColor;

	//Point Shadow CubeMap variables
	unsigned int cubemapVAO;
	std::shared_ptr<Shader> cubemapShader;
	std::string cubemapFragmentShaderPath;
	std::string cubemapVertexShaderPath;

	//Rendering Light Variables
	std::shared_ptr<Shader> lightShader;
	std::shared_ptr<Mesh> lightCubeMesh;
	std::string lightVertexShaderPath;
	std::string lightFragmentShaderPath;
};