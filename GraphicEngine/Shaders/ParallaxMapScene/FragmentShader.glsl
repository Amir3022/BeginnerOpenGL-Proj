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
	float height = texture(displacementTexture, texCoord).r;	//Get the sample depth at original tex coordinate

	vec2 displacementOffset = (viewDir.xy / (viewDir.z + 0.00001f)) * (height * displacementHeightScale);	//divide veiwDir XY by it's z component to get better results at extreme viewing angles when z approaches 0.0f, to have a higher offset value

	return texCoord - displacementOffset;
}


