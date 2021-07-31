#version 440 core


layout(location = 0) out vec4 Glow;

uniform sampler2D DeferredTexture;

in Vertex_Out {
	vec2 tex;
	}_in;

void main()
{
	vec4 color = texture(DeferredTexture, _in.tex);
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness >= 0.1)
       Glow = vec4(color.rgb, 1.0);
    else
        Glow = vec4(vec3(0), 1.0);

}
