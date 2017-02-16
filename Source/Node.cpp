#include "Node.h"
#include "Element.h"
#include <string>

using namespace std;

// A constructor, same functionality as "init" functions
Node::Node(string name, int id){
	isground = false;
	this->name = name;		// unique property, each object has distinctive and unique name
	this->id = id;				// a sequential ID number, might be useful when making the equation
	this->voltage = 0;
	this->elements = new vector<Element*>(0);
}

// connects an element to the node
void Node::addElement(Element* element) {
	elements->push_back(element);
}

// getters and setters
double Node::getVoltage()
{
	return this->voltage;

}

void  Node::setVoltage(double voltage)
{
	this->voltage = voltage;
}

int Node::getNumOfElements()
{
	return elements->size();
}
string Node::getName()
{
	return this->name;
}
bool Node::isGround()
{
	return this->isground;
}
void Node::setGround(bool isground)
{
	this->isground = isground;
}

int Node::getId()
{
	return this->id;
}
void Node::setId(int id)
{
	this->id = id;
}
vector<Element*>* Node::getElements() {
	return elements;
}