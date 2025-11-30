#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec4 ourColor;
out vec2 ourTexCoord;

uniform mat4 inTransform;

void main()
{
	gl_Position = inTransform * vec4(aPos , 1.0f);
	ourColor = vec4(aColor, 1.0f);
	ourTexCoord = aTexCoord;
}