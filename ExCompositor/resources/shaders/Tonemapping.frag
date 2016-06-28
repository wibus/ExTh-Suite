#version 440

uniform sampler2D Source;
uniform sampler2D MipmapSum;
uniform float ExposureGain;
uniform float BloomGain;

in vec2 texCoord;

layout(location=0) out vec4 FragColor;


void main()
{
    vec3 base = texture(Source, texCoord).rgb;
    vec4 mipmap = texture(MipmapSum, texCoord);
    vec3 bloom = vec3(mipmap);
    float tone = mipmap.a;

    vec3 color = ((bloom * BloomGain + base) / tone) * ExposureGain;

    FragColor = vec4(color, 1.0);
}
