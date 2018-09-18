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

	//select mode
	bool enableSingleStepTest = false;
	while (true)
	{
		char ch;
		cout << "Enable single step debugging? Y/N : ";
		cin >> ch;
		if (ch == 'Y' || ch == 'y')
		{
			enableSingleStepTest = true;
			cout << "Enable single step test." << endl;
			IOHelper::WriteLog("Enable single step test.\n");
			break;
		}
		else if (ch == 'N' || ch == 'n')
		{
			enableSingleStepTest = false;
			cout << "Disable single step test." << endl;
			IOHelper::WriteLog("Disable single step test.\n");
			break;
		}
		else
		{
			cout << "Warning! Invalid mode detected! Please retry..." << endl;
		}
	}
	cout << endl;

	//start process
	int clockNumber = 1;
	while (true)
	{
		cout << "Now start clock " << clockNumber << "..." << endl;
		IOHelper::WriteLog("Clock " + to_string(clockNumber));

		bool readyStateChangedFlag = false;

		//WB
		cout << "WB" << endl;
		IOHelper::WriteLog("WB");
		if (!cpu.IsReady(4))
		{
			cout << "No work now." << endl;
			IOHelper::WriteLog("No work now.");
		}
		else
		{
			if (cpu.GetMemWbNeedWriteBack() == 0)
			{
				cout << "No need now." << endl;
				IOHelper::WriteLog("No need now.");
			}
			else
			{
				cout << "Write word " << ConvertHelper::WordToString(cpu.GetMemWbWord()) << " back to r" << cpu.GetMemWbIndex() << "." << endl;
				IOHelper::WriteLog("Write word " + ConvertHelper::WordToString(cpu.GetMemWbWord()) + " back to r" + to_string(cpu.GetMemWbIndex()) + ".");
				cpu.GetGeneralPurposeRegisterSet().Set(cpu.GetMemWbIndex(), cpu.GetMemWbWord());
			}
		}

		//MEM
		cout << "MEM" << endl;
		IOHelper::WriteLog("MEM");
		if (!cpu.IsReady(3))
		{
			cout << "No work now." << endl;
			IOHelper::WriteLog("No work now.");
		}
		else
		{
			if (cpu.GetExMemNeedLoad() != 0)
			{
				//TODO log
				cpu.SetMemWbWord(memory.ReadWord(cpu.GetExMemAddress()));
			}

			if (cpu.GetExMemNeedStore() != 0)
			{
				//TODO log
				memory.WriteWord(cpu.GetExMemAddress(), cpu.GetGeneralPurposeRegisterSet().Get(cpu.GetExMemReg()));
			}

			//TODO log
			cpu.SetMemWbNeedWriteBack(cpu.GetExMemNeedWriteBack());
			cpu.SetMemWbIndex(cpu.GetExMemIndex());
			cpu.SetMemWbWord(cpu.GetExMemWord());
		}

		//EX
		cout << "EX" << endl;
		IOHelper::WriteLog("EX");
		if (!cpu.IsReady(2))
		{
			cout << "No work now." << endl;
			IOHelper::WriteLog("No work now.");
		}
		else
		{
			//get instruction type

			//get ex type

			//get numbers and send them to alu or others

			//trans regs
		}

		//ID
		cout << "ID" << endl;
		IOHelper::WriteLog("ID");
		if (!cpu.IsReady(1))
		{
			cout << "No work now." << endl;
			IOHelper::WriteLog("No work now.");
		}
		else
		{
			//get ir
			switch ((cpu.GetIr() & 0xFC000000) >> 26)
			{
			case 0:
				//type R
				switch ((cpu.GetIr() & 0x00000003F))
				{
				case 0x21:
					//addu
					break;
				case 0x23:
					//subu
					break;
				case 0x24:
					//and
					break;
				case 0x25:
					//or
					break;
				case 0x26:
					//xor
					break;
				case 0x27:
					//nor
					break;
				case 0x00:
					//sll
					break;
				case 0x02:
					//srl
					break;
				case 0x08:
					//jr
					break;
				default:
					//not support 
					break;
				}
				break;
			case 2:
				//type J
				break;
			default:
				//case I
				switch ((cpu.GetIr() & 0xFC000000) >> 26)
				{
				case 0x0C:
					//andi
					break;
				case 0x0D:
					//ori
					break;
				case 0x0E:
					//xori
					break;
				case 0x04:
					//beq
					break;
				case 0x05:
					//bne
					break;
				case 0x23:
					//lw
					break;
				case 0x2B:
					//sw
					break;
				default:
					//not support
					break;
				}
				break;
			}

			//decode and set all reg

			//trans reg
		}

		//IF
		cout << "IF" << endl;
		IOHelper::WriteLog("IF");
		if (!cpu.IsReady(0))
		{
			cout << "No work now." << endl;
			IOHelper::WriteLog("No work now.");
		}
		else
		{
			if (cpu.GetExMemNeedLoad() == 1 || cpu.GetExMemNeedStore() == 1)
			{
				//delay
			}
			else
			{
				//fetch instruction from memory to ir
				cpu.SetIr(memory.ReadWord(cpu.GetPc()));

				//modify pc
				cpu.SetPc = cpu.GetPc() + 4;
			}
		}

		if (!readyStateChangedFlag)
		{
			cpu.MoveOnReady();
		}


		cout << "Clock " << clockNumber << " run end." << endl;
		IOHelper::WriteLog("Clock " + to_string(clockNumber) + " run end.");
		clockNumber++;
		if (!enableSingleStepTest)
		{
			cout << endl;
			continue;
		}

		//test time
		cout << "Now start clock " << clockNumber << " test..." << endl;
		IOHelper::WriteLog("Clock " + to_string(clockNumber) + " test");
		int mode = -1;
		while (mode != 0)
		{
			cout << "Test mode selection : 1 - ; 2 - ; 3 - :";
			cin >> mode;
			switch (mode)
			{
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			default:
				cout << "Warning! Invalid mode id " << mode << " detected! Please retry..." << endl;
				break;
			}
		}
	}










	cout << "By.bunnyxt 2018-9-17" << endl;
	IOHelper::WriteLog("By.bunnyxt 2018-9-17\n");

	IOHelper::WriteLog("Close log file.");
	IOHelper::CloseLogFileStream();

	return 0;
}