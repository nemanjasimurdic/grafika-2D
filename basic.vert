#version 330 core

layout(location = 0) in vec2 inPos;
out vec3 chCol;

uniform float aspectRatio;

void main()
{
    gl_Position = vec4(inPos.x / aspectRatio, inPos.y, 0.0, 1.0);
    chCol = vec3(1.0, 1.0, 0.0);
}
