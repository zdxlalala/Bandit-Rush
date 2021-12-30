#version 330

// From Vertex Shader
in vec3 vcolor;

// Application data
uniform vec3 fcolor;

// Output color
layout(location = 0) out vec4 color;

void main()
{
	color = vec4(vcolor*fcolor, 1.0);
}
