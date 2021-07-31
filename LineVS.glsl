#version 430

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 rgb;


out VO {
	vec3 position;
	vec3 rgb;
} _out;

uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	mat4 mvp = proj * view * world;

	gl_Position = mvp * vec4(pos, 1);
	_out.position = vec3(world * vec4(pos, 1.0));
	_out.rgb = rgb;

}