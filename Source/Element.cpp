#include "Element.h"
#include <cfloat>
#include "Node.h"

Element::Element(string name, ElementType type, double value) {
	this->id = -2;
	this->name = name;
	this->type = type;
	this->isenabled = true;
	this->resistance = 0;
	this->current = 0;
	this->voltage = 0;
	switch (type) {
	case CURRENT_SOURCE:
		this->current = value;
		break;
	case VOLTAGE_SOURCE:
		this->voltage = value;
		break;
	case RESISTOR:
		this->resistance = value;
		break;
	default:
		cout << "ERROR: TYPE UNDEFINED.\n";
		break;
	}
	this->pNode = NULL;
	this->nNode = NULL;
}

void Element::setVoltage(double voltage) {
	this->voltage = voltage;
}
void Element::setCurrent(double current) {
	this->current = current;
}
double Element::getVoltage() {
	if (this->isenabled == false) return DBL_MAX;
	if (getType() == Element::ElementType::VOLTAGE_SOURCE) return voltage;
	return this->pNode->getVoltage() - this->nNode->getVoltage();
}
double Element::getCurrent() {
	if (this->isenabled == false) return DBL_MAX;
	switch (this->type) {
	case Element::ElementType::RESISTOR:
		return -1 * getVoltage() / resistance;
	case Element::ElementType::CURRENT_SOURCE:
	case Element::ElementType::VOLTAGE_SOURCE:
		return current;
	default:
		return 0;
	}
}
double Element::getResistance() {
	return this->resistance;
}

double Element::getPower() {
	return 	-1 * getCurrent()*getVoltage();
}
int Element::getId() {

	return this->id;
}
Node* Element::getPosNode() {
	return this->pNode;
}
Node* Element::getNegNode() {
	return this->nNode;
}

Element::ElementType Element::getType() {
	return this->type;
}
string Element::getName() {
	return this->name;
}

void Element::setPosNode(Node* node) {
	this->pNode = node;

}
void Element::setNegNode(Node* node) {
	this->nNode = node;
}
void Element::setId(int id) {
	this->id = id;
}

// if node == pNode, return nNode, else if node == nNode return pNode, else return NULL
Node* Element::getTheOtherNode(Node* node) {
	if (node == pNode)
		return nNode;
	else
		if (node == nNode)
			return pNode;
		else
			return NULL;
}

bool Element::isEnabled() {
	return isenabled;
}

void Element::setEnabled(bool isenabled) {
	this->isenabled = isenabled;
}

void Element::setType(ElementType ET)
{
	this->type = ET;
}
