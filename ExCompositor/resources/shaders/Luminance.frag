#version 440

uniform sampler2D Source;

in vec2 texCoord;

layout(location=0) out vec4 FragColor;


void main()
{
    vec3 color = texture(Source, texCoord).rgb;

    float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
    float logLuma = log( 1.0 / (luma + 0.01) );
    FragColor = vec4(logLuma, 0.0, 0.0, 1.0);
}
