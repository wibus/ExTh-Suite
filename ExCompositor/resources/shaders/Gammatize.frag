#version 440

uniform sampler2D Bloom;
uniform float ExposureGain;

in vec2 texCoord;

layout(location=0) out vec4 FragColor;


void main()
{
    vec3 color = texture(Bloom, texCoord).rgb;
    //color = pow(color, vec3(1 / 2.0));
    FragColor = vec4(color, 1.0);
}
