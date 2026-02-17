#version 330 core

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

//Calculate Shadow for fragment based on shadow map
float CalculateShadowForFragment(vec3 lightDir, vec3 norm);

out vec4 FragColor;

in VS_OUT
{
	vec3 outNormal;
	vec3 FragPos;
	vec2 TexCoord;
	vec4 FragLightSpacePos;

} fs_in;

uniform bool bUseTiling;
uniform vec3 cameraPos;
uniform Material material;
uniform DirLight dirLight;
uniform sampler2D DirLightShadowMap;
uniform SpotLight spotLight;
uniform PointLight[NR_POINT_LIGHTS] pointLights;


void main()
{
	//Calculate normalized normal vector
	vec3 norm = normalize(fs_in.outNormal);

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
	combinedColor += floor((vec3(1.0f) - vec3(texture(material.texture_specular_1, fs_in.TexCoord)))) * vec3(texture(material.texture_emissive, fs_in.TexCoord)) * material.emissiveAmount;

	FragColor = vec4(combinedColor, 1.0f);
}


vec3 CalculateDirectionalLightEffect(vec3 norm, DirLight localDirLight)
{
	//Calculating light direction
	vec3 lightDir = normalize(-vec3(localDirLight.sourceDir));

	//Calculate the View Direction from camera to Fragment
	vec3 viewDir = normalize(cameraPos - fs_in.FragPos);

	//Use Light Calculation function to return the DirLight effect
	return PerformLightCalculations(norm, lightDir, viewDir, localDirLight.light, 1.0f, 1.0f);
}

vec3 CalculatePointLightEffect(vec3 norm, PointLight localPointLight)
{
	//Calculating light direction and attenuation
	vec3 lightDir = normalize(vec3(localPointLight.sourcePos) - fs_in.FragPos);
	float distanceToLight = length(vec3(localPointLight.sourcePos) - fs_in.FragPos);
	float attenuation = clamp(1.0f / (localPointLight.constant + localPointLight.linear * distanceToLight + localPointLight.quad * (distanceToLight * distanceToLight)), 0.0f, 1.0f);

	//Calculate the View Direction from camera to Fragment
	vec3 viewDir = normalize(cameraPos - fs_in.FragPos);

	//Use Light Calculation function to return the PointLight effect
	return PerformLightCalculations(norm, lightDir, viewDir, localPointLight.light, attenuation, 1.0f);
}

vec3 CalculateSpotLightEffect(vec3 norm, SpotLight localSpotLight)
{
	//Calculating light direction and attenuation
	vec3 lightDir = normalize(vec3(localSpotLight.sourcePos) - fs_in.FragPos);
	float distanceToLight = length(vec3(localSpotLight.sourcePos) - fs_in.FragPos);
	float attenuation = clamp(1.0f / (localSpotLight.constant + localSpotLight.linear * distanceToLight + localSpotLight.quad * (distanceToLight * distanceToLight)), 0.0f, 1.0f);

	//Calculate spotlight area of influence
	float cosAngle = dot(-lightDir, normalize(localSpotLight.sourceDir));

	//Calculate light fall off between inner and outer radi, use light intensity to influence diffuse and specular lights on the lit object
	float lightIntensity = clamp((cosAngle - localSpotLight.outerRadiusCos) / (localSpotLight.innerRadiusCos - localSpotLight.outerRadiusCos), 0.0f, 1.0f);

	//Calculate the View Direction from camera to Fragment
	vec3 viewDir = normalize(cameraPos - fs_in.FragPos);

	//Use Light Calculation function to return the spotLight effect
	return PerformLightCalculations(norm, lightDir, viewDir, localSpotLight.light, attenuation, lightIntensity);
}

vec3 PerformLightCalculations(vec3 norm, vec3 lightDir, vec3 viewDir, Light light, float attenuation, float intensity)
{
	//Try to have texture tile repeat 4 times
	vec2 newTexCoord = bUseTiling ? mod((fs_in.TexCoord * 4), 1.0f) : fs_in.TexCoord;

	// Calculating Ambient light
	vec3 ambientColor = light.ambient * attenuation * vec3(texture(material.texture_diffuse_1, newTexCoord));

	//Calculating Diffuse
	float diffuse = max(dot(norm, lightDir), 0.0f) * intensity;
	vec3 diffuseColor = light.diffuse * diffuse * attenuation * vec3(texture(material.texture_diffuse_1, newTexCoord));

	//Calculating Specular
	vec3 specularColor;
	//Blinn specular is calculated using the angle between the norm dir and the halfway vector, which is the middle vector between the view dir and light dir
	vec3 halfwayVec = (lightDir + viewDir) / length(lightDir + viewDir); 
	float specular = pow(max(dot(norm, halfwayVec), 0.0f), 32.0f) * intensity;	//Shininess should be from 2,4 times larger that phong specular
	specularColor = light.specular * specular * attenuation * vec3(texture(material.texture_specular_1, newTexCoord));

	//Apply shadow to both specular and diffuse output
	float shadow = CalculateShadowForFragment(lightDir, norm);
	vec3 combinedDiffSpec = (1 - shadow) * (diffuseColor + specularColor);

	//Combining Ambient, Diffuse, Specular for complete Bling Phong Shading Model
	return ambientColor + combinedDiffSpec;
}

float CalculateShadowForFragment(vec3 lightDir, vec3 norm)
{
	float shadow = 0.0f;
	//Calculate Bias based on the angle between the Light Source, and the normal of the fragment
	float bias = max(0.008f * (1.0 - dot(norm, lightDir)), 0.005f);

	//Divide the FragLightSpacePos by it's w component to transform to clip space coordinates ( map from [-w, w] to [-1, 1] )
	vec3 projCoord = fs_in.FragLightSpacePos.xyz / fs_in.FragLightSpacePos.w;

	//Add 1 and divide by to map it to [0, 1]
	projCoord = (projCoord + vec3(1.0f)) / 2.0f;

	//Get the current depth of the fragment in relation to the light source (z comp of the projCoord)
	float currentDepth = projCoord.z;

	//Use PCF (Percenatage Closer Filter) to improve jagged lines at shadow border
	vec2 texelSize = 1.0f / textureSize(DirLightShadowMap, 0);
	//Get the average from all surrounding 9 texels to the one we are currently testing against
	for(int y = -2; y < 3; y++)
	{
		for(int x = -2; x < 3; x++)
		{
			//Use the xy components of the projCoord to sample the ShadowMap, since it's a depth buffer texture, we only need to r component of the output
			float closestDepth = texture(DirLightShadowMap, projCoord.xy + vec2(x * texelSize.x, y * texelSize.y)).r;	//This gets the closest depth in the shadow buffer

			//if any projCoord has a z value of greater than 1 (outside the frustum of the light View), make it have a shadow of 0.0f
			if(projCoord.z > 1.0f)
				shadow += 0.0f;
			else
			{
				//Compare the CurrentDepth to Closest depth, if the current depth is larger that closest depth, fragment should be in shadow
				shadow += (currentDepth - bias > closestDepth) ? 1.0f : 0.0f;
			}
		}
	}

	//Get the average shadow values from all 9 texels
	shadow /= 25;

	return shadow;
}

