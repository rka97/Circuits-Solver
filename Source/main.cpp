#include "IO.h"
#include <iostream>
#include "Circuit.h"
using namespace std;

int main() {

	Circuit* c = new Circuit();

	inputValues(c);

	if (c->solve()) {
		cout << "\n\nFor direct responses, please enter the type (I current, V voltage, and P for power) " <<
					"and location (element name/number) of the required response.\n";
		cout << "For superposition, press EN/JN where N is the number of the voltage/current source first.\n";
		cout << "For maximum power transfer, press MP/RM/PM followed by the name of the resistor.\n";
		cout << "Press Q/q to exit.\n";
		double supplied, dissipated;
		bool isbalanced = c->checkPowerBalance(dissipated, supplied);
		cout << "Power supplied = " << supplied << " watts. \nPower dissipated = " << dissipated << " watts. \n\n";
		if (isbalanced)
			cout << "\n\nPower is balanced.\n";
		else
			cout << "\n\nERROR: Power is NOT balanced. \n";
		while (true) {
			string responseType;
			cin >> responseType;
			if (responseType[0] == 'Q' || responseType[0] == 'q') {
				break;
			}
			string responseName;
			cin >> responseName;

			if (responseType[0] == 'E' || responseType[0] == 'J') {
				//Superposition
				c->solveDue(responseType);
				string responseName2;
				cin >> responseName2;
				printValue (responseName2, c, responseName[0]);
			}
			else if (responseType == "MP" || responseType == "RM" || responseType == "PM") {
				// MPT
				double RMax;
				double PMax = c->getMaxPower(responseName, RMax);
				if (PMax != DBL_MAX) {
					cout << "Maximum power transfer to resistor " << responseName << " = " << PMax << " watts at Rmax = " << RMax << " ohms. \n";
				}
				else {
					cout << "ERROR: " << responseName << " either is not a resistor, causes an invalid circuit or the maximum power tends to infinity. \n";
				}
			}
			else {
				c->solve();
				printValue (responseName, c, responseType[0]);
			}
		}
	}

	return 0;
}
