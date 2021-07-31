#pragma once
#include "Shader.h"
#include "Deferred.h"
class FullScreenQuad
{
public:
	FullScreenQuad(Shader *FSQShader);
	~FullScreenQuad();
	void Draw();
	void Texture(GLint texture);
private:
	Shader *_FSQShader;
	GLuint VA, VB;
};

FullScreenQuad::FullScreenQuad(Shader *FSQShader)
{
	_FSQShader = FSQShader;

	struct VertexData
	{
		glm::vec3 position;
		glm::vec2 uv;
	};

	VertexData vertices[4] = {
		{ { -1, -1, 0 }, { 0, 0 } },
		{ { -1,  1, 0 }, { 0, 1 } },
		{ {  1, -1, 0 }, { 1, 0 } },
		{ {  1,  1, 0 }, { 1, 1 } }
	};

	glGenVertexArrays(1, &VA);
	glBindVertexArray(VA);

	glGenBuffers(1, &VB);
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), BUFFER_OFFSET(sizeof(float) * 3));
}

FullScreenQuad::~FullScreenQuad()
{
}

void FullScreenQuad::Draw()
{
	glBindVertexArray(VA);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void FullScreenQuad::Texture(GLint texture)
{
	_FSQShader->useShader();
	_FSQShader->setTexture2D(0, "FSQTexture", texture);
	Draw();
}
