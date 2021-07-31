#version 430
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;


out Vertex_Out {
	vec3 position;
	vec2 tex;
	vec3 normal;
	vec3 tangent;
} _out;

void main()
{
	_out.position = position;
    _out.tex      = tex;
	_out.normal   = normalize(normal);
	_out.tangent  = normalize(tangent);
} 