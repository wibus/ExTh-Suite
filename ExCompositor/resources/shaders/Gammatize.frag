#version 440

uniform sampler2D Source;

in vec2 texCoord;

layout(location=0) out vec4 FragColor;


void main()
{
    vec3 color = texture(Source, texCoord).rgb;
    vec3 gamma = pow(color, vec3(1 / 2.0));
    FragColor = vec4(gamma, 1.0);
}
