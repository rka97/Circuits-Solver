#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "Node.h"
#include "Element.h"
#include <vector>

/*
*	all interactions will be through this class, the user will know nothing about the other classes
*	and will interact only through the names of the elements/ndoes
*/


class Circuit {

private:

	vector<Node*>*		nodes;
	vector<Element*>*	elements;
	vector<Element*>*	voltageSources;

	int lastId;
	bool iscleaned;

	Node* getNode(string name);
	Node* getNode(int id);
	Element* getElement(string name);
	Element* getElement(int id);

	/*
	*	creates all of the equations that represent the circuit
	*	in the form Ax = B where A and B are matricies
	*	@param eqn : A
	*	@param vals : B
	*/
	bool createEquations(double*& eqs, double*& vals);

	/*
	*	creates the nodal equation of the node 'node' GV = I
	*	in the form Ax = B where A is a matrix with one row, B a value
	*	@param node : the node to be analyzed
	*	@param eqn : A
	*	@param val : B
	*/
	bool createEquation(Node* node, double* eqn, double& val);

	/*
	*	craetes the equation V2 - V1 = E of the voltage source
	*	in the form Ax = B where A is a matrix with one row, B a value
	*	@param vsource : the voltage source
	*	@param eqn : A
	*	@param val : B
	*/
	bool createEquation(Element* vsource, double* eqn, double& val);

	/*
	*	solves the system of linear equations Ax = B
	*	@param eqn : A
	*	@param vals : B
	*/
	bool solveEquations(double* eqn, double* vals);

	void deployResults(double* vals);

	bool _solve();


public:

	// a Constructor, same functionality as "init" functions
	Circuit();

	// solves the circuit and deploys the results
	bool solve();

	// adds an element with name "name", type "type" and value "value" to the node "nodename"
	bool addElement(string name, double value, string nodename, Element::ElementType et);

	// adds a node with name "name"
	bool addNode(string name);

	// gets current through element "name"
	double getCurrent(string name);

	// gets voltage across element or node "name"
	double getVoltage(string name);

	// gets the names of the nodes an element is connected across.
	void getNodeNames (string elementName, string& negNode, string& posNode);

	// solves due to sourcename
	bool solveDue(string sourcename);

	// gets power dissipated or supplied by an element
	double getPower(string name);

	// gets the resistance of an element.
	double getResistance(string name);

	// gets the number of voltage sources in the circuit.
	int getNumVoltageSources();

	// gets the maximum power transferred to the resistor and the value of the resistance in such case.
	double getMaxPower(string name, double& Rmax);

	// checks if the circuit's connections are correct.
	bool checkCircuit();

	// cleans up after superposition.
	void cleanUpSP();

	bool checkPowerBalance(double& dissipated, double& supplied);

};



#endif
