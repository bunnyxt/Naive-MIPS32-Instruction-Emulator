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
class GeneralPurposeRegisterSet;
class Alu;
class Cpu;

//IOHelper class
class IOHelper {
public:
	static void LoadCode(Memory &m, ifstream &in);
	static void LoadMemoryData(Memory &m, ifstream &in);
	static void LoadRegisterData(GeneralPurposeRegisterSet &gprs, ifstream &in);
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
public:
	Register();
	word Get();
	void Set(word w);

private:
	word data;
};

//GeneralPurposeRegisterSet class
class GeneralPurposeRegisterSet {
public:
	GeneralPurposeRegisterSet();
	word Get(int index);
	void Set(int index, word w);

private:
	Register gpr[32];
};

//Alu class
class Alu {

};

//Cpu class
class Cpu {
public:

private:
	GeneralPurposeRegisterSet gprs;
	Register pc;
};