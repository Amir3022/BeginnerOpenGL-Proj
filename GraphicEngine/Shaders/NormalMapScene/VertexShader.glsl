#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out VS_OUT
{
	vec3 outNormal;
	vec3 FragPos;
	vec2 TexCoord;
} vs_out;

uniform mat3 normalModelMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vs_out.outNormal = normalModelMatrix * aNormal;
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0f));
	vs_out.TexCoord = aTexCoord;
	gl_Position = projection * view * model * vec4(aPos , 1.0f);
}


//void main()
//{
//	vec3 norm;
//	if(bUseNormalMap)
//	{	
//		norm = texture(normalTexture, fs_in.TexCoord).rgb;	//Get fragment normal in range [0, 1]
//		norm = normalize(norm * 2.0f - 1.0f);	//Get normal in range [-1, 1] normalized
//	}
//	else
//	{
//		norm = normalize(fs_in.outNormal);
//	}
//
//	vec3 color = texture(diffuseTexture, fs_in.TexCoord).rgb;
//	// ambient
//    vec3 ambient = pointLightAmbient;
//    // diffuse
//    vec3 lightDir = normalize(pointLightPos - fs_in.FragPos);
//    float diff = max(dot(lightDir, norm), 0.0);
//    vec3 diffuse = diff * pointLightDiffuse;
//    // specular
//    vec3 viewDir = normalize(cameraPos - fs_in.FragPos);
//    vec3 reflectDir = reflect(-lightDir, norm);
//    float spec = 0.0;
//    vec3 halfwayDir = normalize(lightDir + viewDir);  
//    spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
//    vec3 specular = spec * pointLightSpecular;   
//
//	//Combining Ambient, Diffuse, Specular for complete Bling Phong Shading Model
//	vec3 combinedColor = (ambient + diffuse + specular) * color;  
//
//	//Output the combined color
//	FragColor = vec4(combinedColor, 1.0f);
//}