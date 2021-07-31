#version 430

in Vertex_Out {
	vec2 tex;
	} _in;

layout(location = 0) out vec4 Color;

uniform sampler2D FSQTexture;

void main()
{
	vec3 result = texture(FSQTexture, _in.tex).rgb;
	Color = vec4(result, 1);
}