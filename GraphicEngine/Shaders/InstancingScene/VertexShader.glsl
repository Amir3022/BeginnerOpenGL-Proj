#version 330 core

layout(location = 0)in vec3 aPos;
layout(location = 1)in vec3 aColor;
layout(location = 2)in vec2 aOffset;

out vec4 vColor;

void main()
{
	vColor = vec4(aColor, 1.0f);
	vec2 scaledPos = vec2(aPos.x, aPos.y) * (gl_InstanceID / 100.0f);
	gl_Position = vec4(scaledPos.x + aOffset.x, scaledPos.y + aOffset.y, aPos.z, 1.0f);
}