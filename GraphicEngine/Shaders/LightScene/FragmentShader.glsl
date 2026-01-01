#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 outNormal;

uniform vec3 lightSourcePos;
uniform vec3 cameraPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform float ambient;
uniform float specularStrength;

void main()
{
	//Calculating Diffuse
	vec3 norm = normalize(outNormal);
	vec3 lightDir = normalize(lightSourcePos - FragPos);
	float diffuse = max(dot(norm, lightDir), 0.0f);

	//Calculating Specular
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectedLightDir = normalize(reflect(-lightDir, norm));
	float specular = specularStrength * pow(max(dot(reflectedLightDir, viewDir), 0.0f), 64); //64 is shininnes value

	//Combining Ambient, Diffuse, Specular for complete Phong Lighting Model
	vec3 combined = (ambient + diffuse + specular) * lightColor;
	FragColor = vec4(combined * objectColor, 1.0f);
}
