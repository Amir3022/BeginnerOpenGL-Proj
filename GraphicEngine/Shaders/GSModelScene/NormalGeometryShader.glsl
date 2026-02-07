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

uniform float time;

vec3 CalculateNormal()
{
	vec3 Line_A = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);		
	vec3 Line_B = vec3(gl_in[0].gl_Position - gl_in[2].gl_Position);
	return normalize(cross(Line_A, Line_B));
}

vec4 MoveVertexPos(vec4 position, vec3 normal)
{
	vec4 newPosition = position + vec4(((sin(time) + 1) / 2.0f) * normal, 0.0f);
	return newPosition;
}

void main()
{
	vec3 usedNormal = CalculateNormal();
	outNormal = gs_in[0].vs_outNormal;
	FragPos = gs_in[0].vs_FragPos;
	TexCoord = gs_in[0].vs_TexCoord;
	gl_Position = MoveVertexPos(gl_in[0].gl_Position, usedNormal);
	EmitVertex();

	outNormal = gs_in[1].vs_outNormal;
	FragPos = gs_in[1].vs_FragPos;
	TexCoord = gs_in[1].vs_TexCoord;
	gl_Position = MoveVertexPos(gl_in[1].gl_Position, usedNormal);
	EmitVertex();

	outNormal = gs_in[2].vs_outNormal;
	FragPos = gs_in[2].vs_FragPos;
	TexCoord = gs_in[2].vs_TexCoord;
	gl_Position = MoveVertexPos(gl_in[2].gl_Position, usedNormal);
	EmitVertex();

	EndPrimitive();
}