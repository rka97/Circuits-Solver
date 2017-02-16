#ifndef ELEMENT_H
#define ELEMENT_H

#include <string>
#include <iostream>
using namespace std;

class Node;

/*
*	An element is any component in the circuit (resistor, current source, voltage source)
*	Every element has two terminals pNode (positive terminal) and nNode (negative terminal)
*	value is the resistance in case of a resistor, current in case of a current source
*	and voltage in case of voltage source
*	Conventions used :
*	1 - in case of a current source, "value" represents the current going from nNode to pNode
*	2 - in case of a voltage source, "value" represents the voltage difference of pNode - nNode
*/

class Element {

public: enum ElementType {
	RESISTOR, CURRENT_SOURCE, VOLTAGE_SOURCE, ERROR
};

private:
	Node* pNode;
	Node* nNode;
	double voltage;
	double current;
	double resistance;
	ElementType type;
	string name;	// unique property, each object has distinctive and unique name
	int id;			// a sequential ID number, might be useful when making the equation
	bool isenabled;

public:
	// a constructor. same functionality as init functions in the last project
	Element(string name, ElementType type, double value);

	//getters and setters
	double getVoltage();		// get the voltage across the element
	double getCurrent();		// get the current running through the element
	double getResistance();
	double getPower();
	int getId();
	Node* getPosNode();
	Node* getNegNode();
	ElementType getType();
	string getName();
	bool isEnabled();

	void setPosNode(Node* node);
	void setNegNode(Node* node);
	void setId(int id);
	void setVoltage(double voltage);
	void setCurrent(double current);
	void setEnabled(bool isenabled);

	// if node == pNode, return nNode, else if node == nNode return pNode, else return NULL
	Node* getTheOtherNode(Node* node);
	// sets the type of elements
	void setType(ElementType ET);

};
#endif
