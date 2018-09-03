#pragma once

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

//class declaration
class IOHelper;
class Memory;
class Register;

//IOHelper class
class IOHelper {
public:
	static bool LoadCode(Memory &m, ifstream &in);
	static bool LoadMemoryData(Memory &m, ifstream &in);
	static bool LoadRegisterData(Memory &m, ifstream &in);
};

//Memory class
class Memory {
public:
	Memory();
};

//Register class
class Register {

};