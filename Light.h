#pragma once
//INCLUDE GL//
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include <gl/GL.h>
#include "ShadowMap.h"

//INCLUDE C++//
#include <string>
#include <stdlib.h>


struct Attenuation {
	float _constant;
	float _linear;
	float _exp;
	Attenuation(float constant, float linear, float exp) {
		_constant = constant;
		_linear = linear;
		_exp = exp;
	}
	Attenuation() {
		_constant = 1;
		_linear = 1;
		_exp = 1;
	}
};

class Light
{
private:

public:
	Light(glm::vec3 color, float ambientIntensity, float diffuseIntensity, glm::vec3 direction, int width, int height, Shader *FSQShader);	//Konstuktor för directional light.
	Light(glm::vec3 color, float ambientIntensity, float diffuseIntensity, glm::vec3 position, Attenuation attenuation);	//Konstruktor för point light.
	~Light();
	void createShadowMap(ObjectDataHandler * objects[], PlanePoints far, PlanePoints near, float delta, float rotateAmount, int nrOfObjects);
	GLuint GetShadowMap();

	int _type;
	glm::vec3 _color;
	float _ambientIntensity;
	float _diffuseIntensity;
	ShadowMap *SMap;
	glm::vec3 _direction;
	glm::vec3 _position;
	Attenuation _attenuation;
};

Light::Light(glm::vec3 color, float ambientIntensity, float diffuseIntensity, glm::vec3 direction, int width, int height,Shader *FSQShader)
{
	_type = 0;
	_color = color;
	_ambientIntensity = ambientIntensity;
	_diffuseIntensity = diffuseIntensity;
	_direction = direction;
	SMap = new ShadowMap(width, height, direction, FSQShader);
}

Light::Light(glm::vec3 color, float ambientIntensity, float diffuseIntensity, glm::vec3 position, Attenuation attenuation)
{
	_type = 1;
	_color = color;
	_ambientIntensity = ambientIntensity;
	_diffuseIntensity = diffuseIntensity;
	_position = position;
	_attenuation = attenuation;
	SMap = new ShadowMap(0, 0,glm::vec3(0),NULL);
}

Light::~Light()
{
}

void Light::createShadowMap(ObjectDataHandler * objects[], PlanePoints far, PlanePoints near, float delta, float rotateAmount, int nrOfObjects)
{
	SMap->Render(objects, far, near, delta, rotateAmount, nrOfObjects);
}

GLuint Light::GetShadowMap()
{
	return SMap->GetTexture();
}
