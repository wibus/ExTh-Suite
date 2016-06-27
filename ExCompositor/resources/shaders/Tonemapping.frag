#version 440

uniform sampler2D Source;
uniform sampler2D BaseBlur;
uniform sampler2D LumiBlur;
uniform float ExposureGain;

in vec2 texCoord;

layout(location=0) out vec4 FragColor;


void main()
{
    vec3 base = texture(Source, texCoord).rgb;

    vec3 baseBlur = texture(BaseBlur, texCoord).rgb;
    float baseLumi = dot(baseBlur, vec3(0.2126, 0.7152, 0.0722));
    vec3 bloom = baseBlur * max(log(baseLumi + 1.25), 0.0);

    float lumiBlur = texture(LumiBlur, texCoord).r;
    float tone = 0.32/lumiBlur;

    vec3 color = ((bloom + base) * tone) * ExposureGain;

    FragColor = vec4(color, 1.0);
}
