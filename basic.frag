#version 330 core

in vec3 chCol;
out vec4 outCol;

uniform vec3 sunColor; 
uniform vec3 cloudColor; 
uniform float cloudOpacity; 
uniform bool isCloud;

void main()
{
    if (isCloud) {
        outCol = vec4(cloudColor, cloudOpacity);
    } else {
        outCol = vec4(sunColor, 1.0); 
    }
}
