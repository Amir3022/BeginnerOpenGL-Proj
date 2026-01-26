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
vec4 CalculateDirectionalLightEffect(vec3 norm, DirLight localDirLight);
vec4 CalculatePointLightEffect(vec3 norm, PointLight localPointLight);
vec4 CalculateSpotLightEffect(vec3 norm, SpotLight localSpotLight);
vec4 PerformLightCalculations(vec3 norm, vec3 lightDir, vec3 viewDir, Light light, float attenuation, float Intensity);

out vec4 FragColor;

in vec3 FragPos;
in vec3 outNormal;
in vec2 TexCoord;

uniform bool bLit;
uniform int RenderingMode;
uniform vec3 cameraPos;
uniform Material material;
uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform PointLight[NR_POINT_LIGHTS] pointLights;
uniform samplerCube skyboxCubemap;

void main()
{
	switch(RenderingMode)
	{
	case 1:
	{
		if(bLit)
		{
			//Calculate normalized normal vector
			vec3 norm = normalize(outNormal);

			vec4 combinedColor = vec4(0.0f);

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
			combinedColor += floor((vec4(1.0f) - texture(material.texture_specular_1, TexCoord))) * texture(material.texture_emissive, TexCoord) * material.emissiveAmount;

			if(texture(material.texture_diffuse_1, TexCoord).a < 0.1f)
				discard;

			FragColor = combinedColor;
		}
		else
		{
			vec4 combinedColor = texture(material.texture_diffuse_1, TexCoord);
			if(combinedColor.a < 0.1f)
				discard;
			FragColor = combinedColor;
		}
		break;
	}
	case 2:
	{
		//Calculate normalized normal vector
		vec3 norm = normalize(outNormal);

		//Calculate the View Direction from Fragment to Camera
		vec3 viewDir = normalize(FragPos - cameraPos);

		//Calculate the reflected view Dir on the surface normalized
		vec3 reflected = normalize(reflect(viewDir, norm));

		//Use the reflected view dir to sample the cubemap
		vec4 col = vec4(texture(skyboxCubemap, reflected).rgb, 1.0f);
		FragColor = col;
		break;
	}
	case 3:
	{
		//Calculate normalized normal vector
		vec3 norm = normalize(outNormal);

		//Calculate the View Direction from Fragment to Camera
		vec3 viewDir = normalize(FragPos - cameraPos);

		//Calculate the Refracted view dir on the surface normalized
		vec3 refracted = normalize(refract(viewDir, norm, (1.0f / 1.52f)));	//Using refractive indices rations for when moving from air to glass

		//Use the Refracted view dir to sample the cubemap
		vec4 col = vec4(texture(skyboxCubemap, refracted).rgb, 1.0f);
		FragColor = col;
		break;
	}
	}
}


vec4 CalculateDirectionalLightEffect(vec3 norm, DirLight localDirLight)
{
	//Calculating light direction
	vec3 lightDir = normalize(-vec3(localDirLight.sourceDir));

	//Calculate the View Direction from camera to Fragment
	vec3 viewDir = normalize(cameraPos - FragPos);

	//Use Light Calculation function to return the DirLight effect
	return PerformLightCalculations(norm, lightDir, viewDir, localDirLight.light, 1.0f, 1.0f);
}

vec4 CalculatePointLightEffect(vec3 norm, PointLight localPointLight)
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

vec4 CalculateSpotLightEffect(vec3 norm, SpotLight localSpotLight)
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

vec4 PerformLightCalculations(vec3 norm, vec3 lightDir, vec3 viewDir, Light light, float attenuation, float intensity)
{
	// Calculating Ambient light
	vec4 ambientColor = vec4(light.ambient, 1.0f)  * attenuation * texture(material.texture_diffuse_1, TexCoord);

	//Calculating Diffuse
	float diffuse = max(dot(norm, lightDir), 0.0f) * intensity;
	vec4 diffuseColor = vec4(light.diffuse, 1.0f) * diffuse * attenuation * texture(material.texture_diffuse_1, TexCoord);

	//Calculating Specular
	vec3 reflectedLightDir = normalize(reflect(-lightDir, norm));
	float specular = pow(max(dot(reflectedLightDir, viewDir), 0.0f), /*material.shininess*/32) * intensity;
	vec4 specularColor = vec4(light.specular, 1.0f) * specular * attenuation * texture(material.texture_specular_1, TexCoord);

	//Combining Ambient, Diffuse, Specular for complete Phong Shading Model
	return ambientColor + diffuseColor + specularColor;
}

