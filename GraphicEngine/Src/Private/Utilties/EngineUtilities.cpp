#include "Utilities/EngineUtilities.h"
#include "glad/glad.h"
#include "Utilities/stb_image.h"
#include <iostream>

unsigned int EngineUtilities::LoadImageIntoTexture(const char* imagePath, bool bUseAlpha)
{
    unsigned int outTexture = -1;
    //Load Texture Data from image path
    int width, height, nChannels;
    stbi_set_flip_vertically_on_load(true); //To match images and OpenGL texture y orientation
    unsigned char* data = stbi_load(imagePath, &width, &height, &nChannels, 0);
    if (data)
    {
        //Get the Data format based on image number on channels
        GLenum dataFormat = GL_RED;
        switch (nChannels)
        {
        case 1:
        {
            dataFormat = GL_RED;
            break;
        }
        case 3:
        {
            dataFormat = GL_RGB;
            break;
        }
        case 4:
        {
            dataFormat = GL_RGBA;
            break;
        }
        }

        //Generate Texture Object
        glGenTextures(1, &outTexture);

        //Bind Texture to texture object
        glBindTexture(GL_TEXTURE_2D, outTexture);

        //Set Texture Parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, bUseAlpha ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, bUseAlpha ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //Load image data to Texture object
        glTexImage2D(GL_TEXTURE_2D, 0, bUseAlpha ? GL_RGBA : GL_RGB, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture with path: " << imagePath << std::endl;
    }
    //Set the Default Texture as active
    glActiveTexture(0);
    //Free loaded Image
    stbi_image_free(data);

    return outTexture;
}