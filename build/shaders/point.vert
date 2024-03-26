#version 330 core

layout (location = 0) in vec3 position;
out vec2 TexCoords;

uniform mat4 viewProj;
uniform mat4 model;

void main()
{
    gl_Position = viewProj * model * vec4(position, 1.0);
    gl_PointSize = 25.0; // Adjust the size of points
    TexCoords = gl_PointCoord; // Pass texture coordinates to the fragment shader
}