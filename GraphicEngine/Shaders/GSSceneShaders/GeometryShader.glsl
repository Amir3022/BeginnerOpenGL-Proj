#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 5) out;

in S_COLOR
{
	vec4 color;
} gs_in[];


out vec4 fcolor;
	

void BuildHouse(vec4 position)
{
	//Draw a house using Triange Stip (5 Vertices needed)
	fcolor = gs_in[0].color;	//Setting the Color from the Vertex Shader as the generated vertices colors
	gl_Position = position + vec4(-0.1f, -0.1f, 0.0f, 0.0f);	//Bottom Left Point
	EmitVertex();

	gl_Position = position + vec4(0.1f, -0.1f, 0.0f, 0.0f);		//Bottom Right Point
	EmitVertex();

	gl_Position = position + vec4(-0.1f, 0.1f, 0.0f, 0.0f);		//Top Left Point
	EmitVertex();

	gl_Position = position + vec4(0.1f, 0.1f, 0.0f, 0.0f);		//Top Right Point
	EmitVertex();

	fcolor  = vec4(1.0f, 1.0f, 1.0f, 1.0f);	//Set the tip of the house color to always be white
	gl_Position = position + vec4(0.0f, 0.2f, 0.0f, 0.0f);		//House Roof Point
	EmitVertex();

	EndPrimitive();
}

void main()
{
	BuildHouse(gl_in[0].gl_Position);
}