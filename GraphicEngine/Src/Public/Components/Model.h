#pragma once
#include "Mesh.h"

class Model
{
public: 
	Model(const std::string& path);

	void Draw(std::weak_ptr<Shader> shader);

private:
	void LoadModel(const std::string& path);

private:
	std::vector<Mesh> meshes;
};