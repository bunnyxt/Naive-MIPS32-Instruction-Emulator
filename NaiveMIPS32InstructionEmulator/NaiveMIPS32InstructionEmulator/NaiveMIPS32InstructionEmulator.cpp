#include "NaiveMIPS32InstructionEmulator.h"

int main() {
	//initialize components
	Cpu cpu;
	Memory memory;

	//load code & data
	string codeFileName, memoryDataFileName, registerDataFileName;
	ifstream codeFileIn, memoryDataFileIn, registerDataFileIn;

	//load code
	while (true)
	{
		cout << "Enter code file name : ";
		cin >> codeFileName;
		codeFileIn.open(codeFileName);
		if (!codeFileIn.is_open())
		{
			cout << "Cannot open code file " << codeFileName << "! Please retry..." << endl;
		}
		else
		{
			cout << "Code file " << codeFileName << " opened successfully!" << endl;
			break;
		}
	}
	IOHelper::LoadCode(memory, codeFileIn);
	cout << endl;

	//load memory data
	while (true)
	{
		cout << "Enter memory data file name : ";
		cin >> memoryDataFileName;
		memoryDataFileIn.open(memoryDataFileName);
		if (!memoryDataFileIn.is_open())
		{
			cout << "Cannot open memory data file " << memoryDataFileName << "! Please retry..." << endl;
		}
		else
		{
			cout << "Memory data file " << memoryDataFileName << " opened successfully!" << endl;
			break;
		}
	}
	IOHelper::LoadMemoryData(memory, memoryDataFileIn);
	cout << endl;

	//load register data
	while (true)
	{
		cout << "Enter register data file name : ";
		cin >> registerDataFileName;
		registerDataFileIn.open(registerDataFileName);
		if (!registerDataFileIn.is_open())
		{
			cout << "Cannot open register data file " << registerDataFileName << "! Please retry..." << endl;
		}
		else
		{
			cout << "Register data file " << registerDataFileName << " opened successfully!" << endl;
			break;
		}
	}
	IOHelper::LoadRegisterData(cpu.GetGeneralPurposeRegisterSet(), registerDataFileIn);
	cout << endl;

	cout << "By.bunnyxt 2018-9-4" << endl;

	return 0;
}