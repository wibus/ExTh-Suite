#version 440

uniform sampler2D Source;
uniform sampler2D BloomBlur;
uniform sampler2D LumaBlur;
uniform float ExposureGain;
uniform float BloomGain;

in vec2 texCoord;

layout(location=0) out vec4 FragColor;


void main()
{
    vec3 base = texture(Source, texCoord).rgb;
    float tone = texture(LumaBlur, texCoord).r;
    vec3 bloom = texture(BloomBlur, texCoord).rgb;

    vec3 color = ((bloom * BloomGain + base) / tone) * ExposureGain;

    FragColor = vec4(color, 1.0);
}
