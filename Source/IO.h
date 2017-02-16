#include <iostream>
#include <string>
#include <cfloat>
#include "Circuit.h"
using namespace std;

void inputValues (Circuit* c);
void printValue (string responseName, Circuit* c, char responseType);
Element::ElementType createType (char type, double value);
