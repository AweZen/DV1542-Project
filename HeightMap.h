#pragma once
#include "include/ImageLoader/ImageLoader.h"
#include <iostream>
#include <vector>
#include <stdlib.h>


class HeightMap
{
private:
	float _width, _length;
	float **_heights;
	glm::vec3 ** _normals;
	glm::vec3 _positions;
	bool _updatedNormals;

public:
	Image *_image;

	HeightMap(const char* fileName);
	~HeightMap();

	float getWidth();
	float getLength();
	float getHeight(int x, int z);
	void setHeight(int x, int z, float y);
	glm::vec3 setNgetPos(float x, float y, float z);
};

HeightMap::HeightMap(const char* fileName)
{
	_image = loadBMP(fileName);
	_width = _image->width;
	_length = _image->height;

	_heights = new float*[_length];
	for (int i = 0; i < _length; i++) {
		_heights[i] = new float[_width];
	}

	_normals = new glm::vec3*[_length];
	for (int i = 0; i < _length; i++) {
		_normals = new glm::vec3*[_width];
	}

	_updatedNormals = false;
}

HeightMap::~HeightMap()
{
}

float HeightMap::getWidth()
{
	return _width;
}

float HeightMap::getLength()
{
	return _length;
}

float HeightMap::getHeight(int x, int z)
{
	if (_heights[x][z])
		return _heights[x][z];
}

void HeightMap::setHeight(int x, int z, float y)
{
	_heights[z][x] = y;
	_updatedNormals = false;
}

glm::vec3 HeightMap::setNgetPos(float x, float y, float z)
{
	_positions = glm::vec3(x, y, z);
	return _positions;
}