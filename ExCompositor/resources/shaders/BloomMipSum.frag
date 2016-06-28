#version 440

uniform int LodCount;
uniform sampler2D Source;

in vec2 texCoord;

layout(location=0) out vec4 FragColor;

// Arithmetic weight sum of LODs is not based on physics at all.
// It simply gives great results aesthetically... and realistically.

void main()
{
    float weightSum = 0.0;
    vec3 mipsum = vec3(0.0);
    for(int i=0; i < LodCount; ++i)
    {
        vec3 color = textureLod(Source, texCoord, i).rgb;
        float weight = 1.0 / (i + 1);
        mipsum += color * weight;
        weightSum += weight;
    }

    mipsum = mipsum / weightSum;

    FragColor = vec4(mipsum, 1.0);
}
