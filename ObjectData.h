#pragma once
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

//OPENGL RELATERADE INCLUDES
#include "glew-1.11.0\include\GL\glew.h"
#include "include/GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include <gl/GL.h>
#include "include/SOIL2/SOIL2.h"


//INCLUDE CLASSES
#include "HeightMap.h"
#include "Shader.h"

//PRAGMA
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

enum ObjectType {
	Quad, Cube, Plane, Sphere,Terrain,OBJ
};

struct Vertex
{
	glm::vec3 postion;
	glm::vec2 uv;
	glm::vec3 normal;
	glm::vec3 tangent;
};

class ObjectData
{
private:
	glm::vec3 calcNormal(glm::vec3 one, glm::vec3 two, glm::vec3 three);

	Vertex * vertices;
	unsigned int *indices;
	GLuint _nrOfV;
	GLuint _nrOfI;
	GLsizeiptr vertexBufferSize()const {
		return (_nrOfV * sizeof(Vertex));
	}
	GLsizeiptr indexBufferSize()const {
		return (_nrOfI * sizeof(unsigned int));
	}

	unsigned char *_image, *_img;
	int _width, _height;
	float _rotation = 0.f;
	glm::mat4 _modelMatrix;
	HeightMap *_hMap;
	glm::vec3 _kd, _ks;
public:
	ObjectData(ObjectType type, float size, glm::mat4 posInWorld, glm::vec3 pos, glm::vec2 uvTopRight = { 0,0 }, glm::vec2 uvBotLeft = { 1,1 }, bool movingTexture = false);
	ObjectData(ObjectType type, const char* fileBMP, float heightIntensity, glm::mat4 worldMat, glm::vec3 pos);
	ObjectData(glm::mat4 posInWorld, glm::vec3 pos, std::vector < Vertex > vertex);
	~ObjectData();

	void setTexture(const char* fileName);
	void setNormalMap(const char* fileName);
	void draw(Shader *ShaderProgram, float delta, float rotateAmount, bool normal = false);
	void init();
	float getHeight(float x, float z, float xd, float zd);

	GLuint VB, IB, VA;
	ObjectType _type;
	GLuint _texture, _textureNormal;
	glm::vec3 _position;
	float _size;
	bool _movingTexture;
	float _timeSinceTexChange;
	glm::vec2 _defaultUv[2];
};

ObjectData::ObjectData(ObjectType Objtype, float size, glm::mat4 posInWorld, glm::vec3 pos, glm::vec2 uvBotLeft, glm::vec2 uvTopRight,bool movingTexture)
{
	_defaultUv[0] = uvBotLeft;
	_defaultUv[1] = uvTopRight;
	_movingTexture = movingTexture;
	_timeSinceTexChange = 0;
	_position = pos;
	_modelMatrix = glm::translate(posInWorld,pos);
	_type = Objtype;
	_size = size;

	switch (_type)
	{
		case Quad:
		{
			//CREATE VERTICES DATA
			Vertex vertexData[] =
			{
				 { {-size / 2, -size / 2,  size / 2 }, {glm::vec2(uvBotLeft.x,uvTopRight.y)} , { 0, 0, -1 }},
				 { {-size / 2,  size / 2,  size / 2 }, {uvBotLeft}							 , { 0, 0, -1 }},
				 { { size / 2,  size / 2,  size / 2 }, {glm::vec2(uvTopRight.x,uvBotLeft.y)} , { 0, 0, -1 }},
				 { { size / 2, -size / 2,  size / 2 }, {uvTopRight}							 , { 0, 0, -1 }},
			};
			_nrOfV = sizeof(vertexData) / sizeof(*vertexData);
			vertices = new Vertex[_nrOfV];
			memcpy(vertices, vertexData, sizeof(vertexData));

			//CREATE INDECES DATA
			unsigned int indexData[] = { 0, 2, 1,  0, 3, 2 };
			_nrOfI = sizeof(indexData) / sizeof(*indexData);
			indices = new unsigned int[_nrOfI];
			memcpy(indices, indexData, sizeof(indexData));

			break;
		}

		case Cube:
		{
			//CREATE VERTICES DATA
			Vertex vertexData[] =
			{
				//FRONT
				 { {-size / 2, -size / 2,  size / 2 }, {glm::vec2(uvBotLeft.x,uvTopRight.y)}	, { 0, 0, 1 } },
				 { {-size / 2,  size / 2,  size / 2 }, {uvBotLeft}								, { 0, 0, 1 } },
				 { { size / 2,  size / 2,  size / 2 }, {glm::vec2(uvTopRight.x,uvBotLeft.y)}	, { 0, 0, 1 } },
				 { { size / 2, -size / 2,  size / 2 }, {uvTopRight}								, { 0, 0, 1 } },
				 //BACK
				 { { size / 2, -size / 2, -size / 2 }, {glm::vec2(uvBotLeft.x,uvTopRight.y)}	, { 0, 0, -1 } },
				 { { size / 2,  size / 2, -size / 2 }, {uvBotLeft}								, { 0, 0, -1 } },
				 { {-size / 2,  size / 2, -size / 2 }, {glm::vec2(uvTopRight.x,uvBotLeft.y)}	, { 0, 0, -1 } },
				 { {-size / 2, -size / 2, -size / 2 }, {uvTopRight}								, { 0, 0, -1 } },
				 //TOP
				 { {-size / 2,  size / 2,  size / 2 }, {glm::vec2(uvBotLeft.x,uvTopRight.y)}	, { 0, 1,  0 } },
				 { {-size / 2,  size / 2, -size / 2 }, {uvBotLeft}								, { 0, 1,  0 } },
				 { { size / 2,  size / 2, -size / 2 }, {glm::vec2(uvTopRight.x,uvBotLeft.y)}	, { 0, 1,  0 } },
				 { { size / 2,  size / 2,  size / 2 }, {uvTopRight}								, { 0, 1,  0 } },
				 //BOT
				 { {-size / 2,  -size / 2, -size / 2 }, {glm::vec2(uvBotLeft.x,uvTopRight.y)}	, { 0, -1, 0 } },
				 { {-size / 2,  -size / 2,  size / 2 }, {uvBotLeft}								, { 0, -1, 0 } },
				 { { size / 2,  -size / 2,  size / 2 }, {glm::vec2(uvTopRight.x,uvBotLeft.y)}	, { 0, -1, 0 } },
				 { { size / 2,  -size / 2, -size / 2 }, {uvTopRight}							, { 0, -1, 0 } },
				 //RIGHT
				 { { size / 2,  -size / 2,  size / 2 }, {glm::vec2(uvBotLeft.x,uvTopRight.y)}	, { 1, 0, 0 } },
				 { { size / 2,   size / 2,  size / 2 }, {uvBotLeft}								, { 1, 0, 0 } },
				 { { size / 2,   size / 2, -size / 2 }, {glm::vec2(uvTopRight.x,uvBotLeft.y)}	, { 1, 0, 0 } },
				 { { size / 2,  -size / 2, -size / 2 }, {uvTopRight}							, { 1, 0, 0 } },
				 //LEFT                                              
				 { {-size / 2,  -size / 2, -size / 2 }, {glm::vec2(uvBotLeft.x,uvTopRight.y)}	, { -1, 0, 0 } },
				 { {-size / 2,   size / 2, -size / 2 }, {uvBotLeft}								, {	-1, 0, 0 } },
				 { {-size / 2,   size / 2,  size / 2 }, {glm::vec2(uvTopRight.x,uvBotLeft.y)}	, { -1, 0, 0 } },
				 { {-size / 2,  -size / 2,  size / 2 }, {uvTopRight}							, { -1, 0, 0 } }
			};

			_nrOfV = sizeof(vertexData) / sizeof(*vertexData);
			vertices = new Vertex[_nrOfV];
			memcpy(vertices, vertexData, sizeof(vertexData));

			//CREATE INDECES DATA
			unsigned int indexData[] = { 0 , 2,  1,   0,  3,  2,
										  4,  6,  5,   4,  7,  6,
										  8,  10, 9,   8, 11, 10,
										 12, 14, 13,  12, 15, 14,
										 16, 18, 17,  16, 19, 18,
										 20, 22, 21,  20, 23, 22 };
			_nrOfI = sizeof(indexData) / sizeof(*indexData);
			indices = new unsigned int[_nrOfI];
			memcpy(indices, indexData, sizeof(indexData));

			break;
		}

		case Plane:
		{
			//CREATE VERTICES DATA
			Vertex vertexData[] =
			{
			 { {-size / 2,	-2,  size / 2 }, { 0, 1 }, { 0, 1, 0 } },
			 { {-size / 2,  -2, -size / 2 }, { 0, 0 }, { 0, 1, 0 } },
			 { { size / 2,	-2, -size / 2 }, { 1, 0 }, { 0, 1, 0 } },
			 { { size / 2,	-2,  size / 2 }, { 1, 1 }, { 0, 1, 0 } }
			};
			_nrOfV = sizeof(vertexData) / sizeof(*vertexData);
			vertices = new Vertex[_nrOfV];
			memcpy(vertices, vertexData, sizeof(vertexData));

			//CREATE INDECES DATA
			unsigned int indexData[] = { 0, 2, 1,  0, 3, 2 };
			_nrOfI = sizeof(indexData) / sizeof(*indexData);
			indices = new unsigned int[_nrOfI];
			memcpy(indices, indexData, sizeof(indexData));

			break;
		}
	}

	//CALCULATE TANGENTS
	for (unsigned int i = 0; i < _nrOfI; i += 3) {
		Vertex v0 = vertices[indices[i]];
		Vertex v1 = vertices[indices[i + 1]];
		Vertex v2 = vertices[indices[i + 2]];

		glm::vec3 edge1 = v1.postion - v0.postion;
		glm::vec3 edge2 = v2.postion - v0.postion;

		float U1 = v1.uv.x - v0.uv.x;
		float V1 = v1.uv.y - v0.uv.y;
		float U2 = v2.uv.x - v0.uv.x;
		float V2 = v2.uv.y - v0.uv.y;

		float f = 1.0f / (U1 * V2 - U2 * V1);

		glm::vec3 Tangent, Bitangent;

		Tangent.x = f * (V2 * edge1.x - V1 * edge2.x);
		Tangent.y = f * (V2 * edge1.y - V1 * edge2.y);
		Tangent.z = f * (V2 * edge1.z - V1 * edge2.z);

		Bitangent.x = f * (-U2 * edge1.x - U1 * edge2.x);
		Bitangent.y = f * (-U2 * edge1.y - U1 * edge2.y);
		Bitangent.z = f * (-U2 * edge1.z - U1 * edge2.z);

		vertices[indices[i]].tangent += Tangent;
		vertices[indices[i + 1]].tangent += Tangent;
		vertices[indices[i + 2]].tangent += Tangent;
	}

	for (unsigned int i = 0; i < _nrOfV; i++) {
		glm::vec3 t = glm::normalize(vertices[i].tangent);
		vertices[i].tangent = t;
	}

}

ObjectData::ObjectData(ObjectType type, const char* fileBMP, float heightIntensity, glm::mat4 worldMat, glm::vec3 pos)
{
	if (type == Terrain)
	{
		_position = pos;
		_type = type;

		_modelMatrix = glm::translate(worldMat, pos);
		_hMap = new HeightMap(fileBMP);

		//CREATE VERTICES DATA
		_nrOfV = (_hMap->_image->width * _hMap->_image->height);
		vertices = new Vertex[_nrOfV];
		for (int y = 0; y < _hMap->_image->height; y++) {
			for (int x = 0; x < _hMap->_image->width; x++) {
				//CALCULATE HEIGHTS DEPENDING ON PIXELCOLOR OF HEIGHTMAP
				unsigned char color =
					(unsigned char)_hMap->_image->pixels[3 * (y * _hMap->_image->height + x)];
				float h = heightIntensity * ((color / 255.0f));
				_hMap->setHeight(x, y, h);

				vertices[x + (_hMap->_image->height * y)].postion.x = x - (_hMap->getWidth() / 2);
				vertices[x + (_hMap->_image->height * y)].postion.y = _hMap->getHeight(y, x);
				vertices[x + (_hMap->_image->height * y)].postion.z = y - (_hMap->getLength() / 2);

				vertices[x + (_hMap->_image->width * y)].uv.x = 1 - ((_hMap->getWidth() - x) / (_hMap->getWidth()));
				vertices[x + (_hMap->_image->height * y)].uv.y = ((_hMap->getWidth() - y) / (_hMap->getWidth()));

				vertices[x + (_hMap->_image->width * y)].normal = { 0,0,0 };
			}
		}

		//CREATE INDICES DATA
		_nrOfI = (_hMap->_image->width) * (_hMap->_image->height) * 2 * 3; //2 triangles, 3 indecies
		indices = new unsigned int[_nrOfI];
		unsigned int counter = 0;
		for (int y = 0; y < _hMap->_image->height - 1; y++)
		{
			for (int x = 0; x < _hMap->_image->width - 1; x++)
			{
				indices[counter] = x + ((y + 1) * (_hMap->_image->height));	//left down
				counter += 1;
				indices[counter] = (x + 1) + (y *(_hMap->_image->height));	//right up
				counter += 1;
				indices[counter] = x + (y * (_hMap->_image->height));		//left up	
				counter += 1;

				indices[counter] = x + ((y + 1) * (_hMap->_image->height));			//left down
				counter += 1;
				indices[counter] = (x + 1) + ((y + 1) * (_hMap->_image->height));	//right down
				counter += 1;
				indices[counter] = (x + 1) + (y *(_hMap->_image->height));			//right up
				counter += 1;
			}
		}
	}
}

ObjectData::ObjectData(glm::mat4 posInWorld, glm::vec3 pos, std::vector < Vertex > vertex)
{
	//OBJLOADER
	_modelMatrix = glm::scale(glm::translate(posInWorld, pos),glm::vec3(1));
	_position = pos;
	_type = OBJ;
	_nrOfV = vertex.size();
	_nrOfI = vertex.size();
	vertices = new Vertex[_nrOfV];
	indices = new unsigned int[_nrOfI];

	for (int i = 0; i < vertex.size(); i++) {
		vertices[i].postion = vertex[i].postion;
		vertices[i].uv = vertex[i].uv;
		vertices[i].normal= vertex[i].normal;
		indices[i] = i;
	}
}

ObjectData::~ObjectData()
{

}

void ObjectData::setTexture(const char * fileName)
{
	// BIND TEXTURE
	glGenTextures(1, &_texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);

	//SET TEXTURE PARAMETERS
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//SET TEXTURE FILTERING
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//LOAD AND CREATE TEXTURE
	_image = SOIL_load_image(fileName, &_width, &_height, 0, SOIL_LOAD_RGB);
	if (_image) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, _image);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		cout << "Failed to load texture" << endl;
	SOIL_free_image_data(_image);
}

void ObjectData::setNormalMap(const char* fileName) {
	// BIND TEXTURE
	glGenTextures(1, &_textureNormal);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _textureNormal);

	//SET TEXTURE PARAMETERS
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//SET TEXTURE FILTERING
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//LOAD AND CREATE TEXTURE
	int width, height;
	//_img = SOIL_load_image("res/StoneN.png", &width, &height, 0, SOIL_LOAD_RGB);
	_img = SOIL_load_image(fileName, &width, &height, 0, SOIL_LOAD_RGB);
	if (_img) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, _img);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		cout << "Failed to load texture" << endl;
	SOIL_free_image_data(_img);
}

void ObjectData::draw(Shader *shaderProgram, float delta, float rotateAmount,bool normal)
{
	glBindVertexArray(VA);
	//BIND AND ACTIVATE TEXTURES
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);

	if (shaderProgram->getShader() == 115 || shaderProgram->getShader() == 114)
	{
	}
	else 
	{
		shaderProgram->setTexture2D(0, "DiffuseMap", _texture);
	}
	if (normal && shaderProgram->getShader() != 115 && shaderProgram->getShader() != 114) 
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _textureNormal);
		shaderProgram->setTexture2D(1, "NormalMapping", _textureNormal);
	}

	//CREATE MOVING TEXTURE
	if (_type == Cube && _movingTexture && _timeSinceTexChange > 0.08) {
		for (int i = 0; i < 6; i++) {
			this->vertices[0 + 4 * i].uv.y += _defaultUv[1].y;
			this->vertices[1 + 4 * i].uv.y += _defaultUv[1].y;
			this->vertices[2 + 4 * i].uv.y += _defaultUv[1].y;
			this->vertices[3 + 4 * i].uv.y += _defaultUv[1].y;

			if (this->vertices[3 + 4 * i].uv.y > 1) {
				this->vertices[0 + 4 * i].uv = {glm::vec2(_defaultUv[0].x, _defaultUv[1].y)};
				this->vertices[1 + 4 * i].uv = {_defaultUv[0]};
				this->vertices[2 + 4 * i].uv = {glm::vec2(_defaultUv[1].x, _defaultUv[0].y)};
				this->vertices[3 + 4 * i].uv = {_defaultUv[1]};

			}
			_timeSinceTexChange = 0;
		}
			glBindBuffer(GL_ARRAY_BUFFER, VB);
			glBufferData(GL_ARRAY_BUFFER, vertexBufferSize(), vertices, GL_STATIC_DRAW);

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(float) * 3));
	}
	else if(_movingTexture) _timeSinceTexChange += delta;

	//ROTATE OBJECT
	if (rotateAmount > 0 && _type != Terrain && _type != Plane) {
		_rotation = rotateAmount * delta * 2;
		_modelMatrix = glm::rotate(_modelMatrix, _rotation, glm::vec3(0, 1, 0));
	}

	shaderProgram->setUniform("world", _modelMatrix);

	//DRAW OBJECTS
		glDrawElements(GL_TRIANGLES, _nrOfI, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);
}

void ObjectData::init()
{
	//Bind VertexArray
	glGenVertexArrays(1, &VA);
	glBindVertexArray(VA);

	//Bind VertexBuffer & IndexBuffer
	glGenBuffers(1, &VB);
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize(), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &IB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize(), indices, GL_STATIC_DRAW);

	//Write vertex data to memory
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(float) * 5));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(float) * 8));
}

float ObjectData::getHeight(float x, float z, float xd, float zd)
{
	if ((-_hMap->getWidth() / 2) < int(x) && int(x) < (_hMap->getWidth() / 2) && (-_hMap->getWidth() / 2) < int(z) && int(z) < (_hMap->getWidth() / 2)) {

		return max(vertices[int(x + (_hMap->getWidth() / 2)) + (_hMap->_image->height * int(z + (_hMap->getWidth() / 2)))].postion.y,
			vertices[int(xd + (_hMap->getWidth() / 2)) + (_hMap->_image->height * int(zd + (_hMap->getWidth() / 2)))].postion.y);

	}
	else {

		return 3;
	}
}