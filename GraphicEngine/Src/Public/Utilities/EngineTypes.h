#pragma once
#include <string>

enum ETextureType : unsigned char
{
	diffuse = 0,
	specular,
	emissive,
	normal,
};

struct Texture
{
	unsigned int texIndex;
	ETextureType texType;
	std::string path;
};