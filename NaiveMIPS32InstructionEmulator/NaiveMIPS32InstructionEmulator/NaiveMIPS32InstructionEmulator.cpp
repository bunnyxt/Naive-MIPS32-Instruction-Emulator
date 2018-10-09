#include "NaiveMIPS32InstructionEmulator.h"

//initialize log filestream
ofstream IOHelper::log = ofstream();

int main() {
	cout << "[Emulator] Naive MIPS32 Instruction Emulator by.bunnyxt" << endl;
	cout << endl;

	//open log filestream
	if (IOHelper::OpenLogFileStream())
	{
		cout << "[Emulator] Log file created successfully!\n" << endl;
		IOHelper::WriteLog("[Emulator] Naive MIPS32 Instruction Emulator\n");
		IOHelper::WriteLog("[Emulator] Log file created.\n");
	}
	else
	{
		cout << "[Emulator] Error! Fail to create log file. Now exit..." << endl;
		exit(0);
	}

	//initialize components
	cout << "[Emulator] Now initialize components..." << endl;
	IOHelper::WriteLog("[Emulator] Now initialize components...");
	Cpu cpu;
	Memory memory;
	cout << "[Emulator] Components initialized.\n" << endl;
	IOHelper::WriteLog("[Emulator] Components initialized.\n");

	//load code & data
	string codeFileName, memoryDataFileName, registerDataFileName;
	ifstream codeFileIn, memoryDataFileIn, registerDataFileIn;

	//load code
	while (true)
	{
		IOHelper::WriteLog("[Emulator] Now load code file...");
		cout << "[Emulator] Enter code file name : ";
		cin >> codeFileName;
		codeFileIn.open(codeFileName);
		if (!codeFileIn.is_open())
		{
			cout << "[Emulator] Warning! Cannot open code file " << codeFileName << "! Please retry..." << endl;
			IOHelper::WriteLog("[Emulator] Warning! Fail to open file " + codeFileName + ".");
		}
		else
		{
			cout << "[Emulator] Code file " << codeFileName << " opened successfully!" << endl;
			IOHelper::WriteLog("[Emulator] Load code from " + codeFileName + "...");
			break;
		}
	}
	IOHelper::LoadCode(memory, codeFileIn);
	IOHelper::WriteLog("[Emulator] Code file loaded.\n");
	cout << endl;

	//load memory data
	while (true)
	{
		IOHelper::WriteLog("[Emulator] Now load memory data file...");
		cout << "[Emulator] Enter memory data file name : ";
		cin >> memoryDataFileName;
		memoryDataFileIn.open(memoryDataFileName);
		if (!memoryDataFileIn.is_open())
		{
			cout << "[Emulator] Warning! Cannot open memory data file " << memoryDataFileName << "! Please retry..." << endl;
			IOHelper::WriteLog("[Emulator] Warning! Fail to open file " + memoryDataFileName + ".");
		}
		else
		{
			cout << "[Emulator] Memory data file " << memoryDataFileName << " opened successfully!" << endl;
			IOHelper::WriteLog("[Emulator] Load memory data from " + memoryDataFileName + "...");
			break;
		}
	}
	IOHelper::LoadMemoryData(memory, memoryDataFileIn);
	IOHelper::WriteLog("[Emulator] Memory data file loaded.\n");
	cout << endl;

	//load register data
	while (true)
	{
		IOHelper::WriteLog("[Emulator] Now load register data file...");
		cout << "[Emulator] Enter register data file name : ";
		cin >> registerDataFileName;
		registerDataFileIn.open(registerDataFileName);
		if (!registerDataFileIn.is_open())
		{
			cout << "[Emulator] Warning! Cannot open register data file " << registerDataFileName << "! Please retry..." << endl;
			IOHelper::WriteLog("[Emulator] Warning! Fail to open file " + registerDataFileName + ".");
		}
		else
		{
			cout << "[Emulator] Register data file " << registerDataFileName << " opened successfully!" << endl;
			IOHelper::WriteLog("[Emulator] Load register data from " + registerDataFileName + "...");
			break;
		}
	}
	IOHelper::LoadRegisterData(cpu, registerDataFileIn);
	IOHelper::WriteLog("[Emulator] Register data file loaded.\n");
	cout << endl;

	//select mode
	bool enableSingleStepTest = false;
	while (true)
	{
		char ch;
		cout << "[Emulator] Enable single step debugging? Y/N : ";
		cin >> ch;
		if (ch == 'Y' || ch == 'y')
		{
			enableSingleStepTest = true;
			cout << "[Emulator] Enable single step test." << endl;
			IOHelper::WriteLog("[Emulator] Enable single step test.\n");
			break;
		}
		else if (ch == 'N' || ch == 'n')
		{
			enableSingleStepTest = false;
			cout << "[Emulator] Disable single step test." << endl;
			IOHelper::WriteLog("[Emulator] Disable single step test.\n");
			break;
		}
		else
		{
			cout << "[Emulator] Warning! Invalid mode detected! Please retry..." << endl;
		}
	}
	cout << endl;

	//start process
	int clockNumber = 1;
	bool eoiDetected = false;
	while (cpu.HasWork())
	{
		cout << "[Emulator] ==================== Now start clock " << clockNumber << "... ====================" << endl;
		IOHelper::WriteLog("[Emulator] ==================== Clock " + to_string(clockNumber) + " ====================");

		//cpu run 1 clock
		cpu.Run(memory, eoiDetected, clockNumber);

		cout << "[Emulator] ==================== Clock " << clockNumber << " run end. ====================\n" << endl;
		IOHelper::WriteLog("[Emulator] ==================== Clock " + to_string(clockNumber) + " run end. ====================\n");
		if (!enableSingleStepTest)
		{

		}
		else
		{
			//test time
			cout << "[Emulator] Now start clock " << clockNumber << " test..." << endl;
			IOHelper::WriteLog("[Emulator] Clock " + to_string(clockNumber) + " test");
			int mode = -1;
			while (mode != 0)
			{
				cout << "[Emulator] Test mode selection : \n[Emulator] 1 - view general purpose register set in cpu ; \n[Emulator] 2 - view other registers in cpu ; \n[Emulator] 3 - view memory unit value ; \n[Emulator] 0 - continue to next clock ;\n[Emulator] Select mode : ";
				cin >> mode;
				switch (mode)
				{
				case 1:
					cout << "[Emulator] Now view general purpose register set in cpu..." << endl;
					IOHelper::WriteLog("[Emulator] Now view general purpose register set in cpu...");
					TestHelper::ViewGeneralPurposeRegisterSet(cpu);
					break;
				case 2:
					cout << "[Emulator] Now view view other registers in cpu..." << endl;
					IOHelper::WriteLog("[Emulator] Now view view other registers in cpu...");
					TestHelper::ViewOtherRegisters(cpu);
					break;
				case 3:
					cout << "[Emulator] Now view memory unit value..." << endl;
					address a;
					cout << "[Emulator] Please enter memory address(dec format) : ";
					cin >> a;
					cout << "[Emulator] Now view memoru unit value near address " << a << "..." << endl;
					IOHelper::WriteLog("[Emulator] Now view memoru unit value near address " + to_string(a) + "...");
					TestHelper::ViewMemoryUnitValue(memory, a);
					break;
				case 0:
					cout << "[Emulator] Now continue to next clock...\n" << endl;
					IOHelper::WriteLog("[Emulator] Now continue to next clock...\n");
					break;
				default:
					cout << "[Emulator] Warning! Invalid mode id " << mode << " detected! Please retry..." << endl;
					break;
				}
			}
		}

		clockNumber++;

	}

	while (true)
	{
		char ch;
		cout << "[Emulator] No more instructions left now! Need test now? Y/N : ";
		cin >> ch;
		if (ch == 'Y' || ch == 'y')
		{
			cout << "[Emulator] Now start last test." << endl;
			IOHelper::WriteLog("[Emulator] Now start last test.");

			int mode = -1;
			while (mode != 0)
			{
				cout << "[Emulator] Test mode selection : \n[Emulator] 1 - view general purpose register set in cpu ; \n[Emulator] 2 - view other registers in cpu ; \n[Emulator] 3 - view memory unit value ; \n[Emulator] 0 - exit last test ;\n[Emulator] Select mode : ";
				cin >> mode;
				switch (mode)
				{
				case 1:
					cout << "[Emulator] Now view general purpose register set in cpu..." << endl;
					IOHelper::WriteLog("[Emulator] Now view general purpose register set in cpu...");
					TestHelper::ViewGeneralPurposeRegisterSet(cpu);
					break;
				case 2:
					cout << "[Emulator] Now view view other registers in cpu..." << endl;
					IOHelper::WriteLog("[Emulator] Now view view other registers in cpu...");
					TestHelper::ViewOtherRegisters(cpu);
					break;
				case 3:
					cout << "[Emulator] Now view memory unit value..." << endl;
					address a;
					cout << "[Emulator] Please enter memory address(dec format) : ";
					cin >> a;
					cout << "[Emulator] Now view memoru unit value near address " << a << "..." << endl;
					IOHelper::WriteLog("[Emulator] Now view memoru unit value near address " + to_string(a) + "...");
					TestHelper::ViewMemoryUnitValue(memory, a);
					break;
				case 0:
					cout << "[Emulator] Now exit last test...\n" << endl;
					IOHelper::WriteLog("[Emulator] Now exit last test...\n");
					break;
				default:
					cout << "[Emulator] Warning! Invalid mode id " << mode << " detected! Please retry..." << endl;
					break;
				}
			}
			break;
		}
		else if (ch == 'N' || ch == 'n')
		{
			cout << "[Emulator] Last test skipped.\n" << endl;
			IOHelper::WriteLog("[Emulator] Last test skipped.\n");
			break;
		}
		else
		{
			cout << "[Emulator] Warning! Invalid input detected! Please retry..." << endl;
		}
	}

	cout << "[Emulator] By.bunnyxt 2018-9-24" << endl;
	IOHelper::WriteLog("[Emulator] By.bunnyxt 2018-10-9\n");

	IOHelper::WriteLog("[Emulator] Close log file.");
	IOHelper::CloseLogFileStream();

	return 0;
}