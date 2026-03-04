#pragma once
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "Utilities/EngineTypes.h"
#include "Components/Shader.h"

struct TBNVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 bitangent;
	glm::vec2 texCoords;
};

class TBNMesh
{
public:
	TBNMesh()
	{
		bIsValid = false;
	}

	TBNMesh(const std::vector<TBNVertex>& in_vertices, const std::vector<unsigned int>& in_indices, const std::vector<Texture>& in_textures)
		:vertices(in_vertices), indices(in_indices), textures(in_textures)
	{
		SetupMesh();
		bIsValid = true;

		position = glm::vec3(0.0f);
		rotation = glm::vec3(0.0f);
		scale = glm::vec3(1.0f);
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

	unsigned int GetVAO() { return VAO; }
	std::vector<unsigned int> GetIndices() { return indices; }
	std::vector<Texture> GetTextures() { return textures; }

private:
	void SetupMesh();

private:
	//Main Mesh Data
	std::vector<TBNVertex> vertices;
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