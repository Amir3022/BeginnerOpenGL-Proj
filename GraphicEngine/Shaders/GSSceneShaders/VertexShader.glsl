#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out S_COLOR
{
	vec4 color;
} inColor;

void main()
{
	gl_Position = vec4(aPos.x, aPos.y, 0.0f , 1.0f);
	inColor.color = vec4(aColor, 1.0f);
}