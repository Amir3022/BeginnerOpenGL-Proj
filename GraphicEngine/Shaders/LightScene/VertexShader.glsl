#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 outColor;

uniform vec3 lightSourcePos;
uniform vec3 cameraPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform float ambient;
uniform float specularStrength;

uniform mat3 normalModelMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec3 Normal = normalModelMatrix * aNormal;
	vec3 FragPos = vec3(model * vec4(aPos, 1.0f));

	//Calculating Diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightSourcePos - FragPos);
	float diffuse = max(dot(norm, lightDir), 0.0f);

	//Calculating Specular
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectedLightDir = normalize(reflect(-lightDir, norm));
	float specular = specularStrength * pow(max(dot(reflectedLightDir, viewDir), 0.0f), 64); //64 is shininnes value

	//Combining Ambient, Diffuse, Specular for complete Phong Lighting Model
	vec3 combined = (ambient + diffuse + specular) * lightColor;
	outColor = combined * objectColor;

	gl_Position = projection * view * model * vec4(aPos , 1.0f);
}