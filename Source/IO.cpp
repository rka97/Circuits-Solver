#include "IO.h"

// workaround for MINGW's absence of to_string
#ifdef __MINGW32__
#include <sstream>
template <typename T>
string to_string(const T& value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}
#endif

void inputValues (Circuit* c) {
	cout << "Please enter the number of nodes (must be greater than 1): ";
	int n = 0;
	do {
		cin >> n;
	} while (n <= 1);

	cout << "Code: [R]esistor, [E] Voltage Source, [J] Current Source.\n";
	do {
		for (int i = 0; i < n; i++) {
			c->addNode(to_string(i));
			cout << "Please enter all elements connected to Node " << i << " and press any character other than R/E/J when finished.\n";
			string elemType;
			double value;
			while (true) {
				cin >> elemType;
				elemType[0] = toupper(elemType[0]);
				if (elemType[0] != 'R' && elemType[0] != 'E' && elemType[0] != 'J')
					break;
				cin >> value;
				Element::ElementType et = createType(elemType[0], value);
				if (et == Element::ElementType::ERROR) {
					cout << "Unrecognized type.\n";
					break;
				}
				if (elemType[1] < 0) {
					cout << "ERROR: negative-valued index impossible.\n";
					break;
				}
				if (!c->addElement(elemType, value, to_string(i), et)) {
					cout << "Element addition failed, please re-add element.\n";
				}
			}
		}
	} while (!(c->checkCircuit()));
}

void printValue (string responseName, Circuit* c, char responseType) {
	double v1 = c->getVoltage(responseName);
	if (v1 == DBL_MAX) {
		cout << "Error: " << responseName << " does not exist in the current circuit. \n";
		return;
	}

	string negNode, posNode, responseName2;
	double v2;

	switch (responseType) {
	case 'I':
	case 'i':
		c->getNodeNames(responseName, negNode, posNode);
		cout << "Current through: " << responseName << " " << c->getCurrent(responseName)
				<< " amperes from Node[" << negNode << "] to Node[" << posNode << "]. \n";
		break;
	case 'V':
	case 'v':
		if (responseName[0] == 'E' || responseName[0] == 'J' || responseName[0] == 'R') {
			c->getNodeNames(responseName, negNode, posNode);
			cout << "Voltage across " << responseName << " = " << v1
					<< " volts from Node[" << posNode << "] to Node [" << negNode << "].\n";
		}
		else {
			cin >> responseName2;
			v2 = c->getVoltage(responseName2);
			if (v2 == DBL_MAX)
			{
				cout << "Node does not exist.\n";
				break;
			}
			cout << "Potential difference between Node [" << responseName << "] and [" << responseName2 << "] = " <<
				v1 - v2 << " volts. \n";
		}
		break;
	case 'P':
	case 'p':
		if (responseName[0] == 'E' || responseName[0] == 'J' || responseName[0] == 'R') {
				cout << "Power in " << responseName << " = " << c->getPower(responseName) << " watts. \n";
		}
		else {
			cout << "ERROR: Can not get power in a node.\n";
		}
		break;
	default:
		cout << "Unrecognized response type. Please re-enter either I/i for current, V/v for voltage and P/p for power. \n";
		break;
	}
}

Element::ElementType createType (char type, double value) {
	Element::ElementType et;
	switch (tolower(type)) {
	case 'j':
		et = Element::ElementType::CURRENT_SOURCE;
		break;
	case 'e':
		et = Element::ElementType::VOLTAGE_SOURCE;
		break;
	case 'r':
		if (value < 0)
			return Element::ElementType::ERROR;
		et = (value == 0) ? Element::ElementType::VOLTAGE_SOURCE : Element::ElementType::RESISTOR;
		break;
	default:
		return Element::ElementType::ERROR;
	}
	return et;
}
