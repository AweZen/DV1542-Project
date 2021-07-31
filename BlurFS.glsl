#version 440 core

layout(location = 0) in vec2 Tex;
layout(location = 0) out vec4 Blur;

uniform sampler2D textureColor;
uniform bool horizontalPass;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
	vec2 texOffset = 1.0 / textureSize(textureColor, 0);
	vec3 result = texture(textureColor, Tex).rgb  * weight[0];

	if (horizontalPass)
	{
		for(int i = 1; i < 5; ++i)
		{
			result += texture(textureColor, Tex + vec2(texOffset.x * i, 0.0)).rgb * weight[i];
			result += texture(textureColor, Tex - vec2(texOffset.x * i, 0.0)).rgb * weight[i];
		}
	}
	else
	{
		for(int i = 1; i < 5; ++i)
		{
			result += texture(textureColor, Tex + vec2(0.0, texOffset.y * i)).rgb * weight[i];
			result += texture(textureColor, Tex - vec2(0.0, texOffset.y * i)).rgb * weight[i];
		}
	}

	Blur = vec4(result, 1.0);
}
