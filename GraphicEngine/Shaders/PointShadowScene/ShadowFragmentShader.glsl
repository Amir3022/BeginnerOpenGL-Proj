#version 330 core

in vec3 FragPos;

uniform vec3 pointLightPos;
uniform float far_Plane;

out vec4 FragColor;

void main()
{
	//Get the distance between fragment and light position
	float fragDistance = length(FragPos - pointLightPos);

	//Set the depth Value to be the distance from light source, normalized by far plane
	gl_FragDepth = fragDistance / far_Plane;
}

