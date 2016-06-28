#version 440

uniform int LodCount;
uniform sampler2D Source;
uniform float Relaxation;

in vec2 texCoord;

layout(location=0) out vec4 FragColor;

// Harmonic weight sum of color LODs is not based on physics at all.
// It simply gives great results aesthetically... and realistically.

// Inverse harmonic weight sum of luma LODs is not based on any physics.
// It tries to mimic the impression that our eyes are more sensible to
// the overall brightness of the scene rather than the local brighness.
// Mipmap level LodCount-1 is a single value representing the geometric
// mean of image luminance (usually used for global tonemapping).

void main()
{
    vec3 colorSum = vec3(0.0);
    float colorWeightSum = 0.0;

    float invLogLumaSum = 0.0;
    float invLogLumaWeightSum = 0.0;

    for(int i=0; i < LodCount; ++i)
    {
        vec4 texel = textureLod(Source, texCoord, i);

        vec3 color = vec3(texel);
        float colorWeight = 1.0 / (i + 1);
        colorSum += color * colorWeight;
        colorWeightSum += colorWeight;

        float invLogLuma = texel.a;
        float invLogLumaWeight = 1.0 / (LodCount - i);
        invLogLumaSum += invLogLuma * invLogLumaWeight;
        invLogLumaWeightSum += invLogLumaWeight;
    }


    vec3 color = colorSum / colorWeightSum;

    // Relaxation reprepsent the eye limitation
    // to do perfect local tonemapping
    float luma = exp(-invLogLumaSum / (invLogLumaWeightSum + Relaxation));

    FragColor = vec4(color, luma);
}
