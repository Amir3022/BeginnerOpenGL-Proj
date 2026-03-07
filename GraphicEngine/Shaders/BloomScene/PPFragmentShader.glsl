#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D colorTexture;
uniform sampler2D brightnessTexture;

uniform bool UseBloom;
uniform bool UseHDR;
uniform float exposure;

void main()
{
    vec3 col = texture(colorTexture, TexCoords).rgb;
    vec3 brightness = texture(brightnessTexture, TexCoords).rgb;

    if(UseBloom)
    {
        //Additive blend for both the color and the brightness texture
        col = col + brightness;
    }

    //Apply Tonemapping
    float gamma = 2.2f; //Used for gamma correction
    vec3 mapped;
    if(UseHDR)
    {
        mapped = vec3(1.0f) - exp(-col * exposure); //Apply tonemapping with exposure for HDR
    }
    else
    {
        mapped = col / (col + vec3(1.0f)); //Apply simple tonemapping algorithm
    }
    mapped = pow(mapped, vec3(1 / gamma));  //Apply gamma correction for sRGB textures
    
    FragColor = vec4(mapped, 1.0);
} 


