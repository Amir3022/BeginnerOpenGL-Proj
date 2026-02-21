#version 330 core

out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube cubemapTexture;

void main()
{
    FragColor = vec4(texture(cubemapTexture, TexCoords).r);
} 



