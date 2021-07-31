#pragma once

//Allt shader skit in hit
#include <GL/glew.h>
//DEFAULT C++ INCLUDES
#include <string>
#include <stdlib.h>
#include <fstream>
#include <streambuf>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>

using namespace::std;

class Shader
{
private:
	GLint _shaderProgram;
	GLuint _activeProgram = GL_NONE;
public:

	Shader(const GLchar* VertexShaderFile , const GLchar* FragmentShaderFile );
	Shader(const GLchar* VertexShaderFile, const GLchar* GeoShaderFile, const GLchar* FragmentShaderFile);
	~Shader();
	
	GLuint loadShader(GLenum ShaderType, const GLchar* fileName);
	void useShader();
	GLuint getShader();
	GLint getUniform(const GLchar * name);
	void setUniform(const GLchar * name, const int & value);
	void setUniform(const GLchar * name, const float & value);
	void setUniform(const GLchar * name, const glm::vec2 & vector);
	void setUniform(const GLchar * name, const glm::vec3 & vector);
	void setUniform(const GLchar * name, const glm::mat4 & matrix);
	void setTexture2D(int id, const GLchar * name, GLuint texture);
};

Shader::Shader(const GLchar* VertexShaderFile, const GLchar* FragmentShaderFile)
{
	GLuint vs = loadShader(GL_VERTEX_SHADER, VertexShaderFile);
	GLuint fs = loadShader(GL_FRAGMENT_SHADER, FragmentShaderFile);

	_shaderProgram = glCreateProgram();

	glAttachShader(_shaderProgram, vs);
	glAttachShader(_shaderProgram, fs);

	glLinkProgram(_shaderProgram);

	glDetachShader(_shaderProgram, vs);
	glDetachShader(_shaderProgram, fs);

	glDeleteShader(vs);
	glDeleteShader(fs);


	GLsizei  sizei = 0;
	GLchar test[1000];
	glGetProgramInfoLog(_shaderProgram,1000, &sizei,test);

	for (GLsizei i = 0; i < sizei; i++) {
		cout << test[i];

	}
}

inline Shader::Shader(const GLchar * VertexShaderFile, const GLchar * GeoShaderFile, const GLchar * FragmentShaderFile)
{
	GLuint vs = loadShader(GL_VERTEX_SHADER, VertexShaderFile);
	GLuint gs = loadShader(GL_GEOMETRY_SHADER, GeoShaderFile);
	GLuint fs = loadShader(GL_FRAGMENT_SHADER, FragmentShaderFile);

	_shaderProgram = glCreateProgram();

	glAttachShader(_shaderProgram, vs);
	glAttachShader(_shaderProgram, gs);
	glAttachShader(_shaderProgram, fs);

	glLinkProgram(_shaderProgram);

	glDetachShader(_shaderProgram, vs);
	glDetachShader(_shaderProgram, gs);
	glDetachShader(_shaderProgram, fs);

	glDeleteShader(vs);
	glDeleteShader(gs);
	glDeleteShader(fs);


	GLsizei  sizei = 0;
	GLchar test[1000];
	glGetProgramInfoLog(_shaderProgram, 1000, &sizei, test);

	for (GLsizei i = 0; i < sizei; i++) {
		cout << test[i];
	}
}

Shader::~Shader()
{
	glDeleteProgram(_shaderProgram);
}

GLuint Shader::loadShader(GLenum ShaderType, const GLchar * ShaderFile)
{
	GLuint shader = glCreateShader(ShaderType);

	ifstream shaderFile(ShaderFile);

	string shaderText((istreambuf_iterator<char>(shaderFile)), istreambuf_iterator<char>());
	shaderFile.close();
	const char* shaderTextPtr = shaderText.c_str();
	glShaderSource(shader, 1, &shaderTextPtr, nullptr);
	glCompileShader(shader);

	return shader;
}

void Shader::useShader()
{
	glUseProgram(0);
	glUseProgram(_shaderProgram);
	_activeProgram = _shaderProgram;
}

GLuint Shader::getShader() {
	return _shaderProgram;
}

GLint Shader::getUniform(const GLchar * name)
{
	if (_activeProgram != _shaderProgram)
		throw printf("cant get uniform, shader not active");

	GLint location = glGetUniformLocation(_shaderProgram, name);

	if (location == -1)
		printf("cant find uniform '%s' in shader nr %d\n", name, _shaderProgram);

	return location;
}

void Shader::setUniform(const GLchar * name, const int & value)
{
	glUniform1i(getUniform(name), value);
}

void Shader::setUniform(const GLchar * name, const float & value)
{
	glUniform1f(getUniform(name), value);
}

void Shader::setUniform(const GLchar * name, const glm::vec2 & vector)
{
	glUniform2fv(getUniform(name), 1, value_ptr(vector));
}

void Shader::setUniform(const GLchar * name, const glm::vec3 & vector)
{
	glUniform3fv(getUniform(name), 1, value_ptr(vector));
}

void Shader::setUniform(const GLchar * name, const glm::mat4 & matrix)
{
	glUniformMatrix4fv(getUniform(name), 1, GL_FALSE, value_ptr(matrix));
}
void Shader::setTexture2D(int id, const GLchar * name, GLuint texture)
{
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_2D, texture);
	setUniform(name, id);
}
