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
uniform vec3 pointLightAmbient;
uniform vec3 pointLightDiffuse;
uniform vec3 pointLightSpecular;

void main()
{
	vec3 norm = texture(normalTexture, fs_in.TexCoord).rgb;	//Get fragment normal in range [0, 1]
	norm = normalize(norm * 2.0f - 1.0f);	//Get normal in range [-1, 1] normalized

	// Calculating Ambient light
	vec3 ambientColor = pointLightAmbient;

	//Calculating Diffuse
	vec3 lightDir = normalize(fs_in.tangentPointLightPos - fs_in.tangentFragPos);
	float diffuse = max(dot(norm, lightDir), 0.0f);
	vec3 diffuseColor = pointLightDiffuse * diffuse;

	//Calculating Specular
	vec3 viewDir = normalize(fs_in.tangentCameraPos - fs_in.tangentFragPos);
	//Blinn specular is calculated using the angle between the norm dir and the halfway vector, which is the middle vector between the view dir and light dir
	vec3 halfwayVec = normalize(lightDir + viewDir); 
	float specular = pow(max(dot(norm, halfwayVec), 0.0f), 64.0f);	//Shininess should be from 2,4 times larger that phong specular
	vec3 specularColor = pointLightSpecular * specular;


	//Combining Ambient, Diffuse, Specular for complete Bling Phong Shading Model
	vec3 combinedColor = (ambientColor + diffuseColor + specularColor) * texture(diffuseTexture, fs_in.TexCoord).rgb;

	//Output the combined color
	FragColor = vec4(combinedColor, 1.0f);
}


