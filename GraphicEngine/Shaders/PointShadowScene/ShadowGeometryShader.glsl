#version 330 core

#define NUM_CUBEMAP_FACES 6

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

out vec3 FragPos;

uniform mat4 lightSpaceMat[NUM_CUBEMAP_FACES];

void main()
{
	for(int face = 0; face < NUM_CUBEMAP_FACES; face++)
	{
		gl_Layer = face;	//Setting which layer of the cubemap to render to, only works if a cubemap texture is bound to the framebuffer used
		for(int j = 0; j < 3; j++)
		{
			FragPos = vec3(gl_in[j].gl_Position);	//Set the FragPos which is later used in Fragment shader to get the depth value
			gl_Position = lightSpaceMat[face] * vec4(FragPos, 1.0f);
			EmitVertex();
		}
		EndPrimitive();
	}
}