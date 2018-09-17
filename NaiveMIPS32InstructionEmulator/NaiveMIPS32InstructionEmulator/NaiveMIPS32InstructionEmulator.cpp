#include "NaiveMIPS32InstructionEmulator.h"

//initialize log filestream
ofstream IOHelper::log = ofstream();

int main() {
	cout << "Naive MIPS32 Instruction Emulator" << endl;
	cout << endl;

	//open log filestream
	if (IOHelper::OpenLogFileStream())
	{
		cout << "Log file created successfully!\n" << endl;
		IOHelper::WriteLog("Naive MIPS32 Instruction Emulator\n");
		IOHelper::WriteLog("Log file created.\n");
	}
	else
	{
		cout << "Error! Fail to create log file. Now exit..." << endl;
		exit(0);
	}

	//initialize components
	Cpu cpu;
	Memory memory;

	//load code & data
	string codeFileName, memoryDataFileName, registerDataFileName;
	ifstream codeFileIn, memoryDataFileIn, registerDataFileIn;

	//load code
	while (true)
	{
		IOHelper::WriteLog("Now load code file...");
		cout << "Enter code file name : ";
		cin >> codeFileName;
		codeFileIn.open(codeFileName);
		if (!codeFileIn.is_open())
		{
			cout << "Cannot open code file " << codeFileName << "! Please retry..." << endl;
			IOHelper::WriteLog("Fail to open file " + codeFileName + ".");
		}
		else
		{
			cout << "Code file " << codeFileName << " opened successfully!" << endl;
			IOHelper::WriteLog("Load code from " + codeFileName + "...");
			break;
		}
	}
	IOHelper::LoadCode(memory, codeFileIn);
	IOHelper::WriteLog("Code file loaded.\n");
	cout << endl;

	//load memory data
	while (true)
	{
		IOHelper::WriteLog("Now load memory data file...");
		cout << "Enter memory data file name : ";
		cin >> memoryDataFileName;
		memoryDataFileIn.open(memoryDataFileName);
		if (!memoryDataFileIn.is_open())
		{
			cout << "Cannot open memory data file " << memoryDataFileName << "! Please retry..." << endl;
			IOHelper::WriteLog("Fail to open file " + memoryDataFileName + ".");
		}
		else
		{
			cout << "Memory data file " << memoryDataFileName << " opened successfully!" << endl;
			IOHelper::WriteLog("Load memory data from " + memoryDataFileName + "...");
			break;
		}
	}
	IOHelper::LoadMemoryData(memory, memoryDataFileIn);
	IOHelper::WriteLog("Memory data file loaded.\n");
	cout << endl;

	//load register data
	while (true)
	{
		IOHelper::WriteLog("Now load register data file...");
		cout << "Enter register data file name : ";
		cin >> registerDataFileName;
		registerDataFileIn.open(registerDataFileName);
		if (!registerDataFileIn.is_open())
		{
			cout << "Cannot open register data file " << registerDataFileName << "! Please retry..." << endl;
			IOHelper::WriteLog("Fail to open file " + registerDataFileName + ".");
		}
		else
		{
			cout << "Register data file " << registerDataFileName << " opened successfully!" << endl;
			IOHelper::WriteLog("Load register data from " + registerDataFileName + "...");
			break;
		}
	}
	IOHelper::LoadRegisterData(cpu.GetGeneralPurposeRegisterSet(), registerDataFileIn);
	IOHelper::WriteLog("Register data file loaded.\n");
	cout << endl;

	cout << "By.bunnyxt 2018-9-17" << endl;
	IOHelper::WriteLog("By.bunnyxt 2018-9-17\n");

	IOHelper::WriteLog("Close log file.");
	IOHelper::CloseLogFileStream();

	return 0;
}