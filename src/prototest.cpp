//============================================================================
// Name        : prototest.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdlib.h>
#include "RunnableManager.h"
#include "ModuleA.h"
#include "ModuleB.h"
using namespace std;


int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	getRunnableManager().runAll();
	cout << "Done!" << endl;
	return 0;
}
