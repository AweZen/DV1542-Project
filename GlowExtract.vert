#version 440 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 tex;

out Vertex_Out {
	vec2 tex;
} _out;

uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;

void main()
{

	_out.tex = tex;

	gl_Position = proj * view * vec4(position, 1);
}
