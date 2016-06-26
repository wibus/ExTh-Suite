#version 440

uniform sampler2D FireFlies;
uniform sampler2D Luminance;
uniform float ExposureGain;

in vec2 texCoord;

layout(location=0) out vec4 FragColor;


void main()
{
    vec3 color = texture(FireFlies, texCoord).rgb;
    float lum = texture(Luminance, texCoord).r;
    color = color * ExposureGain;
    FragColor = vec4(color, 1.0);
}
