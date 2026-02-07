#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out VS_OUT
{
	vec4 vs_outNormal;
} vs_out;

uniform mat3 normalMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vs_out.vs_outNormal = vec4(normalize(normalMatrix * aNormal), 0.0f);	//Directions should have w = 0.0f
	gl_Position = projection * view * model * vec4(aPos , 1.0f);
}