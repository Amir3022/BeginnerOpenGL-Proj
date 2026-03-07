#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D colorTexture;

uniform bool Horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    //Declare result variable to accumulate the samples multiplied by the gaussian curve weight
    vec3 col = texture(colorTexture, TexCoords).rgb;
    vec3 result = col * weight[0];
    vec2 texelSize = textureSize(colorTexture, 0);
    //Get size of each texel to be used as offset
    vec2 texelOffset = vec2(1.0f) / texelSize;
    //Check whether averaging on the horizontal or vertical axis
    if(Horizontal)
    {
        for(int x = 1; x < 5; x++)
        {
            result += texture(colorTexture, TexCoords + texelOffset * vec2(x, 0.0f)).rgb * weight[x];
            result += texture(colorTexture, TexCoords - texelOffset * vec2(x, 0.0f)).rgb * weight[x];
        }
    }
    else
    {
        for(int y = 1; y < 5; y++)
        {
            result += texture(colorTexture, TexCoords + texelOffset * vec2(0.0f, y)).rgb * weight[y];
            result += texture(colorTexture, TexCoords - texelOffset * vec2(0.0f, y)).rgb * weight[y];
        }
    }
    
    FragColor = vec4(result, 1.0);
} 


