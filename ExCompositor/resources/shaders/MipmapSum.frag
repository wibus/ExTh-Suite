#version 440

uniform int LodCount;
uniform sampler2D Mipmaps;

in vec2 texCoord;

layout(location=0) out vec4 FragColor;


void main()
{
    float mipsum = 0.0;
    float weightSum = 0.0;
    for(int i=0; i < LodCount; ++i)
    {
        float logLuma = textureLod(Mipmaps, texCoord, i).r;
        float weight = 1.0 / pow((LodCount - i), 2.0);
        mipsum += logLuma * weight;
        weightSum += weight;
    }

    mipsum = exp(-mipsum / weightSum);

    FragColor = vec4(mipsum, 0.0, 0.0, 1.0);
}
