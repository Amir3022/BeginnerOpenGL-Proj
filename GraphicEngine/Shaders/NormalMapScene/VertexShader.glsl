#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBiTangent;
layout (location = 4) in vec2 aTexCoord;

out VS_OUT
{
	vec3 FragPos;
	vec2 TexCoord;
	vec3 tangentFragPos;
	vec3 tangentCameraPos;
	vec3 tangentPointLightPos;
} vs_out;

uniform mat3 normalModelMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 cameraPos;
uniform vec3 pointLightPos;

void main()
{
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0f));
	vs_out.TexCoord = aTexCoord;

	//Creating TBN matrix to transform normal to World Space from Tangent Space
	vec3 N = normalize(normalModelMatrix * aNormal);
	vec3 B = normalize(normalModelMatrix * aBiTangent);
	vec3 T = normalize(normalModelMatrix * aTangent);
	mat3 TBN = mat3(T, B, N);

	//Transform Frag position, camera view position, point light position to tangent space using inverse TBN
	mat3 inverseTBN = transpose(TBN);	//Can get inverse using transpose since it's an orthognal matrix
	vs_out.tangentFragPos = inverseTBN * vs_out.FragPos;
	vs_out.tangentCameraPos = inverseTBN * cameraPos;
	vs_out.tangentPointLightPos = inverseTBN * pointLightPos;
	
	gl_Position = projection * view * model * vec4(aPos , 1.0f);
}