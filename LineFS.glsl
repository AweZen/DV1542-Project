#version 430

in VO {
	vec3 position;
	vec3 rgb;
} _in;

out vec4 OutColor;

void main()
{
	OutColor = vec4((_in.rgb) , 0);
}