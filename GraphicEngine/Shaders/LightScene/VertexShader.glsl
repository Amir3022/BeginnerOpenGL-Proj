#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 outNormal;
out vec3 FragPos;
out vec3 outLightSourcePos;

uniform vec3 lightSourcePos;
uniform mat3 normalModelMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	outNormal = normalModelMatrix * aNormal;
	outLightSourcePos = vec3(view * vec4(lightSourcePos, 1.0f));
	FragPos = vec3(model * view * vec4(aPos, 1.0f));
	gl_Position = projection * view * model * vec4(aPos , 1.0f);
}