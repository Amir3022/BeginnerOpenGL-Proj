#version 330 core

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform float ambientScaler;

void main()
{
	vec3 ambient = lightColor * ambientScaler;
	FragColor = vec4(ambient * objectColor, 1.0f);
}
