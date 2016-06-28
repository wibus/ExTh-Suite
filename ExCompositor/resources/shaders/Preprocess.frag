#version 440

uniform sampler2D Source;
uniform float Aberration;

in vec2 texCoord;

layout(location=0) out vec4 FragColor;


void main()
{
    vec3 color = texture(Source, texCoord).rgb;

    // ABERRATION makes local tonemapping hard in low luminosity area
    // This mimics real eye limitations, but also makes, for very low luma,
    // this process more robust numerically ;)

    float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
    float invLogLuma = log( 1.0 / (luma + Aberration) );
    FragColor = vec4(color, invLogLuma);
}
