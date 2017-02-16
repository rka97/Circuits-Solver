#ifndef NODE_H
#define NODE_H

#include <vector>
#include <string>

using namespace std;

class Element;

class Node {

private:
	bool isground;
	string name;		// unique property, each object has distinctive and unique name
	int id;				// a sequential ID number, might be useful when making the equation
	double voltage;
	vector<Element*>* elements; // too lazy to implement a linked list
								// each node is connected to one or more element, an element is a resistor or voltage/current source

public:
	// A constructor, same functionality as "init" functions
	Node(string name, int id);

	// connects an element to the node
	void addElement(Element* element);
	// getters and setters
	double getVoltage();
	void setVoltage(double voltage);
	int getNumOfElements();
	string getName();
	bool isGround();
	void setGround(bool isground);
	int getId();
	void setId(int id);
	vector<Element*>* getElements();
};

#endif
