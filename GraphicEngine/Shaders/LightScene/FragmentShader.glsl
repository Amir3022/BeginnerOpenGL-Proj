#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 outNormal;

uniform vec3 lightSourcePos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform float ambient;

void main()
{
	vec3 norm = normalize(outNormal);
	vec3 lightDir = normalize(lightSourcePos - FragPos);
	float diffuse = max(dot(norm, lightDir), 0.0f);
	vec3 combined = (ambient + diffuse) * lightColor;
	FragColor = vec4(combined * objectColor, 1.0f);
}
