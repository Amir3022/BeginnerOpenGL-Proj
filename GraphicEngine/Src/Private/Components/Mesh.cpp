#include "Components/Mesh.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

void Mesh::SetupMesh()
{
	//Generate VAO
	glGenVertexArrays(1, &VAO);
	//Bind the VAO to be the one used by the VBO, EBO
	glBindVertexArray(VAO);

	//Generate VBO, EBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	//Add Vertices Data to Vertex Buffer
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	//Add Indices Data to Elements Buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	//Set Vertex Attribute Data
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	//Unbind the mesh VAO
	glBindVertexArray(0);
}

void Mesh::Draw(std::weak_ptr<Shader> shaderRef)
{
	//Check if the Shader ptr is still valid
	if (std::shared_ptr<Shader> shader = shaderRef.lock())
	{
		//Bind the VAO that contains all vertices Data
		glBindVertexArray(VAO);

		unsigned int nr_diffuse = 1;
		unsigned int nr_specular = 1;
		//Cycle through all Textures, and set their Data in Fragment Shader
		for (int i = 0; i < textures.size(); i++)
		{
			Texture texture = textures[i];
			//Set Active Texture Unit
			glActiveTexture(GL_TEXTURE0 + i);
			//Bind the Texture to the Active Texture Unit
			glBindTexture(GL_TEXTURE_2D, texture.texIndex);
			//Get the correct texture name to set in the Data in the Fragment Shader
			std::string textureName = "material.texture_";
			if (texture.texType == ETextureType::diffuse)
				textureName += "diffuse_" + std::to_string(nr_diffuse++);
			else if (texture.texType == ETextureType::specular)
				textureName += "specular_" + std::to_string(nr_specular++);
			shader->SetInt(textureName, i);
		}
		//Deactivate the last used Texture
		glActiveTexture(0);

		//Draw mesh Triangles using all bound Data
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);

		//Unbind the VAO
		glBindVertexArray(0);
	}
}
