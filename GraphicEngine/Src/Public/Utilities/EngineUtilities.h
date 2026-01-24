#pragma once
#include <vector>
#include <string>

class EngineUtilities
{
public:
	static unsigned int LoadImageIntoTexture(const char* imagePath, bool bUseAlpha = false);

	static unsigned int LoadImagesIntoCubemap(const std::vector<std::string>& imagePaths);
};
