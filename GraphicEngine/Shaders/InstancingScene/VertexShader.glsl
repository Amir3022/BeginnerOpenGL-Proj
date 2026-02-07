#version 330 core

layout(location = 0)in vec3 aPos;
layout(location = 1)in vec3 aColor;

out vec4 vColor;

uniform vec2 offsets[100];

void main()
{
	vColor = vec4(aColor, 1.0f);
	gl_Position = vec4(aPos.x + offsets[gl_InstanceID].x, aPos.y + offsets[gl_InstanceID].y, aPos.z, 1.0f);
}