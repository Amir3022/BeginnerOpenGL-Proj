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

private:
	void LoadModel(const std::string& path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, ETextureType typeEnum);

private:
	std::vector<Mesh> meshes;
	std::string Directory;
	std::vector<Texture> loadedTextures;
};