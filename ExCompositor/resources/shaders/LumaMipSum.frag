#version 440

uniform int LodCount;
uniform sampler2D Source;
uniform float Relaxation;

in vec2 texCoord;

layout(location=0) out vec4 FragColor;

// Inverse arithmetic weight sum of LODs is not based on any physics.
// It tries to mimic the impression that our eyes are more sensible to
// the overall brightness of the scene rather than the local brighness.
// Mipmap level LodCount-1 is a single value representing the geometric
// mean of image luminance usually used for global tonemapping.

void main()
{
    float mipsum = 0.0;
    float weightSum = 0.0;
    for(int i=0; i < LodCount; ++i)
    {
        float invLogLuma = textureLod(Source, texCoord, i).a;
        float weight = 1.0 / (LodCount - i);
        mipsum += invLogLuma * weight;
        weightSum += weight;
    }

    // Relaxation reprepsent the eye limitation
    // to do perfect local tonemapping

    mipsum = exp(-mipsum / (weightSum + Relaxation));

    FragColor = vec4(mipsum, 0.0, 0.0, 1.0);
}
