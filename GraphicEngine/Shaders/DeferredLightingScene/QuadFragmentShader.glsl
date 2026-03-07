#version 330 core

#define NR_LIGHTS 32

struct Light
{
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D positionBuffer;
uniform sampler2D normalBuffer;
uniform sampler2D albedoSpecBuffer;

uniform vec3 cameraPos;

uniform Light pointLights[NR_LIGHTS];

void main()
{ 
    //Get Position, normal, Albedo, Specular information from gBuffer components
    vec3 FragPos = texture(positionBuffer, TexCoords).rgb;
    vec3 Norm = normalize(texture(normalBuffer, TexCoords).rgb);
    vec3 Color = texture(albedoSpecBuffer, TexCoords).rgb;
    float SpecularVal = texture(albedoSpecBuffer, TexCoords).a;

    vec3 viewDir = normalize(cameraPos - FragPos);

    //Calculate ambient once for all point light sources
    float ambient = 0.1f; //Hard coded ambient light value

    vec3 combinedColor = ambient * Color;

    //Calculate the effect of each Point Light source on the final fragment color
    for(int i = 0; i < NR_LIGHTS; i++)
    {
        //Calculate light attenuation using quadratic formula
        float distanceToLight = length(FragPos - pointLights[i].position);
        float attenuation = 1.0f / (pointLights[i].constant + pointLights[i].linear * distanceToLight + pointLights[i].quadratic * (distanceToLight * distanceToLight));

        //Calculate Diffuse
        vec3 lightDir = normalize(pointLights[i].position - FragPos);
        float diffuse = max(dot(Norm, lightDir), 0.0f) * attenuation;

        //Calculate Specular
        vec3 halfwayVec = (lightDir + viewDir) / length(lightDir + viewDir); 
        float specular = pow(max(dot(Norm, halfwayVec), 0.0f), 16.0f) * attenuation;

        //Add the Blinn Phong components to get the light effect on fragment
        combinedColor += diffuse * Color + specular * SpecularVal;
    }

    FragColor = vec4(combinedColor, 1.0);
} 


