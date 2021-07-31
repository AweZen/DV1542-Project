#version 440 core

layout(location = 0) out vec4 Color;

uniform sampler2D DeferredTexture;
uniform sampler2D glow;

in Vertex_Out {
	vec2 tex;
	}_in;

void main()
{
	const float gamma = 1.5f;
	const float exposure = 1.5f;

	vec3 glowColor = texture(glow, _in.tex).rgb;
	vec3 DeferredColor = texture(DeferredTexture, _in.tex).rgb;
	vec3 result = vec3(1.0) - exp(-(DeferredColor + glowColor * 0.25f) * exposure);	
	result = pow(result, vec3(1.0 / gamma));

	Color = vec4(result, 1.0f);

}
