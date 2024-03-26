#version 330 core

in vec2 TexCoords;
out vec4 fragColor;

uniform sampler2D spriteTexture;

void main()
{
    vec4 texColor = texture(spriteTexture, TexCoords);
    fragColor = texColor; // Use the texture color
}