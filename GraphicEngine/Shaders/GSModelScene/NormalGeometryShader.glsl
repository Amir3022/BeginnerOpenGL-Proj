#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

#define MAGNITUDE 0.1f

uniform mat4 projection;

in VS_OUT
{
	vec4 vs_outNormal;
} gs_in[];

void GenerateLine(int index)
{
	gl_Position = gl_in[index].gl_Position;
	EmitVertex();

	gl_Position = gl_in[index].gl_Position + projection * gs_in[index].vs_outNormal * MAGNITUDE;
	EmitVertex();

	EndPrimitive();
}

void main()
{
	//Generate 3 Lines at each vertex of the triangle
	GenerateLine(0);
	GenerateLine(1);
	GenerateLine(2);
}