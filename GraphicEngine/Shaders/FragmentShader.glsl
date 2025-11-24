#version 330 core

in vec4 ourColor;
in vec2 ourTexCoord;

out vec4 FragColor;

uniform sampler2D Texture1;
uniform sampler2D Texture2;

void main()
{
    FragColor = mix(texture(Texture1, ourTexCoord), texture(Texture2, vec2(1.0f - ourTexCoord.x, ourTexCoord.y)), 0.5f); 
}