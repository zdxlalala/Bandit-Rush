#version 330

// Input attributes
in vec3 in_position;
in vec3 in_color;


// Passed to fragment shader
out vec2 texcoord;
out vec3 vcolor;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform mat3 translation;

void main()
{
	texcoord = in_position.xy;
    vcolor = in_color;
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
    pos.x = pos.x + translation[0].x;
    pos.y = pos.y + translation[1].y;
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}
