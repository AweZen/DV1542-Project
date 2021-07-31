
#pragma once
#include "ObjectData.h"
#include "QuadTree.h"


class ObjectDataHandler
{
private:
	QuadTree *_tree;
	glm::vec3 _kd, _ks;
	int _nrOfObjects;
	int _cap;
	bool _culling, _normalMap;
public:
	ObjectDataHandler(int cap, bool culling, bool normalMap);
	~ObjectDataHandler();
	bool addObject(ObjectType type, float size, glm::mat4 posInWorld, glm::vec3 pos, const char* fileName, glm::vec2 uvBotLeft = { 0,0 }, glm::vec2 uvTopRight = { 1,1 }, bool movingTexture = false, const char* normalTex = " ");
	bool addObject(ObjectType type, glm::mat4 worldMat, const char * fileBMP, const char* tex, float intensity = 1, glm::vec3 pos = { 0,0,0 });
	bool addObject(glm::mat4 posInWorld, glm::vec3 pos, const char* fileNameObj);
	void renderAll(PlanePoints far, PlanePoints near, Shader *shader, float delta, float rotateAmount);
	void renderAll(PlanePoints far, PlanePoints near, Shader *shader, float delta, float rotateAmount,bool culling);
	void renderDebug(matrix pvw, Shader *shader);
	float getHeight(float x, float z, float xd, float zd);
	glm::vec3 getKD();
	glm::vec3 getKS();
};

ObjectDataHandler::ObjectDataHandler(int cap = 100, bool culling = true, bool normalMap = false)
{
	_tree = new QuadTree(rectrangle(),1);
	_nrOfObjects = 0;
	_cap = cap;
	_culling = culling;
	_normalMap = normalMap;
	_kd = { 1,1,1 };
	_ks = { 1,1,1 };
}

ObjectDataHandler::~ObjectDataHandler()
{
	delete _tree;
}

bool ObjectDataHandler::addObject(ObjectType type, float size, glm::mat4 posInWorld, glm::vec3 pos, const char* fileName, glm::vec2 uvTopRightv, glm::vec2 uvBotLeft, bool movingTexture, const char* normalTex)
{
	if (_nrOfObjects != _cap) {
		ObjectData temp(type, size, posInWorld, pos, uvTopRightv, uvBotLeft, movingTexture);
		temp.init();
		temp.setTexture(fileName);
		if (_normalMap) {
			temp.setNormalMap(normalTex);
		}
		_tree->insert(temp);
		_nrOfObjects++;
	}
	else {

		return false;
	}
}

bool ObjectDataHandler::addObject(ObjectType type, glm::mat4 worldMat, const char * fileBMP, const char* tex, float intensity, glm::vec3 pos)
{
	if (_nrOfObjects != _cap) {
		ObjectData temp(type, fileBMP, intensity, worldMat, pos);
		temp.init();
		temp.setTexture(tex);
		_tree->insert(temp);
		_nrOfObjects++;
	}
	else {

		return false;
	}
}

bool ObjectDataHandler::addObject(glm::mat4 posInWorld, glm::vec3 pos, const char* fileNameObj)
{
	std::vector < Vertex > out_vertices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;
	std::vector< glm::vec3 > temp_Indices;

	std::ifstream objContent(fileNameObj);
	std::string line;
	std::string linemtl;
	std::string mtllib;
	std::string texture;

	while (std::getline(objContent, line)) {
		std::stringstream lineStream(line);
		std::string linePart;

		if (std::getline(lineStream, linePart, ' '))
		{
			if (linePart == "v") {
				std::string part;
				glm::vec3 point;

				getline(lineStream, part, ' ');
				point.x = std::stof(part);
				getline(lineStream, part, ' ');
				point.y = std::stof(part);
				getline(lineStream, part, ' ');
				point.z = std::stof(part);

				temp_vertices.push_back(point);
			} 

			else if (linePart == "vt") {
				std::string part;
				glm::vec2 uv;

				getline(lineStream, part, ' ');
				uv.x = std::stof(part);
				getline(lineStream, part, ' ');
				uv.y = std::stof(part);

				temp_uvs.push_back(uv);
			}
			else if (linePart == "vn") {
				std::string part;
				glm::vec3 normal;

				getline(lineStream, part, ' ');
				normal.x = std::stof(part);
				getline(lineStream, part, ' ');
				normal.y = std::stof(part);
				getline(lineStream, part, ' ');
				normal.z = std::stof(part);

				temp_normals.push_back(normal);
			}
			else if (linePart == "f") {
				std::string part;

				while (getline(lineStream, part, ' '))
				{
					std::stringstream partStream(part);
					std::string subPart;
					glm::ivec3 vertex;

					getline(partStream, subPart, '/');
					vertex.x = std::stoi(subPart);
					getline(partStream, subPart, '/');
					vertex.y = std::stoi(subPart);
					getline(partStream, subPart, ' ');
					vertex.z = std::stoi(subPart);

					temp_Indices.push_back(vertex);
				}
			}
			else if (linePart == "mtllib")
			{
				std::getline(lineStream, linePart);
				mtllib = linePart;
			}
		}
	}
	objContent.close();


	for (glm::vec3 & v : temp_Indices){

		out_vertices.push_back({
			temp_vertices[v.x - 1],
			temp_uvs[v.y - 1],
			temp_normals[v.z - 1]
		});

	}

	std::ifstream mtlContent("res/" + mtllib);
	while (std::getline(mtlContent, linemtl)) {
		std::stringstream lineStream(linemtl);
		std::string linePart;
		if (std::getline(lineStream, linemtl, ' '))
		{
			if (linemtl == "Ns") {

			}
			if (linemtl == "Ka") {

			}
			if (linemtl == "Kd") {
				std::string part;
				glm::vec3 temp;
				getline(lineStream, part, ' ');
				temp.x = std::stof(part);
				getline(lineStream, part, ' ');
				temp.y = std::stof(part);
				getline(lineStream, part, ' ');
				temp.z = std::stof(part);
				_kd = temp;
			}
			if (linemtl == "Ks") {
				std::string part;
				glm::vec3 temp;
				getline(lineStream, part, ' ');
				temp.x = std::stof(part);
				getline(lineStream, part, ' ');
				temp.y = std::stof(part);
				getline(lineStream, part, ' ');
				temp.z = std::stof(part);
				_ks = temp;
			}
			if (linemtl == "Ke") {

			}
			if (linemtl == "Ni") {

			}
			if (linemtl == "d") {

			}
			if (linemtl == "illum") {

			}			
			if (linemtl == "map_Kd") {
				std::getline(lineStream, linePart);
				texture = "res/"+linePart;

			}
		}
	
	}
	char textureChar[100];
	strcpy(textureChar, texture.c_str());
	mtlContent.close();

	ObjectData temp(posInWorld, pos , out_vertices);
	temp.init();
	temp.setTexture(textureChar);
	_tree->insert(temp);
	_nrOfObjects++;

	return true;

}

void ObjectDataHandler::renderAll(PlanePoints far, PlanePoints near,Shader *shader, float delta, float rotateAmount)
{
	if (_normalMap) {
		shader->setUniform("calcBumpNormal", 1);
	}
	else {
		shader->setUniform("calcBumpNormal", 0);
	}
	_tree->drawObject(far,near,shader, delta, rotateAmount,_culling, _normalMap);
}

void ObjectDataHandler::renderAll(PlanePoints far, PlanePoints near, Shader * shader, float delta, float rotateAmount, bool culling)
{
	if (_normalMap) {
		shader->setUniform("calcBumpNormal", 1);
	}
	else {
		shader->setUniform("calcBumpNormal", 0);
	}
	_tree->drawObject(far, near, shader, delta, rotateAmount, culling, _normalMap);
}

void ObjectDataHandler::renderDebug(matrix pvw, Shader * shader)
{
	_tree->drawDebug(pvw, shader);
}

float ObjectDataHandler::getHeight(float x, float z, float xd, float zd)
{
	return _tree->getObject()->getHeight(x, z, xd, zd);
}

glm::vec3 ObjectDataHandler::getKD()
{
	return _kd;
}

glm::vec3 ObjectDataHandler::getKS()
{
	return _ks;
}