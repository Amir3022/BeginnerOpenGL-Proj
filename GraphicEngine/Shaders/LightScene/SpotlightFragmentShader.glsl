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
	vec3 sourceVec; //SpotLight Origin
	vec3 sourceDir; //SpotLight Direction
	float innerRadiusCos;
	float outerRadiusCos;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;	//Constanst for the attenuation formula
	float linear;
	float quad;
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
	//Calculating light direction and attenuation if any
	vec3 lightDir;
	float attenuation;
	lightDir = normalize(vec3(light.sourceVec) - FragPos);
	float distanceToLight = length(vec3(light.sourceVec) - FragPos);
	attenuation = 1.0f / (light.constant + light.linear * distanceToLight + light.quad * (distanceToLight * distanceToLight));

	//Calculate spotlight area of influence
	float cosAngle = dot(-lightDir, normalize(light.sourceDir));

	//Calculate light fall off between inner and outer radi, use light intensity to influence diffuse and specular lights on the lit object
	float lightIntensity = clamp((cosAngle - light.outerRadiusCos) / (light.innerRadiusCos - light.outerRadiusCos), 0.0f, 1.0f);

	// Calculating Ambient light
	vec3 ambientColor = light.ambient * attenuation * vec3(texture(material.diffuse, TexCoord));

	//Calculating Diffuse
	vec3 norm = normalize(outNormal);

	float diffuse = max(dot(norm, lightDir), 0.0f) * lightIntensity;
	vec3 diffuseColor = light.diffuse * diffuse * attenuation * vec3(texture(material.diffuse, TexCoord));

	//Calculating Specular
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectedLightDir = normalize(reflect(-lightDir, norm));
	float specular = pow(max(dot(reflectedLightDir, viewDir), 0.0f), material.shininess) * lightIntensity;
	vec3 specularColor = light.specular * specular * attenuation * vec3(texture(material.specular, TexCoord));

	//Applying Emissive Colors
	vec3 emissiveColor = floor((vec3(1.0f) - vec3(texture(material.specular, TexCoord)))) * vec3(texture(material.emissive, TexCoord)) * material.emissiveAmount;

	//Combining Ambient, Diffuse, Specular for complete Phong Shading Model
	vec3 combined = ambientColor + diffuseColor + specularColor + emissiveColor;

	FragColor = vec4(combined, 1.0f);
}
