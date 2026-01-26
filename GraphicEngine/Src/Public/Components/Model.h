#pragma once
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{
public: 
	Model(const std::string& path);

	void Draw(std::weak_ptr<Shader> shader);

	//Transform Methods
	void SetTransform(const glm::vec3& in_pos = glm::vec3(0.0f), const glm::vec3& in_rot = glm::vec3(0.0f), const glm::vec3& in_scale = glm::vec3(1.0f));

	glm::vec3 GetPosition() const { return position; }
	glm::vec3 GetRotation() const { return rotation; }
	glm::vec3 GetScale() const { return scale; }

private:
	void LoadModel(const std::string& path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, ETextureType typeEnum);

private:
	std::vector<Mesh> meshes;
	std::string Directory;
	std::vector<Texture> loadedTextures;

	//Model Transform Data
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};