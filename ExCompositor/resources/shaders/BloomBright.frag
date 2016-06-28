#version 440

uniform sampler2D Source;

in vec2 texCoord;

layout(location=0) out vec4 FragColor;

// This pass may seems uncessary, since we could directly samples "Fireflies"
// mipmaps. But because I decided to use different resolutions for input and
// output frames, this pass makes sure mipmaps are aligned with the output's
// resolution.

void main()
{
    vec3 color = texture(Source, texCoord).rgb;
    FragColor = vec4(color, 1.0);
}
