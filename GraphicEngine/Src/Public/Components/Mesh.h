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

		position = glm::vec3(0.0f);
		rotation = glm::vec3(0.0f);
		scale = glm::vec3(0.0f);
	}

	void Draw(std::weak_ptr<Shader> shaderRef);

	bool IsValid() { return bIsValid; }

	//Transform Methods
	void SetTransform(const glm::vec3& in_pos, const glm::vec3& in_rot = glm::vec3(0.0f), const glm::vec3& in_scale = glm::vec3(1.0f))
	{
		position = in_pos;
		rotation = in_rot;
		scale = in_scale;
	}

	glm::vec3 GetPosition() const { return position; }
	glm::vec3 GetRotation() const { return rotation; }
	glm::vec3 GetScale() const { return scale; }

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

	//Mesh Transform data
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};