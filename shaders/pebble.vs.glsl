#version 330

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texcoord;
layout (location = 2) in vec3 app_color;
layout (location = 3) in mat3 transform;

// Input attributes
//in vec3 in_color;
//in vec3 in_position;

out vec2 texcoord;
out vec3 fcolor;

// Application data
//uniform mat3 transform;
uniform mat3 projection;
uniform mat3 translation;

void main()
{
    texcoord = in_texcoord;
    fcolor = app_color;
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
    pos.x = pos.x + translation[0].x;
    pos.y = pos.y + translation[1].y;
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}
