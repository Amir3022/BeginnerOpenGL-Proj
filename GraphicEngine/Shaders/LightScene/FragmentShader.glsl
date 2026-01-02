#version 330 core

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emissive;
	float emissiveAmount;
	float shininess;
};

struct Light
{
	vec4 sourceVec; //use a vec4 with the w component used to determine whether we are using point or directional light, 1.0f positional light, 0.0f directional light
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

	vec3 lightDir;
	if(light.sourceVec.w < 0.001f) //Directional light
		lightDir = normalize(-vec3(light.sourceVec));
	else if(light.sourceVec.w > 0.999f)	//Positional light
		lightDir = normalize(vec3(light.sourceVec) - FragPos);

	float diffuse = max(dot(norm, lightDir), 0.0f);
	vec3 diffuseColor = light.diffuse * diffuse * vec3(texture(material.diffuse, TexCoord));

	//Calculating Specular
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectedLightDir = normalize(reflect(-lightDir, norm));
	float specular = pow(max(dot(reflectedLightDir, viewDir), 0.0f), material.shininess);
	vec3 specularColor = light.specular * (specular * vec3(texture(material.specular, TexCoord)));

	//Applying Emissive Colors
	vec3 emissiveColor = floor((vec3(1.0f) - vec3(texture(material.specular, TexCoord)))) * vec3(texture(material.emissive, TexCoord)) * material.emissiveAmount;

	//Combining Ambient, Diffuse, Specular for complete Phong Shading Model
	vec3 combined = ambientColor + diffuseColor + specularColor + emissiveColor;
	FragColor = vec4(combined, 1.0f);
}
