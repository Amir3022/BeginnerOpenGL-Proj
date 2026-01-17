#version 330 core

#define NR_POINT_LIGHTS 4
//Object material struct
struct Material
{
	sampler2D texture_diffuse_1;
	sampler2D texture_diffuse_2;
	sampler2D texture_diffuse_3;
	sampler2D texture_specular_1;
	sampler2D texture_specular_2;
	sampler2D texture_specular_3;
	sampler2D texture_emissive;
	float emissiveAmount;
	float shininess;
};

//Light sources Structs
struct Light
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	Light light;

	vec3 sourcePos;
	
	float constant;	//Constanst for the attenuation formula
	float linear;
	float quad;
};

struct DirLight
{
	Light light;

	vec3 sourceDir;
};

struct SpotLight
{
	Light light;

	vec3 sourcePos;
	vec3 sourceDir;
	float innerRadiusCos;
	float outerRadiusCos;

	float constant;	//Constanst for the attenuation formula
	float linear;
	float quad;
};

//Calculating effects of Different light casters
vec3 CalculateDirectionalLightEffect(vec3 norm, DirLight localDirLight);
vec3 CalculatePointLightEffect(vec3 norm, PointLight localPointLight);
vec3 CalculateSpotLightEffect(vec3 norm, SpotLight localSpotLight);
vec3 PerformLightCalculations(vec3 norm, vec3 lightDir, vec3 viewDir, Light light, float attenuation, float Intensity);

out vec4 FragColor;

in vec3 FragPos;
in vec3 outNormal;
in vec2 TexCoord;

uniform bool bLit;
uniform vec3 cameraPos;
uniform Material material;
uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform PointLight[NR_POINT_LIGHTS] pointLights;

void main()
{
	if(bLit)
	{
		//Calculate normalized normal vector
		vec3 norm = normalize(outNormal);

		vec3 combinedColor = vec3(0.0f);

		//Calculate Directional Light Effect on fragment
		combinedColor += CalculateDirectionalLightEffect(norm, dirLight);

		//Calculate each point Light effect on fragment
		for(int i = 0; i < NR_POINT_LIGHTS; i++)
		{
			combinedColor += CalculatePointLightEffect(norm, pointLights[i]);
		}

		//Calculate Spot Light effect on Fragment
		combinedColor += CalculateSpotLightEffect(norm, spotLight);

		//Add the Emissive color effect
		combinedColor += floor((vec3(1.0f) - vec3(texture(material.texture_specular_1, TexCoord)))) * vec3(texture(material.texture_emissive, TexCoord)) * material.emissiveAmount;

		FragColor = vec4(combinedColor, 1.0f);
	}
	else
	{
		FragColor = texture(material.texture_diffuse_1, TexCoord);
	}
}


vec3 CalculateDirectionalLightEffect(vec3 norm, DirLight localDirLight)
{
	//Calculating light direction
	vec3 lightDir = normalize(-vec3(localDirLight.sourceDir));

	//Calculate the View Direction from camera to Fragment
	vec3 viewDir = normalize(cameraPos - FragPos);

	//Use Light Calculation function to return the DirLight effect
	return PerformLightCalculations(norm, lightDir, viewDir, localDirLight.light, 1.0f, 1.0f);
}

vec3 CalculatePointLightEffect(vec3 norm, PointLight localPointLight)
{
	//Calculating light direction and attenuation
	vec3 lightDir = normalize(vec3(localPointLight.sourcePos) - FragPos);
	float distanceToLight = length(vec3(localPointLight.sourcePos) - FragPos);
	float attenuation = clamp(1.0f / (localPointLight.constant + localPointLight.linear * distanceToLight + localPointLight.quad * (distanceToLight * distanceToLight)), 0.0f, 1.0f);

	//Calculate the View Direction from camera to Fragment
	vec3 viewDir = normalize(cameraPos - FragPos);

	//Use Light Calculation function to return the PointLight effect
	return PerformLightCalculations(norm, lightDir, viewDir, localPointLight.light, attenuation, 1.0f);
}

vec3 CalculateSpotLightEffect(vec3 norm, SpotLight localSpotLight)
{
	//Calculating light direction and attenuation
	vec3 lightDir = normalize(vec3(localSpotLight.sourcePos) - FragPos);
	float distanceToLight = length(vec3(localSpotLight.sourcePos) - FragPos);
	float attenuation = clamp(1.0f / (localSpotLight.constant + localSpotLight.linear * distanceToLight + localSpotLight.quad * (distanceToLight * distanceToLight)), 0.0f, 1.0f);

	//Calculate spotlight area of influence
	float cosAngle = dot(-lightDir, normalize(localSpotLight.sourceDir));

	//Calculate light fall off between inner and outer radi, use light intensity to influence diffuse and specular lights on the lit object
	float lightIntensity = clamp((cosAngle - localSpotLight.outerRadiusCos) / (localSpotLight.innerRadiusCos - localSpotLight.outerRadiusCos), 0.0f, 1.0f);

	//Calculate the View Direction from camera to Fragment
	vec3 viewDir = normalize(cameraPos - FragPos);

	//Use Light Calculation function to return the spotLight effect
	return PerformLightCalculations(norm, lightDir, viewDir, localSpotLight.light, attenuation, lightIntensity);
}

vec3 PerformLightCalculations(vec3 norm, vec3 lightDir, vec3 viewDir, Light light, float attenuation, float intensity)
{
	// Calculating Ambient light
	vec3 ambientColor = light.ambient * attenuation * vec3(texture(material.texture_diffuse_1, TexCoord));

	//Calculating Diffuse
	float diffuse = max(dot(norm, lightDir), 0.0f) * intensity;
	vec3 diffuseColor = light.diffuse * diffuse * attenuation * vec3(texture(material.texture_diffuse_1, TexCoord));

	//Calculating Specular
	vec3 reflectedLightDir = normalize(reflect(-lightDir, norm));
	float specular = pow(max(dot(reflectedLightDir, viewDir), 0.0f), /*material.shininess*/32) * intensity;
	vec3 specularColor = light.specular * specular * attenuation * vec3(texture(material.texture_specular_1, TexCoord));

	//Combining Ambient, Diffuse, Specular for complete Phong Shading Model
	return ambientColor + diffuseColor + specularColor;
}

