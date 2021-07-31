#pragma once
#include "FullScreenQuad.h"
extern matrix pvw;

class ShadowMap
{
private:
	glm::mat4 lightSpaceMatrix;
	GLuint depthMapFramebuffer;
	GLuint depthMap;
	Shader *shadowShader;
	float width, height;

	void CreateDepthMapBuffer();
public:
	ShadowMap(float width,float height, glm::vec3 dir, Shader *FSQShader);
	~ShadowMap();

	void Render(ObjectDataHandler * objects[], PlanePoints far, PlanePoints near, float delta, float rotateAmount, int nrOfObjects);
	GLuint GetTexture();
	glm::mat4 GetMatrix();
};

ShadowMap::ShadowMap(float width, float height, glm::vec3 dir, Shader *FSQShader)
{
	shadowShader = new Shader("ShadowShaderVS.glsl", "ShadowShaderFS.glsl");
	this->width = width;
	this->height = height;
	CreateDepthMapBuffer();

	glm::vec3 lightInvDir = dir;
	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-15, 25, -15, 25, -10, 20);
	glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 depthModelMatrix = glm::mat4(1.0);
	this->lightSpaceMatrix = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

}

ShadowMap::~ShadowMap()
{
}

inline void ShadowMap::Render(ObjectDataHandler * objects[], PlanePoints far, PlanePoints near, float delta, float rotateAmount, int nrOfObjects)
{

	shadowShader->useShader();
	glDepthMask(GL_TRUE);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFramebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	shadowShader->setUniform("LSM", this->lightSpaceMatrix);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);


	for (int i = 0; i < nrOfObjects; i++) {
			objects[i]->renderAll(far, near, this->shadowShader, delta, rotateAmount, false);
	}
	glDisable(GL_CULL_FACE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDepthMask(GL_FALSE);

}

GLuint ShadowMap::GetTexture()
{
	return this->depthMap;
}

glm::mat4 ShadowMap::GetMatrix()
{
	return this->lightSpaceMatrix;
}

inline void ShadowMap::CreateDepthMapBuffer()
{
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->width, this->height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glGenFramebuffers(1, &depthMapFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


