#version 330 core

layout(location = 0) out vec4 PositionBuffer;
layout(location = 1) out vec4 NormalBuffer;
layout(location = 2) out vec4 AlbedoSpecBuffer;

#define NR_POINT_LIGHTS 8
//Object material struct
struct Material
{
	sampler2D texture_diffuse_1;
	sampler2D texture_diffuse_2;
	sampler2D texture_diffuse_3;
	sampler2D texture_specular_1;
	sampler2D texture_specular_2;
	sampler2D texture_specular_3;
	sampler2D texture_normal_1;
	sampler2D texture_emissive;
	float emissiveAmount;
	float shininess;
};

in VS_OUT
{
	vec3 outNormal;
	vec3 FragPos;
	vec2 TexCoord;
} fs_in;


uniform Material material;

void main()
{
	//Set the Fragment Frag World position in the position buffer
	PositionBuffer = vec4(fs_in.FragPos, 1.f);

	//Set the Normal Value interpolated from vertices in Normal buffer
	NormalBuffer = vec4(normalize(fs_in.outNormal), 1.0f);

	//Set the Diffuse texture value in the albedoSpec rgb values
	AlbedoSpecBuffer.rgb = texture(material.texture_diffuse_1, fs_in.TexCoord).rgb;

	//Set the Specular Value in the Alpha of the AlbedoSpecBuffer
	AlbedoSpecBuffer.a = texture(material.texture_specular_1, fs_in.TexCoord).r;
}
