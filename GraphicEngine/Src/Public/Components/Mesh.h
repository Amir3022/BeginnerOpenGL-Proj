#pragma once
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "Utilities/EngineTypes.h"
#include "Components/Shader.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Texture
{
	unsigned int texIndex;
	ETextureType texType;
	std::string path;
};

class Mesh
{
public:
	Mesh()
	{
		bIsValid = false;
	}

	Mesh(const std::vector<Vertex>& in_vertices, const std::vector<unsigned int>& in_indices, const std::vector<Texture>& in_textures)
		:vertices(in_vertices), indices(in_indices), textures(in_textures)
	{
		SetupMesh();
		bIsValid = true;
	}

	void Draw(std::weak_ptr<Shader> shaderRef);

	bool IsValid() { return bIsValid; }

private:
	void SetupMesh();

private:
	//Main Mesh Data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	//GL Containers
	unsigned int VAO, VBO, EBO;

	//Validation Variables
	bool bIsValid;
};