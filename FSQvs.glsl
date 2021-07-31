#version 430
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex;

out Vertex_Out {
	vec2 tex;
	}_out;

void main()
{
	gl_Position = vec4(position, 1);
	_out.tex = tex;
}