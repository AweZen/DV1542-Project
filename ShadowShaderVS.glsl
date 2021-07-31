#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;

uniform mat4 world;
uniform mat4 LSM;
uniform int calcBumpNormal;

out vec2 TexCoordOut;

void main()
{
if(calcBumpNormal == 1)
{	mat4 mvp = LSM  * world;
    gl_Position =  mvp * vec4(position, 1.0);}
	else{
	mat4 mvp = LSM  * world;
    gl_Position =  mvp * vec4(position, 1.0);}
}