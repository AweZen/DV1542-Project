#version 430
layout (triangles) in;
layout (triangle_strip, max_vertices = 6) out;

in Vertex_Out {
	vec3 position;
	vec2 tex;
	vec3 normal;
	vec3 tangent;
} _in[];


out Geometry_Out {
	vec3 position;
	vec2 tex;
	vec3 normal;
	vec3 tangent;

} _out;

uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 pos;

void main()
{
	mat4 pvw = proj * view * world;
	vec3 v1 = vec3(world * vec4(_in[0].position,1)) - vec3(world * vec4(_in[1].position,1));
	vec3 v2 = vec3(world * vec4(_in[0].position,1)) - vec3(world * vec4(_in[2].position,1));
	_out.normal = normalize(cross(v1,v2));

	if(dot(normalize((vec3(vec4(pos,1)) -  vec3(world *vec4(_in[0].position,1)))), _out.normal) > 0 ) {
	for(int i = 0; i < 3; i++)
	{
		gl_Position = pvw * vec4(_in[i].position , 1);
		_out.position = vec3(world * vec4(_in[i].position,1));
		_out.tex = _in[i].tex;
		_out.tangent = _in[i].tangent;
		EmitVertex();
	}
	EndPrimitive();
	}
} 
