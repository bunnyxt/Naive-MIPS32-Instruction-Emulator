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

	bool IsReady(int index);
	void SetReady(int index);
	void SetNotReady(int index);
	void SetNewReady();
	bool HasWork();
	//void DropReady(int index);

	bool IsRunDone(int index);
	void SetRunDone(int index);
	void SetRunInterrupted(int index);
	void ResetRunStatus();

	GeneralPurposeRegisterSet& GetGeneralPurposeRegisterSet();
	void SetPc(address add);
	address GetPc();
	void SetIr(instruction i);
	instruction GetIr();

	//Caution! Not support lock 1 register twice!
	void LockReg(int index);
	void UnlockReg(int index);
	bool IsRegLocked(int index);

	bool IsFw0Vacant();
	void SetFw0Vacant();
	void SetFw0Value(word w);
	word GetFw0Value();
	void SetFw0Index(word w);
	word GetFw0Index();
	bool IsFw1Vacant();
	void SetFw1Vacant();
	void SetFw1Value(word w);
	word GetFw1Value();
	void SetFw1Index(word w);
	word GetFw1Index();

	Decoder GetDecoder();
	Alu GetAlu();

	void SetIfIdPc(address add);
	address GetIfIdPc();

	void SetIdExPc(address add);
	address GetIdExPc();
	void SetIdExTypeR(word w);
	word GetIdExTypeR();
	void SetIdExTypeI(word w);
	word GetIdExTypeI();
	void SetIdExTypeJ(word w);
	word GetIdExTypeJ();
	void SetIdExOp(word w);
	word GetIdExOp();
	void SetIdExRs(word w);
	word GetIdExRs();
	void SetIdExRt(word w);
	word GetIdExRt();
	void SetIdExRd(word w);
	word GetIdExRd();
	void SetIdExShamt(word w);
	word GetIdExShamt();
	void SetIdExFunc(word w);
	word GetIdExFunc();
	void SetIdExImmediate(word w);
	word GetIdExImmediate();
	void SetIdExAddress_(address add);
	address GetIdExAddress_();
	void SetIdExNeedLoad(word w);
	word GetIdExNeedLoad();
	void SetIdExNeedStore(word w);
	word GetIdExNeedStore();
	//void SetIdExAddress(address add);
	//address GetIdExAddress();
	void SetIdExRegValue(word w);
	word GetIdExRegValue();
	void SetIdExNeedWriteBack(word w);
	word GetIdExNeedWriteBack();
	void SetIdExIndex(word w);
	word GetIdExIndex();
	//void SetIdExWord(word w);
	//word GetIdExWord();

	void SetExMemNeedLoad(word w);
	word GetExMemNeedLoad();
	void SetExMemNeedStore(word w);
	word GetExMemNeedStore();
	void SetExMemAddress(address add);
	address GetExMemAddress();
	void SetExMemRegValue(word w);
	word GetExMemRegValue();
	void SetExMemNeedWriteBack(word w);
	word GetExMemNeedWriteBack();
	void SetExMemIndex(word w);
	word GetExMemIndex();
	void SetExMemWord(word w);
	word GetExMemWord();

	void SetMemIfAllow(word w);
	word GetMemIfAllow();
	void SetMemWbNeedWriteBack(word w);
	word GetMemWbNeedWriteBack();
	void SetMemWbIndex(word w);
	word GetMemWbIndex();
	void SetMemWbWord(word w);
	word GetMemWbWord();

private:
	Register isReady;
	Register runStatus;
	Register lockMarker;

	Register fw0_value;
	Register fw0_index;
	Register fw1_value;
	Register fw1_index;

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
	//Register id_ex_address;//l/s memory address
	Register id_ex_reg_value;//store to memory word reg value
	Register id_ex_need_write_back;//need write back
	Register id_ex_index;//write back reg index
	//Register id_ex_word;//write back word

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
};

class TestHelper {
public:
	void static ViewGeneralPurposeRegisterSet(Cpu cpu);
	void static ViewOtherRegisters(Cpu cpu);
	void static ViewMemoryUnitValue(Memory m, address a);
};