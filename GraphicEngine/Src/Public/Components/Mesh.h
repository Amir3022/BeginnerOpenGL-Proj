#pragma once
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "Utilities/EngineTypes.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Texture
{
	unsigned int texIndex;
	TextureType texType;
};

class Mesh
{

};