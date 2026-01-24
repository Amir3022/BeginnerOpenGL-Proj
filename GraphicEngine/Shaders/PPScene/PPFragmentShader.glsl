#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int ppMode;

//Kernal Functions
vec3 ApplySharpenKernel();
vec3 ApplyBlurKernel();
vec3 ApplyEdgeDetectionKernel();

vec3 ApplyKernel(const in float kernel[9], in float samplePrecision);

void main()
{
    vec3 col;
    switch(ppMode)
    {
        case 1:     //Default Mode
        {
            col = texture(screenTexture, TexCoords).rgb;
            break;
        }
        case 2:     //Inverted colors
        {
            col = vec3(1.0f) - texture(screenTexture, TexCoords).rgb;
            break;
        }
        case 3:     //Greyscale image
        {
            vec3 texCol = texture(screenTexture, TexCoords).rgb;
            float average = 0.2126f * texCol.r  + 0.7152f * texCol.g + 0.0722f * texCol.b;
            col = vec3(average);
            break;
        }
        case 4:     //Sharpen Kernel
        {
            col = ApplySharpenKernel();
            break;
        }
        case 5:     //Blur Kernel
        {
            col = ApplyBlurKernel();
            break;
        }
        case 6:     //Edge Detection kernel
        {
            col = ApplyEdgeDetectionKernel();
            break;
        }
        default:
        {
            col = texture(screenTexture, TexCoords).rgb;
            break;
        }
    }
    FragColor = vec4(col, 1.0);
} 


vec3 ApplySharpenKernel()
{
    float kernel[9] = float[]
    (
        -1, -1, -1,
        -1, 9, -1,
        -1, -1, -1
    );

    float offset = 1 / 600.0f;

    return ApplyKernel(kernel, offset);
}

vec3 ApplyBlurKernel()
{
    float kernel[9] = float[]
    (
        1 / 16.0f, 2 / 16.0f, 1 / 16.0f,
        2 / 16.0f, 4/ 16.0f, 2 / 16.0f,
        1 / 16.0, 2 / 16.0f, 1 / 16.0f
    );

    float offset = 1 / 300.0f;

    return ApplyKernel(kernel, offset);
}

vec3 ApplyEdgeDetectionKernel()
{
    float kernel[9] = float[]
    (
        1, 1, 1,
        1, -8, 1,
        1, 1, 1
    );

    float offset = 1 / 600.0f;

    return ApplyKernel(kernel, offset);
}

vec3 ApplyKernel(const in float kernel[9], in float samplePrecision)
{
     vec2 offsets[9] = vec2[]    //Declare the offsets array
    (
        vec2(-samplePrecision, samplePrecision),  //Top Left
        vec2(0.0f, samplePrecision),     //Top Center
        vec2(samplePrecision, samplePrecision),   //Top Right
        vec2(-samplePrecision, 0.0f),    //Center Left
        vec2(0.0f, 0.0f),       //Center Center
        vec2(samplePrecision, 0.0f),      //Center Right
        vec2(-samplePrecision, -samplePrecision),  //Bottom Left
        vec2(0.0f, -samplePrecision),    //Bottom Center
        vec2(samplePrecision, -samplePrecision)  //Bottom Right
    );

    //Get Color Sample at each offset
    vec3 colorSamples[9];
    for(int i = 0; i < 9; i++)
    {
        colorSamples[i] = texture(screenTexture, TexCoords + offsets[i]).rgb;
    }
    //Multiply each color sample by it's kernel and combine into final output
    vec3 outCol;
    for(int i = 0; i < 9; i++)
    {
        outCol += colorSamples[i] * kernel[i];
    }
    return outCol;
}


