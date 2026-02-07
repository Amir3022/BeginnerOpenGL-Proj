#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT
{
	vec3 vs_outNormal;
	vec3 vs_FragPos;
	vec2 vs_TexCoord;
} gs_in[];

out vec3 outNormal;
out vec3 FragPos;
out vec2 TexCoord;

	
void main()
{
	outNormal = gs_in[0].vs_outNormal;
	FragPos = gs_in[0].vs_FragPos;
	TexCoord = gs_in[0].vs_TexCoord;
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	outNormal = gs_in[1].vs_outNormal;
	FragPos = gs_in[1].vs_FragPos;
	TexCoord = gs_in[1].vs_TexCoord;
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();

	outNormal = gs_in[2].vs_outNormal;
	FragPos = gs_in[2].vs_FragPos;
	TexCoord = gs_in[2].vs_TexCoord;
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();
}