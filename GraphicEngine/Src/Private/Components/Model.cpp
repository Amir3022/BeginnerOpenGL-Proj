#include "Components/Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


Model::Model(const std::string& path)
{
	LoadModel(path);
}

void Model::LoadModel(const std::string& path)
{

}

void Model::Draw(std::weak_ptr<Shader> shaderRef)
{
	//Make sure the weak ptr is pointing to valid reference
	if (std::shared_ptr<Shader> shader = shaderRef.lock())
	{
		//Iterate through all meshes and call their Draw function
		for (Mesh& mesh : meshes)
		{
			mesh.Draw(shaderRef);
		}
	}
}