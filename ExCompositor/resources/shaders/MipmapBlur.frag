#version 440

uniform int CurrentLod;
uniform sampler2D Source;

in vec2 texCoord;

layout(location=0) out vec4 FragColor;

const float[] kernel = float[](
    1, 4,  7,  4,  1,
    4, 16, 26, 16, 4,
    7, 26, 41, 26, 7,
    4, 16, 26, 16, 4,
    1, 4,  7,  4,  1
);

void main()
{
    vec4 blur = vec4(0.0);
    ivec2 coor = ivec2(gl_FragCoord.xy);

    blur += texelFetch(Source, coor + ivec2(-2, -2), CurrentLod) * kernel[0];
    blur += texelFetch(Source, coor + ivec2(-2, -1), CurrentLod) * kernel[1];
    blur += texelFetch(Source, coor + ivec2(-2,  0), CurrentLod) * kernel[2];
    blur += texelFetch(Source, coor + ivec2(-2,  1), CurrentLod) * kernel[3];
    blur += texelFetch(Source, coor + ivec2(-2,  2), CurrentLod) * kernel[4];

    blur += texelFetch(Source, coor + ivec2(-1, -2), CurrentLod) * kernel[5];
    blur += texelFetch(Source, coor + ivec2(-1, -1), CurrentLod) * kernel[6];
    blur += texelFetch(Source, coor + ivec2(-1,  0), CurrentLod) * kernel[7];
    blur += texelFetch(Source, coor + ivec2(-1,  1), CurrentLod) * kernel[8];
    blur += texelFetch(Source, coor + ivec2(-1,  2), CurrentLod) * kernel[9];

    blur += texelFetch(Source, coor + ivec2( 0, -2), CurrentLod) * kernel[10];
    blur += texelFetch(Source, coor + ivec2( 0, -1), CurrentLod) * kernel[11];
    blur += texelFetch(Source, coor + ivec2( 0,  0), CurrentLod) * kernel[12];
    blur += texelFetch(Source, coor + ivec2( 0,  1), CurrentLod) * kernel[13];
    blur += texelFetch(Source, coor + ivec2( 0,  2), CurrentLod) * kernel[14];

    blur += texelFetch(Source, coor + ivec2( 1, -2), CurrentLod) * kernel[15];
    blur += texelFetch(Source, coor + ivec2( 1, -1), CurrentLod) * kernel[16];
    blur += texelFetch(Source, coor + ivec2( 1,  0), CurrentLod) * kernel[17];
    blur += texelFetch(Source, coor + ivec2( 1,  1), CurrentLod) * kernel[18];
    blur += texelFetch(Source, coor + ivec2( 1,  2), CurrentLod) * kernel[19];

    blur += texelFetch(Source, coor + ivec2( 2, -2), CurrentLod) * kernel[20];
    blur += texelFetch(Source, coor + ivec2( 2, -1), CurrentLod) * kernel[21];
    blur += texelFetch(Source, coor + ivec2( 2,  0), CurrentLod) * kernel[22];
    blur += texelFetch(Source, coor + ivec2( 2,  1), CurrentLod) * kernel[23];
    blur += texelFetch(Source, coor + ivec2( 2,  2), CurrentLod) * kernel[24];

    FragColor = blur / 273.0;
}
