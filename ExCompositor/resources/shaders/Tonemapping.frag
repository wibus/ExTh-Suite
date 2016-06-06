#version 440

uniform sampler2D SourceImage;
uniform float MidGray;

in vec2 texCoord;

layout(location=0) out vec4 FragColor;


vec3 AcesFilm(in vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return min((x*(a*x+b))/(x*(c*x+d)+e), 1.0);
}

vec3 Reinhard(in vec3 x)
{
    vec3 t = x / (1.0 + x);
    return pow(t, vec3(1 / 1.8));
}


void main()
{
    vec3 color = texture(SourceImage, texCoord).rgb;

    vec3 linear = pow(color, vec3(2.2));
    vec3 exposure = color / (MidGray * 2.0);
    vec3 film = AcesFilm(exposure);
    vec3 gamma = pow(film, vec3(1 / 2.2));

    FragColor = vec4(color, 1.0);
}
