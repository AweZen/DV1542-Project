#pragma once
#include "Deferred.h"
#include "Blur.h"
class Glow
{
private:
	Blur *blur;
	Shader *glow, *merge;
	GLuint glowFramebuffer;
	GLuint glowTexture;
	GLuint blurFramebuffers[2];
	GLuint blurTextures[2];
	GLuint finalFramebuffer;
	GLuint finalTexture;

	void createGlowBuffers(int width, int height);
	void createBlurBuffers(int width, int height);
	void createFinalBuffers(int width, int height);

public:
	Glow(int width, int height,Shader *FSQShader);
	~Glow();

	void renderGlowTexture(GLuint texture,ObjectDataHandler *objects[], PlanePoints far, PlanePoints near, float delta, float rotateAmount, int nrOfObjects);
	void renderGlow(GLuint texture,GLuint colorTexture, ObjectDataHandler *objects[], PlanePoints far, PlanePoints near, float delta, float rotateAmount, int nrOfObjects);
	GLuint getFinalTexture();

	FullScreenQuad *quad;
};

Glow::Glow(int width, int height, Shader *FSQShader)
{
	glow = new Shader("FSQvs.glsl", "GlowExtract.frag");
	merge = new Shader("FSQvs.glsl", "GlowMerge.frag");
	blur = new Blur(width, height,FSQShader);
	quad = new FullScreenQuad(FSQShader);
	createGlowBuffers(width, height);
	createBlurBuffers(width, height);
	createFinalBuffers(width, height);
}

Glow::~Glow()
{
	delete glow;
	delete merge;
	delete blur;
	delete quad;
}

void Glow::renderGlowTexture(GLuint texture,ObjectDataHandler * objects[],PlanePoints far , PlanePoints near, float delta, float rotateAmount, int nrOfObjects)
{
	glBindFramebuffer(GL_FRAMEBUFFER, glowFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glow->useShader();
	glow->setTexture2D(0, "DeferredTexture", texture);

	quad->Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Glow::renderGlow(GLuint texture, GLuint colorTexture, ObjectDataHandler * objects[], PlanePoints far, PlanePoints near, float delta, float rotateAmount, int nrOfObjects)
{
	renderGlowTexture(colorTexture,objects, far, near, delta, rotateAmount, nrOfObjects);
	blur->createBlur(glowTexture, 30);

	glBindFramebuffer(GL_FRAMEBUFFER, finalFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	merge->useShader();
	merge->setTexture2D(0, "DeferredTexture", texture);
	merge->setTexture2D(1, "glow", blur->getBlurTexture());
	
	quad->Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint Glow::getFinalTexture()
{
	return this->finalTexture;
}

void Glow::createGlowBuffers(int width, int height)
{
	glGenFramebuffers(1, &glowFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, glowFramebuffer);

	glGenTextures(1, &glowTexture);
	glBindTexture(GL_TEXTURE_2D, glowTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glowTexture, 0);

	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw "Glow Effect: Glow Framebuffer not complete!";

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Glow::createBlurBuffers(int width, int height)
{
	glGenFramebuffers(2, blurFramebuffers);
	glGenTextures(2, blurTextures);

	for (int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, blurFramebuffers[i]);
		glBindTexture(GL_TEXTURE_2D, blurTextures[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTextures[i], 0);

		GLenum attachments[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, attachments);

		//DEPTH BUFFER
		GLuint depthBuffer;
		glGenRenderbuffers(1, &depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			throw "Glow Effect: Blur Framebuffer not complete!";
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Glow::createFinalBuffers(int width, int height)
{
	glGenFramebuffers(1, &finalFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, finalFramebuffer);

	glGenTextures(1, &finalTexture);
	glBindTexture(GL_TEXTURE_2D, finalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, finalTexture, 0);

	//DEPTH BUFFER
	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw "Glow Effect: Glow Framebuffer not complete!";

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
