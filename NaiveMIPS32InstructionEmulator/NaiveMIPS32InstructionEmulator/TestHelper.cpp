#include "NaiveMIPS32InstructionEmulator.h"

void TestHelper::ViewGeneralPurposeRegisterSet(Cpu cpu)
{
	GeneralPurposeRegisterSet gprs = cpu.GetGeneralPurposeRegisterSet();
	for (int i = 0; i < 32; i++)
	{
		cout << "r" << i << " : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(gprs.Get(i))) << "(" << gprs.Get(i) << ")" << endl;
		IOHelper::WriteLog("r" + to_string(i) + " : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(gprs.Get(i))) + "(" + to_string(gprs.Get(i)) + ")");
	}
}

void TestHelper::ViewOtherRegisters(Cpu cpu)
{
	cout << "PC : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetPc())) << "(" << cpu.GetPc() << ")" << endl;
	IOHelper::WriteLog("PC : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetPc())) + "(" + to_string(cpu.GetPc()) + ")");
	cout << "IR : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIr())) << "(" << cpu.GetIr() << ")" << endl;
	IOHelper::WriteLog("IR : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIr())) + "(" + to_string(cpu.GetIr()) + ")");

	cout << "IdExTypeR : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExTypeR())) << "(" << cpu.GetIdExTypeR() << ")" << endl;
	IOHelper::WriteLog("IdExTypeR : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExTypeR())) + "(" + to_string(cpu.GetIdExTypeR()) + ")");
	cout << "IdExTypeI : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExTypeI())) << "(" << cpu.GetIdExTypeI() << ")" << endl;
	IOHelper::WriteLog("IdExTypeI : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExTypeI())) + "(" + to_string(cpu.GetIdExTypeI()) + ")");
	cout << "IdExTypeJ : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExTypeJ())) << "(" << cpu.GetIdExTypeJ() << ")" << endl;
	IOHelper::WriteLog("IdExTypeJ : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExTypeJ())) + "(" + to_string(cpu.GetIdExTypeJ()) + ")");
	cout << "IdExOp : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExOp())) << "(" << cpu.GetIdExOp() << ")" << endl;
	IOHelper::WriteLog("IdExOp : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExOp())) + "(" + to_string(cpu.GetIdExOp()) + ")");
	cout << "IdExRs : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExRs())) << "(" << cpu.GetIdExRs() << ")" << endl;
	IOHelper::WriteLog("IdExRs : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExRs())) + "(" + to_string(cpu.GetIdExRs()) + ")");
	cout << "IdExRt : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExRt())) << "(" << cpu.GetIdExRt() << ")" << endl;
	IOHelper::WriteLog("IdExRt : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExRt())) + "(" + to_string(cpu.GetIdExRt()) + ")");
	cout << "IdExRd : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExRd())) << "(" << cpu.GetIdExRd() << ")" << endl;
	IOHelper::WriteLog("IdExRd : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExRd())) + "(" + to_string(cpu.GetIdExRd()) + ")");
	cout << "IdExShamt : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExShamt())) << "(" << cpu.GetIdExShamt() << ")" << endl;
	IOHelper::WriteLog("IdExShamt : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExShamt())) + "(" + to_string(cpu.GetIdExShamt()) + ")");
	cout << "IdExFunc : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExFunc())) << "(" << cpu.GetIdExFunc() << ")" << endl;
	IOHelper::WriteLog("IdExShamt : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExFunc())) + "(" + to_string(cpu.GetIdExFunc()) + ")");
	cout << "IdExImmediate : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExImmediate())) << "(" << cpu.GetIdExImmediate() << ")" << endl;
	IOHelper::WriteLog("IdExImmediate : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExImmediate())) + "(" + to_string(cpu.GetIdExImmediate()) + ")");
	cout << "IdExAddress_ : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExAddress_())) << "(" << cpu.GetIdExAddress_() << ")" << endl;
	IOHelper::WriteLog("IdExAddress_ : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExAddress_())) + "(" + to_string(cpu.GetIdExAddress_()) + ")");
	cout << "IdExNeedLoad : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExNeedLoad())) << "(" << cpu.GetIdExNeedLoad() << ")" << endl;
	IOHelper::WriteLog("IdExNeedLoad : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExNeedLoad())) + "(" + to_string(cpu.GetIdExNeedLoad()) + ")");
	cout << "IdExNeedStore : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExNeedStore())) << "(" << cpu.GetIdExNeedStore() << ")" << endl;
	IOHelper::WriteLog("IdExNeedStore : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExNeedStore())) + "(" + to_string(cpu.GetIdExNeedStore()) + ")");
	cout << "IdExRegValue : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExRegValue())) << "(" << cpu.GetIdExRegValue() << ")" << endl;
	IOHelper::WriteLog("IdExRegValue : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExRegValue())) + "(" + to_string(cpu.GetIdExRegValue()) + ")");
	cout << "IdExNeedWriteBack : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExNeedWriteBack())) << "(" << cpu.GetIdExNeedWriteBack() << ")" << endl;
	IOHelper::WriteLog("IdExNeedWriteBack : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExNeedWriteBack())) + "(" + to_string(cpu.GetIdExNeedWriteBack()) + ")");
	cout << "IdExIndex : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExIndex())) << "(" << cpu.GetIdExIndex() << ")" << endl;
	IOHelper::WriteLog("IdExIndex : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetIdExIndex())) + "(" + to_string(cpu.GetIdExIndex()) + ")");

	cout << "ExMemNeedLoad : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetExMemNeedLoad())) << "(" << cpu.GetExMemNeedLoad() << ")" << endl;
	IOHelper::WriteLog("ExMemNeedLoad : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetExMemNeedLoad())) + "(" + to_string(cpu.GetExMemNeedLoad()) + ")");
	cout << "ExMemNeedStore : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetExMemNeedStore())) << "(" << cpu.GetExMemNeedStore() << ")" << endl;
	IOHelper::WriteLog("ExMemNeedStore : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetExMemNeedStore())) + "(" + to_string(cpu.GetExMemNeedStore()) + ")");
	cout << "ExMemAddress : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetExMemAddress())) << "(" << cpu.GetExMemAddress() << ")" << endl;
	IOHelper::WriteLog("ExMemAddress : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetExMemAddress())) + "(" + to_string(cpu.GetExMemAddress()) + ")");
	cout << "ExMemRegValue : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetExMemRegValue())) << "(" << cpu.GetExMemRegValue() << ")" << endl;
	IOHelper::WriteLog("ExMemRegValue : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetExMemRegValue())) + "(" + to_string(cpu.GetExMemRegValue()) + ")");
	cout << "ExMemNeedWriteBack : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetExMemNeedWriteBack())) << "(" << cpu.GetExMemNeedWriteBack() << ")" << endl;
	IOHelper::WriteLog("ExMemNeedWriteBack : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetExMemNeedWriteBack())) + "(" + to_string(cpu.GetExMemNeedWriteBack()) + ")");
	cout << "ExMemIndex : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetExMemIndex())) << "(" << cpu.GetExMemIndex() << ")" << endl;
	IOHelper::WriteLog("ExMemIndex : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetExMemIndex())) + "(" + to_string(cpu.GetExMemIndex()) + ")");
	cout << "ExMemWord : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetExMemWord())) << "(" << cpu.GetExMemWord() << ")" << endl;
	IOHelper::WriteLog("ExMemWord : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetExMemWord())) + "(" + to_string(cpu.GetExMemWord()) + ")");

	cout << "MemIfAllow : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetMemIfAllow())) << "(" << cpu.GetMemIfAllow() << ")" << endl;
	IOHelper::WriteLog("MemIfAllow : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetMemIfAllow())) + "(" + to_string(cpu.GetMemIfAllow()) + ")");
	cout << "MemWbNeedWriteBack : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetMemWbNeedWriteBack())) << "(" << cpu.GetMemWbNeedWriteBack() << ")" << endl;
	IOHelper::WriteLog("MemWbNeedWriteBack : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetMemWbNeedWriteBack())) + "(" + to_string(cpu.GetMemWbNeedWriteBack()) + ")");
	cout << "MemWbIndex : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetMemWbIndex())) << "(" << cpu.GetMemWbIndex() << ")" << endl;
	IOHelper::WriteLog("MemWbIndex : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetMemWbIndex())) + "(" + to_string(cpu.GetMemWbIndex()) + ")");
	cout << "MemWbWord : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetMemWbWord())) << "(" << cpu.GetMemWbWord() << ")" << endl;
	IOHelper::WriteLog("MemWbWord : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetMemWbWord())) + "(" + to_string(cpu.GetMemWbWord()) + ")");
}

void TestHelper::ViewMemoryUnitValue(Memory m, address a)
{
	//remodify address
	a %= MEMSIZE;

	//align address
	a -= a % 4;

	address start = (a < 40) ? 0 : (a - 40);
	address end = (a + 40 > MEMSIZE - 4) ? MEMSIZE - 4 : a + 40;

	for (address i = start; i <= end; i += 4)
	{
		cout << "memory address " << i << " : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(m.ReadWord(i))) << "(" << m.ReadWord(i) << ")" << endl;
		IOHelper::WriteLog("memory address " + to_string(i) + " : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(m.ReadWord(i))) + "(" + to_string(m.ReadWord(i)) + ")");
	}
}
