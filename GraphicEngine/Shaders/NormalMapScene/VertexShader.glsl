#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aTexCoord;

out VS_OUT
{
	vec3 outNormal;
	vec3 FragPos;
	vec2 TexCoord;
	mat3 TBN;
} vs_out;

uniform mat3 normalModelMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vs_out.outNormal = normalize(normalModelMatrix * aNormal);
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0f));
	vs_out.TexCoord = aTexCoord;

	//Calculate the TBN matrix to transform normals from tangent to world space
	vec3 T = normalize(normalModelMatrix * aTangent);
	vec3 N = vs_out.outNormal;
	//Re-orthogonalize Tangent and Normal vectors using Gramm Schmidt process (Correct the tangent with projection of the normal vector on tangent direction
	T = normalize(T - dot(T, N) * N);
	//Calculate the Bitangent vector from crossing normal and tangent
	vec3 B = normalize(cross(N, T));
	
	vs_out.TBN = mat3(T, B, N);

	gl_Position = projection * view * model * vec4(aPos , 1.0f);
}