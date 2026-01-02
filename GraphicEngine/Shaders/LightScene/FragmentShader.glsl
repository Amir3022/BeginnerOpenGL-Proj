#version 330 core

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct Light
{
	vec3 sourcePos;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 outNormal;
in vec2 TexCoord;

uniform vec3 lightSourcePos;
uniform vec3 cameraPos;
uniform Material material;
uniform Light light;

void main()
{
	// Calculating Ambient light
	vec3 ambientColor = light.ambient * vec3(texture(material.diffuse, TexCoord));

	//Calculating Diffuse
	vec3 norm = normalize(outNormal);
	vec3 lightDir = normalize(light.sourcePos - FragPos);
	float diffuse = max(dot(norm, lightDir), 0.0f);
	vec3 diffuseColor = light.diffuse * diffuse * vec3(texture(material.diffuse, TexCoord));

	//Calculating Specular
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectedLightDir = normalize(reflect(-lightDir, norm));
	float specular = pow(max(dot(reflectedLightDir, viewDir), 0.0f), material.shininess);
	vec3 specularColor = light.specular * (specular * vec3(texture(material.specular, TexCoord)));

	//Combining Ambient, Diffuse, Specular for complete Phong Shading Model
	vec3 combined = ambientColor + diffuseColor + specularColor;
	FragColor = vec4(combined, 1.0f);
}
