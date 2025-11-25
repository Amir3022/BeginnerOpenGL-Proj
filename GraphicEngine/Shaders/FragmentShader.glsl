#version 330 core

in vec4 ourColor;
in vec2 ourTexCoord;

out vec4 FragColor;

uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform float mixAlpha;

void main()
{
    FragColor = mix(texture(Texture1, ourTexCoord), texture(Texture2, ourTexCoord), mixAlpha); 
}