#include "Circuit.h"
#include <cfloat>
#include <cstdlib>
#include <iostream>
#include "Element.h"
#include "Eigen/Dense"

using namespace std;

// adds a node with name "name"
bool Circuit::addNode(string name) {
	if (getNode(name) != NULL)
		return false;

	if (nodes->size() == 0) {
		lastId = -1;
	}
	Node* tNode = new Node(name, this->lastId);
	if (lastId == -1) {
		tNode->setGround(true);
	}
	this->lastId++;
	this->nodes->push_back(tNode);
	return true;
}

// gets current through element "name"
double Circuit::getCurrent(string name) {
	Element* tElement = getElement(name);
	if (tElement == NULL)
		return DBL_MAX;
	return tElement->getCurrent();
}
// gets voltage across element or node "name"
double Circuit::getVoltage(string name) {
	Element* tElement = getElement(name);
	if (tElement == NULL) {
		Node* node = getNode(name);
		if (node != NULL)
			return node->getVoltage();
		else return DBL_MAX;
	}
	else {
		return tElement->getVoltage();
	}
}
double Circuit::getResistance(string name) {
	Element* tElement = getElement(name);
	if (tElement == NULL)
		return -1;
	return tElement->getResistance();
}

Node* Circuit::getNode(string name) {
	for (vector<Node*>::iterator it = this->nodes->begin(); it != nodes->end(); it++) {
		if ((*it)->getName() == name) return (*it);
	}
	return NULL;
}
Node* Circuit::getNode(int id) {
	for (vector<Node*>::iterator it = this->nodes->begin(); it != nodes->end(); it++) {
		if ((*it)->getId() == id) return (*it);
	}
	return NULL;
}
Element* Circuit::getElement(int id) {
	for (vector<Element*>::iterator it = this->elements->begin(); it != elements->end(); it++) {
		if ((*it)->getId() == id) return (*it);
	}
	for (vector<Element*>::iterator it = this->voltageSources->begin(); it != voltageSources->end(); it++) {
		if ((*it)->getId() == id) return (*it);
	}
	return NULL;
}
Element* Circuit::getElement(string name) {
	for (vector<Element*>::iterator it = this->elements->begin(); it != elements->end(); it++) {
		if ((*it)->getName() == name) return (*it);
	}
	for (vector<Element*>::iterator it = this->voltageSources->begin(); it != voltageSources->end(); it++) {
		if ((*it)->getName() == name) return (*it);
	}
	return NULL;
}

void Circuit::getNodeNames (string elementName, string& negNode, string& posNode) {
	Element* tElem = this->getElement(elementName);
	if (tElem == NULL) {
		negNode = "0";
		posNode = "0";
		return;
	}
	negNode = (tElem->getNegNode())->getName();
	posNode = (tElem->getPosNode())->getName();
}

bool Circuit::solveEquations(double* eqn, double* vals) {
	int numofeqs = nodes->size() - 1 + voltageSources->size();
	Eigen::Map < Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> > A(eqn, numofeqs, numofeqs);

	Eigen::Map<Eigen::VectorXd> b(vals, numofeqs);
	Eigen::VectorXd x = A.colPivHouseholderQr().solve(b);
	double relative_error = (A*x - b).norm() / b.norm();

	if (relative_error > 0.1) {
		cout << "ERROR: Invalid circuit, either two different voltage sources in parallel or two different current sources in series, or "
			<< " source is short-circuited.\n";
		return false;
	}


	for (int i = 0; i < numofeqs; i++) vals[i] = x[i];

	return true;
}

void Circuit::deployResults(double* vals) {
	int n = voltageSources->size() + nodes->size() - 1;
	for (int i = 0; i < n; i++) {
		Node* tNode = getNode(i);
		if (tNode != NULL)
			tNode->setVoltage(vals[i]);
		else
		{
			Element* tElem = getElement(i);
			if (tElem != NULL) {
				tElem->setCurrent(vals[i]);
			}
		}
	}
}


Circuit::Circuit() {

	nodes = new vector<Node*>(0);
	elements = new vector<Element*>(0);
	voltageSources = new vector<Element*>(0);
	lastId = 0;
	iscleaned = true;
}


bool Circuit::_solve() {
	double* eqn = NULL;
	double* vals = NULL;

	createEquations(eqn, vals);

	if (!solveEquations(eqn, vals))
		return false;

	deployResults(vals);

	return true;
}

bool Circuit::solve() {
	if (!iscleaned)
		cleanUpSP();
	return this->_solve();
}

bool Circuit::createEquations(double*& eqs, double*& vals) {
	int n = voltageSources->size() + nodes->size() - 1;
	//cout << endl << endl << n << endl << endl;
	eqs = new double[n*n];
	vals = new double[n];

	for (int i = 0; i < n; i++) {
		vals[i] = 0;
		for (int j = 0; j < n; j++) {
			eqs[i + n*j] = 0;
		}
	}

	int i = 0;
	for (vector<Node*>::iterator it = nodes->begin(); it != nodes->end(); it++) {
		if ((*it)->isGround()) continue;
		createEquation((*it), (eqs + n*i), vals[i]);
		i++;
	}

	for (vector<Element*>::iterator it = voltageSources->begin(); it != voltageSources->end(); it++) {
		createEquation((*it), (eqs + n*i), vals[i]);
		i++;
	}

	return true;
}

bool Circuit::createEquation(Element* vsource, double* eqn, double& val) {
	if(!vsource->getPosNode()->isGround())
		eqn[vsource->getPosNode()->getId()] = 1;
	if (!vsource->getNegNode()->isGround())
		eqn[vsource->getNegNode()->getId()] = -1;
	if (vsource->isEnabled())
		val = vsource->getVoltage();
	else
		val = 0;
	return true;
}


bool Circuit::createEquation(Node* node, double* eqn, double& val) {
	for (vector<Element*>::iterator it = node->getElements()->begin(); it != node->getElements()->end(); it++) {
		double x;
		switch ((*it)->getType()) {
		case Element::ElementType::RESISTOR:
			x = 1 / (*it)->getResistance();
			eqn[node->getId()] += x;
			if (!(*it)->getTheOtherNode(node)->isGround())
				eqn[(*it)->getTheOtherNode(node)->getId()] -= x;
			break;
		case Element::ElementType::CURRENT_SOURCE:
			if ((*it)->isEnabled()) {
				if ((*it)->getPosNode() == node) {
					x = (*it)->getCurrent();
				}
				else {
					x = -(*it)->getCurrent();
				}
			}
			else {
				x = 0;
			}
			val += x;
			break;
		case Element::ElementType::VOLTAGE_SOURCE:
			if ((*it)->getPosNode() == node) {
				x = -1;
			}
			else x = 1;
			eqn[(*it)->getId()] += x;
			break;
		case Element::ElementType::ERROR:
			return false;
			break;
		}
	}
	return true;
}


// adds an element with name "name", type "type" and value "value" to the node "nodename"
bool Circuit::addElement(string name, double value, string nodename, Element::ElementType et) {
	Node* n = getNode(nodename);

	if (n == NULL || (et == Element::ElementType::RESISTOR && value <= 0) || et == Element::ElementType::ERROR) {
		return false;
	}

	Element* e = getElement(name);

	if (e == NULL) {
		e = new Element(name, et, value);
		if (e->getType() == Element::ElementType::VOLTAGE_SOURCE) {
			e->setId(lastId);
			lastId++;
			this->voltageSources->push_back(e);
		}
		else {
			this->elements->push_back(e);
		}
	}
	else {
		if (et != e->getType()) {
			cout << "ERROR: Type mismatch on adding element: " << name << ".\n";
			return false;
		}
	}

	if (e->getPosNode() == NULL) {
		e->setPosNode(n);
	}
	else if (e->getPosNode() == n) {
		cout << "ERROR: Trying to enter element [" << e->getName() << "] twice into the same node. Please re-enter. \n";
		return false;
	}
	else if (e->getNegNode() == NULL) {
		switch (e->getType()) {
		case Element::ElementType::VOLTAGE_SOURCE:
			if (e->getVoltage() != -1*value)
				return false;
			break;
		case Element::ElementType::CURRENT_SOURCE:
			if (e->getCurrent() != -1*value)
				return false;
			break;
		default:
			break;
		}
		e->setNegNode(n);
	}
	else {
		cout << "ERROR: element " << name << " already exists.\n";
	}
	n->addElement(e);

	return true;
}


double Circuit::getPower(string name)
{
	Element* telement = getElement(name);
	if (telement == NULL)
		return 0;
	else
		return telement->getPower();
}

bool Circuit::solveDue(string sourcename) {
	Element* source = getElement(sourcename);
	if (source == NULL || source->getType() == Element::ElementType::RESISTOR) {
		cout << sourcename << " does not exist or is not a source.\n";
		return false;
	}
	if (!iscleaned)
		cleanUpSP();

	for (vector<Element*>::iterator it = elements->begin(); it != elements->end(); it++) {
		if ((*it)->getType() != Element::ElementType::RESISTOR) {
			if ((*it) != source) (*it)->setEnabled(false);
		}
	}

	for (vector<Element*>::iterator it = voltageSources->begin(); it != voltageSources->end(); it++) {
		if ((*it) != source) (*it)->setEnabled(false);
	}

	bool success = _solve();

	this->iscleaned = false;
	return success;

}

void Circuit::cleanUpSP () {
	for (vector<Element*>::iterator it = elements->begin(); it != elements->end(); it++) {
		if ((*it)->getType() != Element::ElementType::RESISTOR) {
			(*it)->setEnabled(true);
		}
	}

	for (vector<Element*>::iterator it = voltageSources->begin(); it != voltageSources->end(); it++) {
		(*it)->setEnabled(true);
	}
	this->iscleaned = true;
}


bool Circuit::checkCircuit() {
	// check empty nodes
	for (vector<Node*>::iterator it = nodes->begin(); it != nodes->end(); it++) {
		if ((*it)->getNumOfElements() < 2)
		{
			cout << "ERROR: Node [" << (*it)->getName() << "] is connected to less than two elements, please enter other elements.\n";
			return false;
		}
	}
	// check voltage sources
	for (vector<Element*>::iterator it = voltageSources->begin(); it != voltageSources->end(); it++) {
		if ((*it)->getPosNode() == NULL || (*it)->getNegNode() == NULL)
		{
			cout << "ERROR: Voltage Source [" << (*it)->getName() << "] is connected to less than two nodes, please enter the other end.\n";
			return false;
		}
	}
	// check other elements
	for (vector<Element*>::iterator it = elements->begin(); it != elements->end(); it++) {
		if ((*it)->getPosNode() == NULL || (*it)->getNegNode() == NULL)
		{
			cout << "ERROR: Element [" << (*it)->getName() << "] is connected to less than two nodes, please enter the other end.\n";
			return false;
		}
	}
	return true;
}

int Circuit::getNumVoltageSources() {
	return voltageSources->size();
}

double Circuit::getMaxPower(string name, double& Rmax)
{
	Element* telement = getElement(name);
	if (telement == NULL || telement->getType() != Element::ElementType::RESISTOR)
        return DBL_MAX;
	telement->setType(Element::ElementType::CURRENT_SOURCE);

	telement->setCurrent(0);
	solve();
	double Vth = telement->getVoltage();
	telement->setType(Element::ElementType::VOLTAGE_SOURCE);
	telement->setId(lastId);
	telement->setVoltage(1);
	voltageSources->push_back(telement);
	solveDue(telement->getName());
	cleanUpSP();
	double i = telement->getCurrent();
	telement->setType(Element::ElementType::RESISTOR);
	voltageSources->pop_back();
	telement->setId(-1);

	if (i == 0)	return DBL_MAX;
	Rmax = 1.0 / i;

	return (Vth*Vth) / (4 * Rmax);
}

bool Circuit::checkPowerBalance(double& dissipated, double& supplied) {
	dissipated = 0;
	supplied = 0;
	double p;
	for (vector<Element*>::iterator it = elements->begin(); it != elements->end(); it++) {
		p = (*it)->getPower();
		if (p > 0)	dissipated += p;
		else		supplied -= p;
	}
	for (vector<Element*>::iterator it = voltageSources->begin(); it != voltageSources->end(); it++) {
		p = (*it)->getPower();
		if (p > 0)	dissipated += p;
		else		supplied -= p;
	}
	return ( ( dissipated - supplied ) / supplied) < 0.01;
}
