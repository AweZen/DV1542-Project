#version 440 core
 
// OUT //
layout(location = 0) out vec4 FinalFragColor;
layout(location = 1) in vec4 ShadowCoord;

// Uniforms //
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform int lightType;
uniform vec3 ambientLight;
uniform vec3 lightDirection;
uniform mat4 lightMatrix;
uniform vec3 pos;
uniform vec2 screenSize;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;

uniform sampler2D PositionMap;
uniform sampler2D NormalMap;
uniform sampler2D ColorMap;
uniform sampler2D ShadowMap;


vec4 CalcPointLight(vec3 WorldPos, vec3 Normal)
{
	// Calculate Attenuation //
	float constant		= 1.f;
	float linear		= 0.05f;
	float quadratic		= 0.05f;
	float dist			= length(lightPosition - WorldPos);
	float attenuation	= (1.0f / (constant + linear * dist + quadratic * (dist * dist)));

	// Calculate Diffuse //
	vec3 lightVector = normalize(lightPosition - WorldPos);
	float diffuseBrightness = max(dot(Normal,lightVector),ambientLight.x);
	vec4 diffuseLight = vec4( diffuseBrightness) * vec4(materialDiffuse, 0);
    vec4 specularLight = vec4(0, 0, 0, 0);

    if (diffuseBrightness > ambientLight.x) {
		// Calculate Specular //
		vec3 VertexToEyeVec = normalize(pos - WorldPos);
		vec3 reflectVec = reflect(lightVector, Normal);
		float specularFactor = clamp(dot(reflectVec, VertexToEyeVec), 0, 1);
		specularFactor = pow(specularFactor, materialSpecular.x);
		// The Specular Light //
		if(specularFactor > 0){
		 specularLight = vec4(specularFactor, specularFactor, specularFactor, 0);
		 }
	}
	return (diffuseLight + specularLight) * vec4(lightColor,0) * attenuation;
}

vec2 CalcTexCoord()
{
	 return (gl_FragCoord.xy / screenSize);
}

vec4 CalcSunLight(vec3 Position, vec3 Normal)
{
	vec4 lightSpacePosition = lightMatrix * vec4(Position, 1);
	vec3 uvz = (lightSpacePosition.xyz / lightSpacePosition.w) * 0.5 + 0.5;

	float currentDepth = uvz.z;

	float bias = max(0.05 * (1.0 - dot(normalize(Normal), normalize(lightDirection))), 0.005);  
	float shadow = 0.0;
	vec2 size = 1.0 / textureSize(ShadowMap, 0);

	for(int x = -1; x <= 1; ++x){
		for(int y = -1; y <= 1; ++y){
				float pcfDepth = texture(ShadowMap, uvz.xy + vec2(x, y) * size).r;
				shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
		}
	}

	shadow /= 9.0;
	if(currentDepth > 1.0){
	   shadow = 0.0;
	}

	// Calculate Diffuse //
	vec3 lightVector = normalize(lightDirection - Position);
    float diffuseBrightness = dot(normalize(Normal), normalize(lightDirection));
    vec4 diffuseLight = max(vec4(diffuseBrightness, diffuseBrightness, diffuseBrightness, 1.0f) * vec4(materialDiffuse,0),vec4(ambientLight,0));

    vec4 specularLight = vec4(0, 0, 0, 0);
 
    if (diffuseBrightness > ambientLight.x) {
        // Calculate Specular //
        vec3 VertexToEyeVec = normalize(pos - Position);
        vec3 reflectVec = reflect(-lightVector, Normal);
        float specularFactor = clamp(dot(reflectVec, VertexToEyeVec), 0, 1);
         specularFactor = materialSpecular.x * pow(specularFactor, 60);
        // The Specular Light //
        if(specularFactor > 0){
         specularLight = vec4(specularFactor, specularFactor, specularFactor, 0);
         }
    }
    return (diffuseLight + specularLight)  + (1 - shadow);
}

void main()
{
   	vec2 TexCoord = CalcTexCoord();
   	vec3 WorldPos = texture(PositionMap, TexCoord).xyz;
   	vec3 Color = texture(NormalMap, TexCoord).xyz;
	vec3 Normal = texture(ColorMap, TexCoord).xyz;
	if(lightType == 1){
		FinalFragColor = vec4(Color, 1.0)* CalcPointLight(WorldPos,Normal );
	}else{
		FinalFragColor = vec4(Color, 1.0) * CalcSunLight(WorldPos,Normal) * 0.4;
	}
}

