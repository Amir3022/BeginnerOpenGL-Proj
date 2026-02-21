#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aPos;
    vec4 resPos = projection * view * vec4(aPos , 1.0f);
    gl_Position = resPos.xyww; //the depth value for the cubemap will always be 1.0 so it's always behind anything rendered before it
} 
