#pragma once
#pragma warning (disable : 4996)

#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <stdlib.h>

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
class ConvertHelper;
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
	static bool OpenLogFileStream();
	static bool WriteLog(string s);
	static bool CloseLogFileStream();

private:
	static ofstream log;
};


//ConvertHelper
class ConvertHelper {
public:
	static string InstructionToString(instruction i);
	static string AddressToString(address a);
	static string WordToString(word w);
	static string SeperateString(string s, int interval = 4, char seperator = ' ');

private:
	static string unsignedIntToString(unsigned int ui);
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
	Cpu();
	GeneralPurposeRegisterSet& GetGeneralPurposeRegisterSet();

private:
	GeneralPurposeRegisterSet gprs;
	Register pc;
};