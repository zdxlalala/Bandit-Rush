#version 330
#define ITERATIONS 128
#define RADIUS .3

// From vertex shader
in vec2 texcoord;
in vec3 vcolor;

// Application data
uniform vec3 fcolor;


// Output color
layout(location = 0) out  vec4 color;

void main()
{
    float radius = length(texcoord);
    color = vec4(fcolor*vcolor, 0.7*(1 - radius));
}
