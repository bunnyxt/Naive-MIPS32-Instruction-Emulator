#pragma once

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

//type definition
typedef unsigned int instruction;
typedef unsigned int address;
typedef unsigned int word;
typedef unsigned char byte;

//const declaration
const int MEMSIZE = 4096;//4096 = 4KB

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
	word ReadWord(address add);
	byte ReadByte(address add);
	void WriteWord(address add, word w);
	void WriteByte(address add, byte b);

private:
	byte data[MEMSIZE];
};

//Register class
class Register {

};