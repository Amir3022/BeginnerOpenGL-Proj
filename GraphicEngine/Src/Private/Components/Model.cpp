#include "Components/Model.h"
#include "Utilities/EngineUtilities.h"

Model::Model(const std::string& path)
{
	LoadModel(path);

	//Set Initial Transform for the Model
	SetTransform(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
}

void Model::LoadModel(const std::string& path)
{
	//Create Assimp importer and try to Load the model into scene object
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "Assimp Error!: " << importer.GetErrorString() << std::endl;
		return;
	}
	//Initialize the Directory based on Model path
	Directory = path.substr(0, path.find_last_of('/'));
	//Process the Root node of the loaded Scene (it will recursively process all child nodes)
	ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	//Validate both node and scene aren't null ptrs
	if (node && scene)
	{
		//Iterate through all node meshes, process them and add to Meshes vector
		for (int i = 0; i < (int)scene->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[i];
			meshes.push_back(ProcessMesh(mesh, scene));
		}
		//Iterate through all child nodes, and process them
		for (int i = 0; i < (int)node->mNumChildren; i++)
		{
			aiNode* childNode = node->mChildren[i];
			ProcessNode(childNode, scene);
		}
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	//Validate both mesh and scene aren't null ptrs
	if (mesh && scene)
	{
		//Declate vertices, indices, textures vectors to use to generate the Mesh
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		//Process all vertices data into vertices vector
		for (int i = 0; i < (int)mesh->mNumVertices; i++)
		{
			Vertex vertex;
			//Process position data
			glm::vec3 vector;
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.position = vector;
			//Process normal data
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;
			//Process texture coordinate if any
			if (mesh->mTextureCoords)
			{
				glm::vec2 texCoords;
				texCoords.x = mesh->mTextureCoords[0][i].x;
				texCoords.y = mesh->mTextureCoords[0][i].y;
				vertex.texCoords = texCoords;
			}
			//Add the vertex to vector of vertices
			vertices.push_back(vertex);
		}

		//Process all Indices from face data into indices vector
		for (int i = 0; i < (int)mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (int j = 0; j < (int)face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		//Process Mesh material
		if (mesh->mMaterialIndex >= 0)
		{
			//Get material with index from scene materials
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			//Load textures from material and insert them in the textures array
			std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, ETextureType::diffuse);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType::aiTextureType_SPECULAR, ETextureType::specular);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		//return generated Mesh
		return Mesh(vertices, indices, textures);
	}
	return Mesh();
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, ETextureType typeEnum)
{
	//Validate Material ptr isn't nullptr
	if (mat)
	{
		std::vector<Texture> textures;
		for (int i = 0; i < mat->GetTextureCount(type); i++)
		{
			//Get Texture from Path defined by texture index and type
			Texture texture;
			aiString texPath;
			mat->GetTexture(type, i, &texPath);
			//Check if texture is already present in loaded Textures
			bool bSkip = false;
			for (int j = 0; j < loadedTextures.size(); j++)
			{
				if (strcmp(texPath.C_Str(), loadedTextures[j].path.c_str()) == 0)
				{
					bSkip = true;
					textures.push_back(loadedTextures[j]);
					break;
				}
			}
			if (!bSkip)
			{
				//Fill the texture struct data
				std::string textureLocalPath = Directory + '/' + texPath.C_Str();
				texture.texIndex = EngineUtilities::LoadImageIntoTexture((textureLocalPath).c_str());
				texture.texType = typeEnum;
				texture.path = texPath.C_Str();
				//Add the created texture to Textures vector and the Loaded textures vector
				textures.push_back(texture);
				loadedTextures.push_back(texture);
			}
		}
		return textures;
	}
	return std::vector<Texture>();
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

void Model::SetTransform(const glm::vec3& in_pos, const glm::vec3& in_rot, const glm::vec3& in_scale)
{
	//Setting internal transform values
	position = in_pos;
	rotation = in_rot;
	scale = in_scale;

	//Setting transform values for all meshes in Model
	for (Mesh& mesh : meshes)
	{
		mesh.SetTransform(in_pos, in_rot, in_scale);
	}
}