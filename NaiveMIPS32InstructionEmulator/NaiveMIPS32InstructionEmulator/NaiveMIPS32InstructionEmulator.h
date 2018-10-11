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
class Decoder;
class Cpu;
class TestHelper;

//IOHelper class
class IOHelper {
public:
	static void LoadCode(Memory &m, ifstream &in);
	static void LoadMemoryData(Memory &m, ifstream &in);
	static void LoadRegisterData(Cpu &cpu, ifstream &in);
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
	static word GetSignExtendWord(word w, int length = 16);

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
public:
	Alu();
	word CalculateR(word rs, word rt, word shamt, word func);
	word CalculateI(word op, word rs, word rt, word immediate);
	word CalculateJ(word op, word address);
};

//Decoder class
class Decoder {
public:
	Decoder();
	word GetOp(instruction i);
	word GetRs(instruction i);
	word GetRt(instruction i);
	word GetRd(instruction i);
	word GetShamt(instruction i);
	word GetFunc(instruction i);
	word GetImmediate(instruction i);
	address GetAddress(instruction i);

private:
	unsigned int getSubNumInUnsignedInt(unsigned int ui, int start, int length);
};

//Cpu class
class Cpu {
public:
	Cpu();

	void Run(Memory &memory, bool &eoiDetected, int clockNumber);
	bool HasWork();

	//allow IOHelper & TestHelper class access to private member
	friend IOHelper;
	friend TestHelper;

private:
	Register isReady_;
	Register runStatus;
	Register lockMarker;

	Register fw0_value;
	Register fw0_index;
	Register fw1_value;
	Register fw1_index;
	Register fw2_value;
	Register fw2_index;

	Decoder decoder;
	Alu alu;

	GeneralPurposeRegisterSet gprs;
	Register pc;
	Register ir;

	Register if_id_pc;//pc of this instruction

	Register id_ex_pc;//pc of this instruction
	Register id_ex_type_R;//type R instruction
	Register id_ex_type_I;//type I instruction
	Register id_ex_type_J;//type J instruction
	Register id_ex_op;//op part
	Register id_ex_rs;//rs part
	Register id_ex_rt;//rt part
	Register id_ex_rd;//rd part
	Register id_ex_shamt;//shamt part
	Register id_ex_func;//func part
	Register id_ex_immediate;//immediate part
	Register id_ex_address_;//address part
	Register id_ex_need_load;//meed load
	Register id_ex_need_store;//need store
	Register id_ex_reg_value;//store to memory word reg value
	Register id_ex_need_write_back;//need write back
	Register id_ex_index;//write back reg index

	Register ex_mem_need_load;//need load
	Register ex_mem_need_store;//nees store
	Register ex_mem_address;//l/s memory address
	Register ex_mem_reg_value;//store to memory word reg
	Register ex_mem_need_write_back;//need write back
	Register ex_mem_index;//write back index
	Register ex_mem_word;//write back word

	Register mem_if_allow;//allow if
	Register mem_wb_need_write_back;//need write back
	Register mem_wb_index;//write back reg index
	Register mem_wb_word;//write back word

	bool isReady(int index);
	void setReady(int index);
	void setNotReady(int index);
	void setNewReady();

	bool isRunDone(int index);
	void setRunDone(int index);
	void setRunInterrupted(int index);
	void resetRunStatus();

	GeneralPurposeRegisterSet& getGeneralPurposeRegisterSet();
	void setPc(address add);
	address getPc();
	void setIr(instruction i);
	instruction getIr();

	//Caution! Not support lock 1 register twice!
	void lockReg(int index);
	void unlockReg(int index);
	bool isRegLocked(int index);

	bool isFw0Vacant();
	void setFw0Vacant();
	void setFw0Value(word w);
	word getFw0Value();
	void setFw0Index(word w);
	word getFw0Index();
	bool isFw1Vacant();
	void setFw1Vacant();
	void setFw1Value(word w);
	word getFw1Value();
	void setFw1Index(word w);
	word getFw1Index();
	bool isFw2Vacant();
	void setFw2Vacant();
	void setFw2Value(word w);
	word getFw2Value();
	void setFw2Index(word w);
	word getFw2Index();

	Decoder getDecoder();
	Alu getAlu();

	void setIfIdPc(address add);
	address getIfIdPc();

	void setIdExPc(address add);
	address getIdExPc();
	void setIdExTypeR(word w);
	word getIdExTypeR();
	void setIdExTypeI(word w);
	word getIdExTypeI();
	void setIdExTypeJ(word w);
	word getIdExTypeJ();
	void setIdExOp(word w);
	word getIdExOp();
	void setIdExRs(word w);
	word getIdExRs();
	void setIdExRt(word w);
	word getIdExRt();
	void setIdExRd(word w);
	word getIdExRd();
	void setIdExShamt(word w);
	word getIdExShamt();
	void setIdExFunc(word w);
	word getIdExFunc();
	void setIdExImmediate(word w);
	word getIdExImmediate();
	void setIdExAddress_(address add);
	address getIdExAddress_();
	void setIdExNeedLoad(word w);
	word getIdExNeedLoad();
	void setIdExNeedStore(word w);
	word getIdExNeedStore();
	void setIdExRegValue(word w);
	word getIdExRegValue();
	void setIdExNeedWriteBack(word w);
	word getIdExNeedWriteBack();
	void setIdExIndex(word w);
	word getIdExIndex();

	void setExMemNeedLoad(word w);
	word getExMemNeedLoad();
	void setExMemNeedStore(word w);
	word getExMemNeedStore();
	void setExMemAddress(address add);
	address getExMemAddress();
	void setExMemRegValue(word w);
	word getExMemRegValue();
	void setExMemNeedWriteBack(word w);
	word getExMemNeedWriteBack();
	void setExMemIndex(word w);
	word getExMemIndex();
	void setExMemWord(word w);
	word getExMemWord();

	void setMemIfAllow(word w);
	word getMemIfAllow();
	void setMemWbNeedWriteBack(word w);
	word getMemWbNeedWriteBack();
	void setMemWbIndex(word w);
	word getMemWbIndex();
	void setMemWbWord(word w);
	word getMemWbWord();
};

class TestHelper {
public:
	void static ViewGeneralPurposeRegisterSet(Cpu cpu);
	void static ViewOtherRegisters(Cpu cpu);
	void static ViewMemoryUnitValue(Memory m, address a);
};