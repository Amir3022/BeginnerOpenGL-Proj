#version 330 core

layout(points) in;
layout(points, max_vertices = 1) out;

void main()
{
	gl_Position = gl_in[0].gl_Position;	//GS passthrough, the input point vertex is outputed as it is
	EmitVertex();

	EndPrimitive();
}