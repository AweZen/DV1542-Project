#version 430
in Geometry_Out {
	vec3 position;
	vec2 tex;
	vec3 normal;
	vec3 tangent;
} _in;

layout(location = 0) out vec3 FragPosition;
layout(location = 1) out vec3 FragColor;
layout(location = 2) out vec3 FragNormal;
layout(location = 3) out vec3 FragTex;

uniform sampler2D DiffuseMap; 
uniform sampler2D NormalMapping;

uniform int calcBumpNormal;

vec3 CalcBumpedNormal()
{
    vec3 _normal = normalize(_in.normal);
    vec3 _tangent = normalize(_in.tangent);
    _tangent = normalize(_tangent - dot(_tangent, _normal) * _normal);
    vec3 _bitangent = cross(_tangent, _normal);
    vec3 bumpMapNormal = texture(NormalMapping, _in.tex).xyz;
    bumpMapNormal = 2.0 * bumpMapNormal - vec3(1.0, 1.0, 1.0);

    vec3 normalOut;
    mat3 TBN = mat3(_tangent, _bitangent, _normal);
    normalOut = TBN * bumpMapNormal;
    normalOut = normalize(normalOut);
    return normalOut;
}

void main() 
{ 
    FragPosition = vec3(_in.position);
	if(calcBumpNormal == 1){
	FragNormal	 = CalcBumpedNormal();
	FragColor	 = texture(DiffuseMap, _in.tex).xyz;}
	else{
	FragNormal	 = normalize(_in.normal); 
	FragColor	 = texture(DiffuseMap, _in.tex).xyz; }
    FragTex		 = vec3(_in.tex, 0.0); 
}