#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D positionBuffer;
uniform sampler2D normalBuffer;
uniform sampler2D albedoSpecBuffer;

uniform int DrawMode;

void main()
{
    vec3 col;
    switch(DrawMode)
    {
        case 0:
        {   
            col = texture(positionBuffer, TexCoords).rgb;
            break;
        }
        case 1:
        {   
            col = texture(normalBuffer, TexCoords).rgb;
            break;
        }
        case 2:
        {   
            col = texture(albedoSpecBuffer, TexCoords).rgb;
            break;
        }
        case 3:
        {   
            col = vec3(texture(albedoSpecBuffer, TexCoords).a);
            break;
        }
    }

    FragColor = vec4(col, 1.0);
} 


