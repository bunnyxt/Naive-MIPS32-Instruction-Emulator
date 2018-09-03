#include "NaiveMIPS32InstructionEmulator.h"

int main() {
	//initialize components
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
	IOHelper::LoadRegisterData(memory, registerDataFileIn);

	return 0;
}