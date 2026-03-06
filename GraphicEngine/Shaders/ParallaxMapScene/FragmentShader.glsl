#version 330 core

out vec4 FragColor;

in VS_OUT
{
	vec3 FragPos;
	vec2 TexCoord;
	vec3 tangentFragPos;
	vec3 tangentCameraPos;
	vec3 tangentPointLightPos;
} fs_in;

uniform bool bUseNormalMap;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;
uniform sampler2D displacementTexture;
uniform float displacementHeightScale;
uniform vec3 pointLightAmbient;
uniform vec3 pointLightDiffuse;
uniform vec3 pointLightSpecular;

vec2 ParallaxMapping(vec2 texCoord, vec3 viewDir);

void main()
{
	//Calculate the viewDir in tangent Space
	vec3 viewDir = normalize(fs_in.tangentCameraPos - fs_in.tangentFragPos);
	//Perform parallax mapping to get modified texture coordinates
	vec2 texCoord = ParallaxMapping(fs_in.TexCoord, viewDir);

	//Discard any fragment if the tex coord is out of range of [0, 1]
	if(texCoord.x < 0 || texCoord.x > 1 || texCoord.y < 0 || texCoord.y > 1)
		discard;

	vec3 norm = texture(normalTexture, texCoord).rgb;	//Get fragment normal in range [0, 1]
	norm = normalize(norm * 2.0f - 1.0f);	//Get normal in range [-1, 1] normalized

	// Calculating Ambient light
	vec3 ambientColor = pointLightAmbient;

	//Calculating Diffuse
	vec3 lightDir = normalize(fs_in.tangentPointLightPos - fs_in.tangentFragPos);
	float diffuse = max(dot(norm, lightDir), 0.0f);
	vec3 diffuseColor = pointLightDiffuse * diffuse;

	//Calculating Specular
	//Blinn specular is calculated using the angle between the norm dir and the halfway vector, which is the middle vector between the view dir and light dir
	vec3 halfwayVec = normalize(lightDir + viewDir); 
	float specular = pow(max(dot(norm, halfwayVec), 0.0f), 64.0f);	//Shininess should be from 2,4 times larger that phong specular
	vec3 specularColor = pointLightSpecular * specular;


	//Combining Ambient, Diffuse, Specular for complete Bling Phong Shading Model
	vec3 combinedColor = (ambientColor + diffuseColor + specularColor) * texture(diffuseTexture, texCoord).rgb;

	//Output the combined color
	FragColor = vec4(combinedColor, 1.0f);
}

vec2 ParallaxMapping(vec2 texCoord, vec3 viewDir)
{
	//Determine the number of layers to use for Steep Parallax Mapping
	int minLayers = 8;
	int maxLayers = 32;
	float numLayers = mix(minLayers, maxLayers, max(dot(viewDir, vec3(0.0f, 0.0f, 1.0f)), 0.0f));
	float layerDepth = 1.0f / numLayers;
	float currentLayerDepth = 0.0f;

	//Determine the amount of each move along the Vector P in the view direction
	vec2 P = (viewDir.xy / (viewDir.z + 0.0000001f)) * displacementHeightScale;
	vec2 texCoordinateDelta = P / numLayers;

	//Advance the current texture coordinate till the texCoord height is less than the currentLayerDepth
	vec2 currentTexCoord = texCoord;
	float currentTexCoordHeight = texture(displacementTexture, currentTexCoord).r;

	while(currentLayerDepth < currentTexCoordHeight)
	{	
		currentTexCoord -= texCoordinateDelta;
		currentTexCoordHeight = texture(displacementTexture, currentTexCoord).r;
		currentLayerDepth += layerDepth;
	}
	//Perform Parallex Occlusion mapping to get rid of stepping artifacts from steep parallax mapping
	vec2 prevTexCoord = currentTexCoord + texCoordinateDelta;
	float prevTexCoordHeight = texture(displacementTexture, prevTexCoord).r;

	//Get the difference between each tex Coord Height and it's layer depth
	float currentDifference = currentTexCoordHeight - currentLayerDepth;
	float prevDifference = (currentLayerDepth - layerDepth) - prevTexCoordHeight;

	//Get the interpolated tex coordinate value
	float alpha = currentDifference / (currentDifference + prevDifference);
	currentTexCoord = alpha * prevTexCoord + (1 - alpha) * currentTexCoord;

	return currentTexCoord;
}


