#version 440

uniform sampler2D FireFlies;

in vec2 texCoord;

layout(location=0) out vec4 FragColor;


void main()
{
    vec3 color = texture(FireFlies, texCoord).rgb;
    float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
    FragColor = vec4(luminance, 0.0, 0.0, 1.0);
}
