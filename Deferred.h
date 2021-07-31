#pragma once
//INCLUDE//
#include <GL/glew.h>
#include <stdio.h>
#include <string.h>

//INCLUDE CLASSES//
#include "ObjectDataHandler.h"
#include "QuadTree.h"
#include "Light.h"

extern matrix pvw;

class Deferred
{
private:
	void CreateBuffer(int windowWidth, int windowHeight);
	void CreateFinalBuffer(int windowWidth, int windowHeight);
	GLuint GenerateColorTexture(int width, int height);
	GLuint GenerateVec3Texture(int width, int height);
	void GeoPass(ObjectDataHandler *objects[], PlanePoints far, PlanePoints near, float delta, float rotateAmount, int nrOfObjects);
	void BeginLightPasses();
	void LightsPass(Light *lights[], int nrOfLights);

	ObjectDataHandler *_objects;
	GLuint FB, FinalFB;
	GLuint  TexturePosition,
			TextureNormal,
			TextureColor,
			TextureUV,
			DepthTexture,
			FinalTexture;
	Shader *FSQShader;
	GLuint VA, VB;
	ShadowMap *SMap;

public:
	Deferred(int WindowWidth, int WindowHeight, Shader *FSQShader);
	~Deferred();
	void Render(ObjectDataHandler *objects[], PlanePoints far, PlanePoints near, float delta, float rotateAmount, int nrOfObjects, Light *lights[], int nrOfLights);
	GLuint GetFinalTexture();
	GLuint GetColorTexture();
	GLuint GetPositionTexture();
	GLuint GetUVTexture();
	GLuint GetNormalTexture();

	FullScreenQuad *quad;
	Shader *GeoShader, *LightShader;
};

Deferred::Deferred(int WindowWidth, int WindowHeight,Shader *FSQShader)
{
	GeoShader = new Shader("GeoPass-vs.glsl","GeoPass-gs.glsl", "GeoPass-fs.glsl");
	LightShader = new Shader("LightPass-vs.glsl", "LightPass-fs.glsl");
	quad = new FullScreenQuad(FSQShader);
	CreateBuffer(WindowWidth, WindowHeight);
	CreateFinalBuffer(WindowWidth, WindowHeight);
}

Deferred::~Deferred()
{
	delete GeoShader;
	delete LightShader;
	delete quad;
}

void Deferred::Render(ObjectDataHandler * objects[], PlanePoints far, PlanePoints near, float delta, float rotateAmount, int nrOfObjects, Light *lights[], int nrOfLights)
{
	glEnable(GL_DEPTH_TEST);

	for (int i = 0; i < nrOfLights; i++) {
		if (lights[i]->_type == 0) lights[i]->SMap->Render(objects, far, near, delta, rotateAmount, nrOfObjects);
	}

	GeoPass(objects, far, near, delta, rotateAmount, nrOfObjects);
	glDisable(GL_DEPTH_TEST);
	BeginLightPasses();

	//SEND KD AND KS TO LIGHTPASS
	LightShader->useShader();
	LightShader->setUniform("materialDiffuse", glm::vec3{ 1,1,1 });
	LightShader->setUniform("materialSpecular", glm::vec3{ 1,1,1 });
	for (int i = 0; i < nrOfObjects; i++)
	{
		if (objects[i]->getKD() != glm::vec3{ 1,1,1 }) {
			LightShader->setUniform("materialDiffuse", objects[i]->getKD());
		}
		if (objects[i]->getKS() != glm::vec3{ 1,1,1 }) {
			LightShader->setUniform("materialSpecular", objects[i]->getKS());
		}
	}

	LightsPass(lights, nrOfLights);
}

void Deferred::CreateBuffer(int WindowWidth, int WindowHeight)
{
	//CREATE GEOMETRY-FRAMEBUFFER
	glGenFramebuffers(1, &FB);
	glBindFramebuffer(GL_FRAMEBUFFER, FB);

	//CREATE TEXTURES TO STORE INFO
	TexturePosition = GenerateVec3Texture(WindowWidth, WindowHeight);
	TextureNormal = GenerateColorTexture(WindowWidth, WindowHeight);
	TextureColor = GenerateVec3Texture(WindowWidth, WindowHeight);
	TextureUV = GenerateVec3Texture(WindowWidth, WindowHeight);

	//COLOR ATTACHMENTS
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TexturePosition, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, TextureNormal, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, TextureColor, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, TextureUV, 0);

	GLuint attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

	//DEPTH TEXTURE
	glBindTexture(GL_TEXTURE_2D, DepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTexture, 0);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw "Deffered: GeometryFB incomplete";

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Deferred::CreateFinalBuffer(int width, int height)
{
	//CREATE FINAL-FRAMEBUFFER
	glGenFramebuffers(1, &FinalFB);
	glBindFramebuffer(GL_FRAMEBUFFER, FinalFB);

	//CREATE FINALTEXTURE OUT
	FinalTexture = GenerateColorTexture(width, height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FinalTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw "Deffered: FinalFB incomplete";

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint Deferred::GenerateVec3Texture(int width, int height)
{
	//GENERATE A VEC3 TEXTURE
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

GLuint Deferred::GenerateColorTexture(int width, int height)
{
	//CREATE A COLOR TEXTURE(STORE POSITIV VALUES)
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

void Deferred::GeoPass(ObjectDataHandler *objects[], PlanePoints far, PlanePoints near, float delta, float rotateAmount, int nrOfObjects)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FB);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//USE GEOMETRYPASS-SHADER AND CREATE EVERYTHING  
	//SO VALUES FROM THE SCENE CAN BE STORED IN OUR TEXTURES
	GeoShader->useShader();
	GeoShader->setUniform("world", pvw.World);
	GeoShader->setUniform("view", pvw.View);
	GeoShader->setUniform("proj", pvw.Projection);

	for (int i = 0; i < nrOfObjects; i++) {
		objects[i]->renderAll(far, near, GeoShader, delta, rotateAmount);
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDepthMask(GL_FALSE);

}

inline void Deferred::BeginLightPasses()
{
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	glBindFramebuffer(GL_FRAMEBUFFER, FinalFB);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

inline void Deferred::LightsPass(Light * lights[], int nrOfLights)
{
	//USE LIGHTPASS-SHADER AND CALCULATE LIGHT EQUATIONS
	//FOR A POINT-LIGHT OR A DIRECTIONAL-LIGHT
	LightShader->useShader();

	LightShader->setTexture2D(0, "PositionMap", TexturePosition);
	LightShader->setTexture2D(1, "NormalMap", TextureNormal);
	LightShader->setTexture2D(2, "ColorMap", TextureColor);

	for (int i = 0; i < nrOfLights; i++) {
		//POINT LIGHT
		if (lights[i]->_type == 1) {
			LightShader->setUniform("lightType", lights[i]->_type);
			LightShader->setUniform("lightPosition", lights[i]->_position);
			LightShader->setUniform("lightColor", lights[i]->_color);
			LightShader->setUniform("ambientLight", lights[i]->_color * lights[i]->_ambientIntensity);
			quad->Draw();
		}
		//DIRECTIONAL LIGHT
		else {
			LightShader->setUniform("lightMatrix", lights[i]->SMap->GetMatrix());
			LightShader->setTexture2D(3, "ShadowMap", lights[i]->SMap->GetTexture());
			LightShader->setUniform("lightType", lights[i]->_type);
			LightShader->setUniform("lightDirection", lights[i]->_direction);
			LightShader->setUniform("lightColor", lights[i]->_color);
			LightShader->setUniform("ambientLight", (lights[i]->_color * lights[i]->_ambientIntensity));
			quad->Draw();
		}
	}
	glDisable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint Deferred::GetFinalTexture()
{
	return this->FinalTexture;
}

GLuint Deferred::GetColorTexture()
{
	return this->TextureNormal;
}

inline GLuint Deferred::GetPositionTexture()
{
	return this->TexturePosition;
}

inline GLuint Deferred::GetUVTexture()
{
	return this->TextureUV;
}

GLuint Deferred::GetNormalTexture()
{
	return this->TextureColor;
}
