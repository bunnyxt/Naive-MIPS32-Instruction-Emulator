#include "NaiveMIPS32InstructionEmulator.h"

void TestHelper::ViewGeneralPurposeRegisterSet(Cpu cpu)
{
	GeneralPurposeRegisterSet gprs = cpu.getGeneralPurposeRegisterSet();
	for (int i = 0; i < 32; i++)
	{
		cout << "r" << i << " : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(gprs.Get(i))) << "(" << gprs.Get(i) << ")" << endl;
		IOHelper::WriteLog("r" + to_string(i) + " : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(gprs.Get(i))) + "(" + to_string(gprs.Get(i)) + ")");
	}
}

void TestHelper::ViewOtherRegisters(Cpu cpu)
{
	cout << "PC : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getPc())) << "(" << cpu.getPc() << ")" << endl;
	IOHelper::WriteLog("PC : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getPc())) + "(" + to_string(cpu.getPc()) + ")");
	cout << "IR : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIr())) << "(" << cpu.getIr() << ")" << endl;
	IOHelper::WriteLog("IR : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIr())) + "(" + to_string(cpu.getIr()) + ")");

	cout << "IfIdPc : " << ConvertHelper::SeperateString(ConvertHelper::AddressToString(cpu.getIfIdPc())) << "(" << cpu.getIfIdPc() << ")" << endl;
	IOHelper::WriteLog("IfIdPc : " + ConvertHelper::SeperateString(ConvertHelper::AddressToString(cpu.getIfIdPc())) + "(" + to_string(cpu.getIfIdPc()) + ")");

	cout << "IdExPc : " << ConvertHelper::SeperateString(ConvertHelper::AddressToString(cpu.getIdExPc())) << "(" << cpu.getIdExPc() << ")" << endl;
	IOHelper::WriteLog("IdExPc : " + ConvertHelper::SeperateString(ConvertHelper::AddressToString(cpu.getIdExPc())) + "(" + to_string(cpu.getIdExPc()) + ")");
	cout << "IdExTypeR : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExTypeR())) << "(" << cpu.getIdExTypeR() << ")" << endl;
	IOHelper::WriteLog("IdExTypeR : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExTypeR())) + "(" + to_string(cpu.getIdExTypeR()) + ")");
	cout << "IdExTypeI : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExTypeI())) << "(" << cpu.getIdExTypeI() << ")" << endl;
	IOHelper::WriteLog("IdExTypeI : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExTypeI())) + "(" + to_string(cpu.getIdExTypeI()) + ")");
	cout << "IdExTypeJ : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExTypeJ())) << "(" << cpu.getIdExTypeJ() << ")" << endl;
	IOHelper::WriteLog("IdExTypeJ : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExTypeJ())) + "(" + to_string(cpu.getIdExTypeJ()) + ")");
	cout << "IdExOp : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExOp())) << "(" << cpu.getIdExOp() << ")" << endl;
	IOHelper::WriteLog("IdExOp : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExOp())) + "(" + to_string(cpu.getIdExOp()) + ")");
	cout << "IdExRs : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExRs())) << "(" << cpu.getIdExRs() << ")" << endl;
	IOHelper::WriteLog("IdExRs : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExRs())) + "(" + to_string(cpu.getIdExRs()) + ")");
	cout << "IdExRt : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExRt())) << "(" << cpu.getIdExRt() << ")" << endl;
	IOHelper::WriteLog("IdExRt : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExRt())) + "(" + to_string(cpu.getIdExRt()) + ")");
	cout << "IdExRd : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExRd())) << "(" << cpu.getIdExRd() << ")" << endl;
	IOHelper::WriteLog("IdExRd : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExRd())) + "(" + to_string(cpu.getIdExRd()) + ")");
	cout << "IdExShamt : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExShamt())) << "(" << cpu.getIdExShamt() << ")" << endl;
	IOHelper::WriteLog("IdExShamt : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExShamt())) + "(" + to_string(cpu.getIdExShamt()) + ")");
	cout << "IdExFunc : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExFunc())) << "(" << cpu.getIdExFunc() << ")" << endl;
	IOHelper::WriteLog("IdExShamt : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExFunc())) + "(" + to_string(cpu.getIdExFunc()) + ")");
	cout << "IdExImmediate : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExImmediate())) << "(" << cpu.getIdExImmediate() << ")" << endl;
	IOHelper::WriteLog("IdExImmediate : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExImmediate())) + "(" + to_string(cpu.getIdExImmediate()) + ")");
	cout << "IdExAddress_ : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExAddress_())) << "(" << cpu.getIdExAddress_() << ")" << endl;
	IOHelper::WriteLog("IdExAddress_ : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExAddress_())) + "(" + to_string(cpu.getIdExAddress_()) + ")");
	cout << "IdExNeedLoad : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExNeedLoad())) << "(" << cpu.getIdExNeedLoad() << ")" << endl;
	IOHelper::WriteLog("IdExNeedLoad : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExNeedLoad())) + "(" + to_string(cpu.getIdExNeedLoad()) + ")");
	cout << "IdExNeedStore : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExNeedStore())) << "(" << cpu.getIdExNeedStore() << ")" << endl;
	IOHelper::WriteLog("IdExNeedStore : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExNeedStore())) + "(" + to_string(cpu.getIdExNeedStore()) + ")");
	cout << "IdExRegValue : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExRegValue())) << "(" << cpu.getIdExRegValue() << ")" << endl;
	IOHelper::WriteLog("IdExRegValue : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExRegValue())) + "(" + to_string(cpu.getIdExRegValue()) + ")");
	cout << "IdExNeedWriteBack : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExNeedWriteBack())) << "(" << cpu.getIdExNeedWriteBack() << ")" << endl;
	IOHelper::WriteLog("IdExNeedWriteBack : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExNeedWriteBack())) + "(" + to_string(cpu.getIdExNeedWriteBack()) + ")");
	cout << "IdExIndex : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExIndex())) << "(" << cpu.getIdExIndex() << ")" << endl;
	IOHelper::WriteLog("IdExIndex : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getIdExIndex())) + "(" + to_string(cpu.getIdExIndex()) + ")");

	cout << "ExMemNeedLoad : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getExMemNeedLoad())) << "(" << cpu.getExMemNeedLoad() << ")" << endl;
	IOHelper::WriteLog("ExMemNeedLoad : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getExMemNeedLoad())) + "(" + to_string(cpu.getExMemNeedLoad()) + ")");
	cout << "ExMemNeedStore : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getExMemNeedStore())) << "(" << cpu.getExMemNeedStore() << ")" << endl;
	IOHelper::WriteLog("ExMemNeedStore : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getExMemNeedStore())) + "(" + to_string(cpu.getExMemNeedStore()) + ")");
	cout << "ExMemAddress : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getExMemAddress())) << "(" << cpu.getExMemAddress() << ")" << endl;
	IOHelper::WriteLog("ExMemAddress : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getExMemAddress())) + "(" + to_string(cpu.getExMemAddress()) + ")");
	cout << "ExMemRegValue : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getExMemRegValue())) << "(" << cpu.getExMemRegValue() << ")" << endl;
	IOHelper::WriteLog("ExMemRegValue : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getExMemRegValue())) + "(" + to_string(cpu.getExMemRegValue()) + ")");
	cout << "ExMemNeedWriteBack : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getExMemNeedWriteBack())) << "(" << cpu.getExMemNeedWriteBack() << ")" << endl;
	IOHelper::WriteLog("ExMemNeedWriteBack : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getExMemNeedWriteBack())) + "(" + to_string(cpu.getExMemNeedWriteBack()) + ")");
	cout << "ExMemIndex : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getExMemIndex())) << "(" << cpu.getExMemIndex() << ")" << endl;
	IOHelper::WriteLog("ExMemIndex : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getExMemIndex())) + "(" + to_string(cpu.getExMemIndex()) + ")");
	cout << "ExMemWord : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getExMemWord())) << "(" << cpu.getExMemWord() << ")" << endl;
	IOHelper::WriteLog("ExMemWord : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getExMemWord())) + "(" + to_string(cpu.getExMemWord()) + ")");

	cout << "MemIfAllow : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getMemIfAllow())) << "(" << cpu.getMemIfAllow() << ")" << endl;
	IOHelper::WriteLog("MemIfAllow : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getMemIfAllow())) + "(" + to_string(cpu.getMemIfAllow()) + ")");
	cout << "MemWbNeedWriteBack : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getMemWbNeedWriteBack())) << "(" << cpu.getMemWbNeedWriteBack() << ")" << endl;
	IOHelper::WriteLog("MemWbNeedWriteBack : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getMemWbNeedWriteBack())) + "(" + to_string(cpu.getMemWbNeedWriteBack()) + ")");
	cout << "MemWbIndex : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getMemWbIndex())) << "(" << cpu.getMemWbIndex() << ")" << endl;
	IOHelper::WriteLog("MemWbIndex : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getMemWbIndex())) + "(" + to_string(cpu.getMemWbIndex()) + ")");
	cout << "MemWbWord : " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getMemWbWord())) << "(" << cpu.getMemWbWord() << ")" << endl;
	IOHelper::WriteLog("MemWbWord : " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.getMemWbWord())) + "(" + to_string(cpu.getMemWbWord()) + ")");
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
