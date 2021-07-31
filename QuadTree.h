#pragma once
#include "ObjectData.h"
#include "Shader.h"
extern int counter;

struct Point {
	float x;
	float y;
};

struct matrix {
	glm::mat4 Projection;
	glm::mat4 View;
	glm::mat4 World;
};

struct PlanePoints {
	glm::vec3 topLeft;
	glm::vec3 topRight;
	glm::vec3 botLeft;
	glm::vec3 botRight;
	glm::vec3 leftNormal;
	glm::vec3 rightNormal;
};

struct rectrangle {
	rectrangle(float x=0,float y=0, float width=250,float height=250) {
		this->x = x;
		this->z = y;
		this->width = width;
		this->height = height;
	}

	bool contains(ObjectData point) {
		return(point._position.x >= this->x - this->width	&&
			point._position.x	<= this->x + this->width	&&
			point._position.z	>= this->z - this->height	&&
			point._position.z <= this->z + this->height);
	}

	float x;
	float z;
	float width;
	float height;
};

struct VertexLine
{
	glm::vec3 pos;
	glm::vec3 rgb;
};

class QuadTree
{
private:
	void Subdivide();
	bool inView(PlanePoints far, PlanePoints near);

	rectrangle boundary;
	int capacity;
	bool full;
	ObjectData* Object;
	bool divided;
	unsigned int * indices;
	VertexLine * vertices;
public:
	QuadTree(rectrangle boundary,int cap);
	~QuadTree();
	bool insert(ObjectData object);
	bool gotObject();
	std::string toTextSize();
	ObjectData* getObject();
	QuadTree *northEast, *northWest, *southWest, *southEast;
	void drawDebug(matrix pvw,Shader *shader);
	void drawObject(PlanePoints far, PlanePoints near, Shader *shader, float delta, float rotateAmount, bool culling,bool normal);
	void init();
	GLuint VB, IB, VA;
};

QuadTree::QuadTree(rectrangle boundary, int cap = 1)
{
	this->boundary = boundary;
	this->capacity = cap;
	this->divided = false;
	this->full = false;
	init();
}

QuadTree::~QuadTree()
{
}

bool QuadTree::insert(ObjectData object)
{
	bool inserted = false;
	if (this->boundary.contains(object)) {
		if (!full) {

			this->Object = new ObjectData(object);
			full = true;
			inserted = true;
		}
		else {
			if (!this->divided) {

				this->Subdivide();
				if (northWest->insert(*this->Object)) {
					inserted = true;

				}
				else if(northEast->insert(*this->Object)){
					inserted = true;

				}
				else if (southWest->insert(*this->Object)) {
					inserted = true;

				}
				else if (southEast->insert(*this->Object)) {
					inserted = true;

				}
			}
			if (northWest->insert(object)) {
				inserted = true;

			}
			else if (northEast->insert(object)) {
				inserted = true;

			}
			else if (southWest->insert(object)) {
				inserted = true;

			}
			else if (southEast->insert(object)) {
				inserted = true;

			}


		}
	}
	return inserted;
}

bool QuadTree::gotObject()
{
	return full;
}

std::string QuadTree::toTextSize()
{
	std::string temp;
	temp = std::to_string(this->Object->_size) +" \n";

	if (this->divided) {
		if (this->northEast->gotObject())
			temp += "North East block \n" + this->northEast->toTextSize();
		if (this->northWest->gotObject())
			temp += "North West block \n" +this->northWest->toTextSize();
		if (this->southEast->gotObject())
			temp += "SouthEast Block \n" +this->southEast->toTextSize();
		if (this->southWest->gotObject())
			temp += "SouthWest block \n" + this->southWest->toTextSize();
	}

	return temp;
}

ObjectData* QuadTree::getObject()
{
	return this->Object;
}

void QuadTree::drawDebug(matrix pvw,Shader *shader)
{
	shader->setUniform("world", pvw.World);
	shader->setUniform("view", pvw.View);
	shader->setUniform("proj", pvw.Projection);
	glBindVertexArray(VA);

	glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);

	if (divided) {
		northEast->drawDebug(pvw, shader);
		northWest->drawDebug(pvw, shader);
		southEast->drawDebug(pvw, shader);
		southWest->drawDebug(pvw, shader);
	}
}

void QuadTree::drawObject(PlanePoints far, PlanePoints near,Shader *shader, float delta, float rotateAmount,bool culling,bool normal)
{
		if (full) {
			if (divided) {
				northEast->drawObject(far,near,shader, delta, rotateAmount, culling,normal);
				northWest->drawObject(far,near,shader, delta, rotateAmount, culling,normal);
				southWest->drawObject(far,near,shader, delta, rotateAmount, culling,normal);
				southEast->drawObject(far,near,shader, delta, rotateAmount, culling,normal);

			}
			else {
				if (!culling) {
					Object->draw(shader , delta, rotateAmount, normal);
				}
				else
				{
					if (inView(far, near)) {

						Object->draw(shader , delta, rotateAmount, normal);
						counter++;
					}


				}
			}
		}
}

void QuadTree::init()
{
	float x = boundary.x;
	float y = boundary.z;
	float width = boundary.width;
	float height = boundary.height;

	VertexLine line[] =
	{
		{glm::vec3( x-width, 1, y + height), glm::vec3(0,1,0)},
		{glm::vec3(x + width, 1, y + height), glm::vec3(0,1,0)},
		{glm::vec3(x + width, 1, y - height), glm::vec3(0,1,0)},
		{glm::vec3(x - width, 1, y - height), glm::vec3(0,1,0)}
	};
	unsigned int index[] = { 0,1,1,2,2,3,3,0};

	glGenVertexArrays(1, &VA);
	glBindVertexArray(VA);

	//Bind VertexBuffer & IndexBuffer
	glGenBuffers(1, &VB);
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glBufferData(GL_ARRAY_BUFFER, (4 * sizeof(VertexLine)), line, GL_STATIC_DRAW);

	glGenBuffers(1, &IB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (8 * sizeof(unsigned int)), index, GL_STATIC_DRAW);

	// write vertex data to memory
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLine), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLine), BUFFER_OFFSET(sizeof(float) * 3));

	if (divided) {
		northEast->init();
		northWest->init();
		southEast->init();
		southWest->init();
	}
}

void QuadTree::Subdivide()
{
	 float x, y, w, h;
	 x = this->boundary.x;
	 y = this->boundary.z;
	 w = this->boundary.width;
	 h = this->boundary.height;

	 rectrangle nw(x + w / 2, y - w / 2, w / 2, h / 2);
	 this->northWest = new QuadTree(nw);
	 rectrangle ne(x - w / 2, y - w / 2, w / 2, h / 2);
	 this->northEast = new QuadTree(ne);
	 rectrangle sw(x + w / 2, y + w / 2, w / 2, h / 2);
	 this->southWest = new QuadTree(sw);
	 rectrangle se(x - w / 2, y + w / 2, w / 2, h / 2);
	 this->southEast = new QuadTree(se);

	 this; divided = true;

}

bool QuadTree::inView(PlanePoints far, PlanePoints near)
 {

	 bool outsideRight = false;
	 bool outsideLeft = false;

	 //project a vector from bot of nearplane to centre of boundery on the normal. 
	 float sdistance = dot(normalize(far.rightNormal), glm::vec3(boundary.x, 0, boundary.z) - near.botRight);

	 if (sdistance <= -sqrt(pow(boundary.height,2)+ pow(boundary.width, 2)) && sdistance <= -sqrt(pow(boundary.height, 2) + pow(boundary.width, 2))) {
		outsideRight = true;		 
	 }


	 //project a vector from bot of nearplane to centre of boundery on the normal. 
	 sdistance = dot(normalize(far.leftNormal), glm::vec3(boundary.x, 0, boundary.z) - near.botLeft);

	//Pythagoras for quad offset. (So if the boundery is inside of frustom even though centre is outside of frustum.
	 if (sdistance <=  -sqrt(pow(boundary.height, 2) + pow(boundary.width, 2)) && sdistance <= -sqrt(pow(boundary.height, 2) + pow(boundary.width, 2))) {
		 outsideLeft = true;
	 }
	 if (outsideLeft || outsideRight) {
		 return false;
	 }
	 return true;
 }

