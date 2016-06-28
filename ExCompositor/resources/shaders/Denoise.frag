#version 440

uniform sampler2D Source;
uniform float Threshold;

in vec2 texCoord;

layout(location=0) out vec4 FragColor;


void main()
{
    ivec2 coor = ivec2(gl_FragCoord.xy);
    vec3 color = texelFetch(Source, coor, 0).rgb;

    vec3 neighBors[] = vec3[8](
        texelFetch(Source, coor + ivec2(-1, -1), 0).rgb,
        texelFetch(Source, coor + ivec2( 0, -1), 0).rgb,
        texelFetch(Source, coor + ivec2( 1, -1), 0).rgb,
        texelFetch(Source, coor + ivec2( 1,  0), 0).rgb,
        texelFetch(Source, coor + ivec2( 1,  1), 0).rgb,
        texelFetch(Source, coor + ivec2( 0,  1), 0).rgb,
        texelFetch(Source, coor + ivec2(-1,  1), 0).rgb,
        texelFetch(Source, coor + ivec2(-1,  0), 0).rgb
    );

    bool isFireFly = true;
    bool firstFound = false;
    vec3 averageColor = vec3(0.0);
    float intensity = length(color);
    for(int i=0; i < 8; ++i)
    {
        if(length(neighBors[i]) < intensity - Threshold)
        {
            averageColor += neighBors[i];
        }
        else
        {
            if(firstFound)
            {
                isFireFly = false;
                break;
            }
            else
            {
                firstFound = true;
            }
        }
    }

    if(isFireFly)
    {
        color = averageColor / 8.0;
    }

    FragColor = vec4(color, 1.0);
}
