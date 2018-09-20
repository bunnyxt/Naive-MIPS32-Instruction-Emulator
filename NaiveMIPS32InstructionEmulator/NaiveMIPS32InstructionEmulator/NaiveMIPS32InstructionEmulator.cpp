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
	cout << "Now initialize components..." << endl;
	IOHelper::WriteLog("Now initialize components...");
	Cpu cpu;
	Memory memory;
	cout << "Components initialized.\n" << endl;
	IOHelper::WriteLog("Components initialized.\n");

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
			cout << "Warning! Cannot open code file " << codeFileName << "! Please retry..." << endl;
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
			cout << "Warning! Cannot open memory data file " << memoryDataFileName << "! Please retry..." << endl;
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
			cout << "Warning! Cannot open register data file " << registerDataFileName << "! Please retry..." << endl;
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
		cout << "========== Now start clock " << clockNumber << "... ==========" << endl;
		IOHelper::WriteLog("========== Clock " + to_string(clockNumber) + " ==========");

		cpu.ResetRunStatus();

		try
		{
			//WB
			cout << "WB" << endl;
			IOHelper::WriteLog("WB");
			if (!cpu.IsReady(4))
			{
				cout << "No work now." << endl;
				IOHelper::WriteLog("No work now.");
				cpu.SetRunInterrupted(4);
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
					//write back
					cout << "Write word " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetMemWbWord())) << "(" << cpu.GetMemWbWord() << ") back to r" << cpu.GetMemWbIndex() << "." << endl;
					IOHelper::WriteLog("Write word " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetMemWbWord())) + "(" + to_string(cpu.GetMemWbWord()) + ") back to r" + to_string(cpu.GetMemWbIndex()) + ".");
					cpu.GetGeneralPurposeRegisterSet().Set(cpu.GetMemWbIndex(), cpu.GetMemWbWord());

					//unlock reg
					cpu.UnlockReg(cpu.GetMemWbIndex());

					//free fw
					if (cpu.GetFw0Index() == cpu.GetMemWbIndex())
					{
						cpu.SetFw0Vacant();
					}
					else if (cpu.GetFw1Index() == cpu.GetMemWbIndex())
					{
						cpu.SetFw1Vacant();
					}
					else
					{
						//no index from found, error occurred
					}
				}
			}

			//MEM
			cout << "MEM" << endl;
			IOHelper::WriteLog("MEM");
			if (!cpu.IsReady(3))
			{
				cout << "No work now." << endl;
				IOHelper::WriteLog("No work now.");
				cpu.SetRunInterrupted(3);
			}
			else
			{
				if (cpu.GetExMemNeedLoad() != 0)
				{
					//TODO log
					cpu.SetMemWbWord(memory.ReadWord(cpu.GetExMemAddress()));

					//not allow IF
					cpu.SetMemIfAllow(1);

					//set FW
					if (cpu.IsFw0Vacant())
					{
						cpu.SetFw0Index(cpu.GetMemWbIndex());
						cpu.SetFw0Value(cpu.GetMemWbWord());
					}
					else if (cpu.IsFw1Vacant())
					{
						cpu.SetFw1Index(cpu.GetMemWbIndex());
						cpu.SetFw1Value(cpu.GetMemWbWord());
					}
					else
					{
						//no free fw now, fail to unlock before
					}
				}
				else if (cpu.GetExMemNeedStore() != 0)
				{
					//TODO log
					memory.WriteWord(cpu.GetExMemAddress(), cpu.GetExMemRegValue());

					//not allow IF
					cpu.SetMemIfAllow(1);

					cpu.SetMemWbWord(cpu.GetExMemWord());
				}
				else
				{
					//allow IF
					cpu.SetMemIfAllow(0);

					cpu.SetMemWbWord(cpu.GetExMemWord());
				}

				//TODO log
				cpu.SetMemWbNeedWriteBack(cpu.GetExMemNeedWriteBack());
				cpu.SetMemWbIndex(cpu.GetExMemIndex());
			}

			//EX
			cout << "EX" << endl;
			IOHelper::WriteLog("EX");
			if (!cpu.IsReady(2))
			{
				cout << "No work now." << endl;
				IOHelper::WriteLog("No work now.");
				cpu.SetRunInterrupted(2);
			}
			else
			{
				//get instruction type
				if (cpu.GetIdExTypeR() == 1)
				{
					//set word
					cpu.SetExMemWord(cpu.GetAlu().CalculateR(cpu.GetIdExRs(), cpu.GetIdExRt(), cpu.GetIdExShamt(), cpu.GetIdExFunc()));

					//set FW
					if (cpu.IsFw0Vacant())
					{
						cpu.SetFw0Index(cpu.GetIdExIndex());
						cpu.SetFw0Value(cpu.GetExMemWord());
					}
					else if (cpu.IsFw1Vacant())
					{
						cpu.SetFw1Index(cpu.GetIdExIndex());
						cpu.SetFw1Value(cpu.GetExMemWord());
					}
					else
					{
						//no free fw now, fail to unlock before
					}
				}
				else if (cpu.GetIdExTypeI() == 1)
				{
					switch (cpu.GetIdExOp())
					{
					case 0x0C:
					case 0x0D:
					case 0x0E:
						//andi ori xori

						//set word
						cpu.SetExMemWord(cpu.GetAlu().CalculateI(cpu.GetIdExOp(), cpu.GetIdExRs(), cpu.GetIdExRt(), cpu.GetIdExImmediate()));

						//set FW
						if (cpu.IsFw0Vacant())
						{
							cpu.SetFw0Index(cpu.GetIdExIndex());
							cpu.SetFw0Value(cpu.GetExMemWord());
						}
						else if (cpu.IsFw1Vacant())
						{
							cpu.SetFw1Index(cpu.GetIdExIndex());
							cpu.SetFw1Value(cpu.GetExMemWord());
						}
						else
						{
							//no free fw now, fail to unlock before
						}

						break;
					case 0x04:
					case 0x05:
						//beq bne

						//TODO

						break;
					case 0x23:
						//lw

						//set address
						cpu.SetExMemAddress(cpu.GetAlu().CalculateI(cpu.GetIdExOp(), cpu.GetIdExRs(), cpu.GetIdExRt(), cpu.GetIdExImmediate()));

						break;
					case 0x2B:
						//sw

						//set address
						cpu.SetExMemAddress(cpu.GetAlu().CalculateI(cpu.GetIdExOp(), cpu.GetIdExRs(), cpu.GetIdExRt(), cpu.GetIdExImmediate()));

						break;
					default:
						break;
					}
				}
				else if (cpu.GetIdExTypeJ() == 1)
				{

				}
				else
				{
					//not support
					cout << "Error! No valid instruction type detected! Now exit..." << endl;
					IOHelper::WriteLog("Error! No valid instruction type detected! Now exit...");
					exit(0);
				}

				//trans regs
				cpu.SetExMemNeedLoad(cpu.GetIdExNeedLoad());
				cpu.SetExMemNeedStore(cpu.GetIdExNeedStore());
				cpu.SetExMemRegValue(cpu.GetIdExRegValue());
				cpu.SetExMemNeedWriteBack(cpu.GetIdExNeedWriteBack());
				cpu.SetExMemIndex(cpu.GetIdExIndex());
			}

			//ID
			cout << "ID" << endl;
			IOHelper::WriteLog("ID");
			if (!cpu.IsReady(1))
			{
				cout << "No work now." << endl;
				IOHelper::WriteLog("No work now.");
				cpu.SetRunInterrupted(1);
			}
			else
			{
				//clear reg
				cout << "Now clear regsiters..." << endl;
				IOHelper::WriteLog("Now clear registers...");
				cpu.SetIdExTypeR(0);
				cpu.SetIdExTypeJ(0);
				cpu.SetIdExTypeI(0);
				cpu.SetIdExOp(0);
				cpu.SetIdExRs(0);
				cpu.SetIdExRt(0);
				cpu.SetIdExRd(0);
				cpu.SetIdExShamt(0);
				cpu.SetIdExFunc(0);
				cpu.SetIdExImmediate(0);
				cpu.SetIdExAddress_(0);
				cpu.SetIdExNeedLoad(0);
				cpu.SetIdExNeedStore(0);
				//cpu.SetIdExAddress(0);
				cpu.SetIdExRegValue(0);
				cpu.SetIdExNeedWriteBack(0);
				cpu.SetIdExIndex(0);
				//cpu.SetIdExWord(0);
				cout << "Clear registers done." << endl;
				IOHelper::WriteLog("Clear registers done.");

				//category instruction loaded in IR
				switch (cpu.GetDecoder().GetOp(cpu.GetIr()))
				{
				case 0:
					//type R
					cpu.SetIdExTypeR(1);

					//set op
					cpu.SetIdExOp(0);

					//set func
					switch (cpu.GetDecoder().GetFunc(cpu.GetIr()))
					{
					case 0x21:
						//addu
						cpu.SetIdExFunc(0x21);
						break;
					case 0x23:
						//subu
						cpu.SetIdExFunc(0x23);
						break;
					case 0x24:
						//and
						cpu.SetIdExFunc(0x24);
						break;
					case 0x25:
						//or
						cpu.SetIdExFunc(0x25);
						break;
					case 0x26:
						//xor
						cpu.SetIdExFunc(0x26);
						break;
					case 0x27:
						//nor
						cpu.SetIdExFunc(0x27);
						break;
					case 0x00:
						//sll
						cpu.SetIdExFunc(0x00);
						break;
					case 0x02:
						//srl
						cpu.SetIdExFunc(0x02);
						break;
					case 0x08:
						//jr
						cpu.SetIdExFunc(0x08);
						break;
					default:
						//not support 
						cout << "Error! Invalid instruction " << ConvertHelper::InstructionToString(cpu.GetIr()) << " detected! Now exit..." << endl;
						IOHelper::WriteLog("Error! Invalid instruction " + ConvertHelper::InstructionToString(cpu.GetIr()) + " detected! Now exit...");
						exit(0);
						break;
					}

					//set shamt
					cpu.SetIdExShamt(cpu.GetDecoder().GetShamt(cpu.GetIr()));

					//set rd
					cpu.SetIdExRd(cpu.GetDecoder().GetRd(cpu.GetIr()));

					//set index
					cpu.SetIdExIndex(cpu.GetDecoder().GetRd(cpu.GetIr()));

					//can rt get from fw
					if (cpu.GetFw0Index() == cpu.GetDecoder().GetRt(cpu.GetIr()))
					{
						cpu.SetIdExRt(cpu.GetFw0Value());
					}
					else if (cpu.GetFw1Index() == cpu.GetDecoder().GetRt(cpu.GetIr()))
					{
						cpu.SetIdExRt(cpu.GetFw1Value());
					}
					else
					{
						//is reg locked
						if (cpu.IsRegLocked(cpu.GetDecoder().GetRt(cpu.GetIr())))
						{
							//delay
							throw 1;
						}
						else
						{
							//set rt
							cpu.SetIdExRt(cpu.GetGeneralPurposeRegisterSet().Get(cpu.GetDecoder().GetRt(cpu.GetIr())));
						}
					}

					//can rs get from fw
					if (cpu.GetFw0Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
					{
						cpu.SetIdExRs(cpu.GetFw0Value());
					}
					else if (cpu.GetFw1Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
					{
						cpu.SetIdExRs(cpu.GetFw1Value());
					}
					else
					{
						//is reg locked
						if (cpu.IsRegLocked(cpu.GetDecoder().GetRs(cpu.GetIr())))
						{
							//delay
							throw 1;
						}
						else
						{
							//set rs
							cpu.SetIdExRs(cpu.GetGeneralPurposeRegisterSet().Get(cpu.GetDecoder().GetRs(cpu.GetIr())));
						}
					}

					//lock rd
					cpu.LockReg(cpu.GetDecoder().GetRd(cpu.GetIr()));

					//set need write back
					cpu.SetIdExNeedWriteBack(1);

					break;
				case 2:
					//type J
					cpu.SetIdExTypeJ(1);

					//set op
					cpu.SetIdExOp(0x02);

					//set address_
					cpu.SetIdExAddress_(cpu.GetDecoder().GetAddress(cpu.GetIr()));

					break;
				default:
					//case I
					cpu.SetIdExTypeI(1);

					//set op
					switch (cpu.GetDecoder().GetOp(cpu.GetIr()))
					{
					case 0x0C:
						//andi
						cpu.SetIdExOp(0x0C);
						break;
					case 0x0D:
						//ori
						cpu.SetIdExOp(0x0D);
						break;
					case 0x0E:
						//xori
						cpu.SetIdExOp(0x0E);
						break;
					case 0x04:
						//beq
						cpu.SetIdExOp(0x04);
						break;
					case 0x05:
						//bne
						cpu.SetIdExOp(0x05);
						break;
					case 0x23:
						//lw
						cpu.SetIdExOp(0x23);
						cpu.SetIdExNeedLoad(1);
						break;
					case 0x2B:
						//sw
						cpu.SetIdExOp(0x2B);
						cpu.SetIdExNeedStore(1);
						break;
					default:
						//not support
						cout << "Error! Invalid instruction " << ConvertHelper::InstructionToString(cpu.GetIr()) << " detected! Now exit..." << endl;
						IOHelper::WriteLog("Error! Invalid instruction " + ConvertHelper::InstructionToString(cpu.GetIr()) + " detected! Now exit...");
						exit(0);
						break;
					}

					//set immediate
					cpu.SetIdExImmediate(cpu.GetDecoder().GetImmediate(cpu.GetIr()));

					//rs rt related
					switch (cpu.GetDecoder().GetOp(cpu.GetIr()))
					{
					case 0x0C:
					case 0x0D:
					case 0x0E:
						//andi ori xori

						//set rt
						cpu.SetIdExRt(cpu.GetDecoder().GetRt(cpu.GetIr()));

						//set need write back
						cpu.SetIdExNeedWriteBack(1);

						//set index
						cpu.SetIdExIndex(cpu.GetDecoder().GetRt(cpu.GetIr()));

						//can rs get from fw
						if (cpu.GetFw0Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
						{
							cpu.SetIdExRs(cpu.GetFw0Value());
						}
						else if (cpu.GetFw1Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
						{
							cpu.SetIdExRs(cpu.GetFw1Value());
						}
						else
						{
							//is reg locked
							if (cpu.IsRegLocked(cpu.GetDecoder().GetRs(cpu.GetIr())))
							{
								//delay
								throw 1;
							}
							else
							{
								//set rs
								cpu.SetIdExRs(cpu.GetGeneralPurposeRegisterSet().Get(cpu.GetDecoder().GetRs(cpu.GetIr())));
							}
						}

						//lock rt
						cpu.LockReg(cpu.GetDecoder().GetRt(cpu.GetIr()));

						break;
					case 0x04:
					case 0x05:
						//beq bne

						break;
					case 0x23:
						//lw

						//set rt
						cpu.SetIdExRt(cpu.GetDecoder().GetRt(cpu.GetIr()));

						//set need write back
						cpu.SetIdExNeedWriteBack(1);

						//set index
						cpu.SetIdExIndex(cpu.GetDecoder().GetRt(cpu.GetIr()));

						//can rs get from fw
						if (cpu.GetFw0Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
						{
							cpu.SetIdExRs(cpu.GetFw0Value());
						}
						else if (cpu.GetFw1Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
						{
							cpu.SetIdExRs(cpu.GetFw1Value());
						}
						else
						{
							//is reg locked
							if (cpu.IsRegLocked(cpu.GetDecoder().GetRs(cpu.GetIr())))
							{
								//delay
								throw 1;
							}
							else
							{
								//set rs
								cpu.SetIdExRs(cpu.GetGeneralPurposeRegisterSet().Get(cpu.GetDecoder().GetRs(cpu.GetIr())));
							}
						}

						//lock rt
						cpu.LockReg(cpu.GetDecoder().GetRt(cpu.GetIr()));

						break;
					case 0x2B:
						//sw

						//can rt get from fw
						if (cpu.GetFw0Index() == cpu.GetDecoder().GetRt(cpu.GetIr()))
						{
							cpu.SetIdExRt(cpu.GetFw0Value());
						}
						else if (cpu.GetFw1Index() == cpu.GetDecoder().GetRt(cpu.GetIr()))
						{
							cpu.SetIdExRt(cpu.GetFw1Value());
						}
						else
						{
							//is reg locked
							if (cpu.IsRegLocked(cpu.GetDecoder().GetRt(cpu.GetIr())))
							{
								//delay
								throw 1;
							}
							else
							{
								//set rt
								cpu.SetIdExRt(cpu.GetGeneralPurposeRegisterSet().Get(cpu.GetDecoder().GetRt(cpu.GetIr())));

								//set reg value
								cpu.SetIdExRegValue(cpu.GetGeneralPurposeRegisterSet().Get(cpu.GetDecoder().GetRt(cpu.GetIr())));
							}
						}

						//can rs get from fw
						if (cpu.GetFw0Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
						{
							cpu.SetIdExRs(cpu.GetFw0Value());
						}
						else if (cpu.GetFw1Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
						{
							cpu.SetIdExRs(cpu.GetFw1Value());
						}
						else
						{
							//is reg locked
							if (cpu.IsRegLocked(cpu.GetDecoder().GetRs(cpu.GetIr())))
							{
								//delay
								throw 1;
							}
							else
							{
								//set rs
								cpu.SetIdExRs(cpu.GetGeneralPurposeRegisterSet().Get(cpu.GetDecoder().GetRs(cpu.GetIr())));
							}
						}

						break;
					default:
						break;
					}

					break;
				}
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
				if (cpu.GetMemIfAllow() == 1)
				{
					//delay
					throw 0;
				}
				else
				{
					//fetch instruction from memory to IR
					cpu.SetIr(memory.ReadWord(cpu.GetPc()));

					//modify pc
					cpu.SetPc(cpu.GetPc() + 4);
				}
			}

		}
		catch (int index)
		{
			//delay process
			switch (index)
			{
			case 4:
				cpu.SetRunInterrupted(4);
			case 3:
				cpu.SetRunInterrupted(3);
			case 2:
				cpu.SetRunInterrupted(2);
			case 1:
				cpu.SetRunInterrupted(1);
			case 0:
				cpu.SetRunInterrupted(0);
				break;
			default:
				break;
			}
		}

		//Set next clock time ready
		cpu.SetNewReady();

		cout << "========== Clock " << clockNumber << " run end. ==========\n" << endl;
		IOHelper::WriteLog("========== Clock " + to_string(clockNumber) + " run end. ==========\n");
		if (!enableSingleStepTest)
		{

		}
		else
		{
			//test time
			cout << "Now start clock " << clockNumber << " test..." << endl;
			IOHelper::WriteLog("Clock " + to_string(clockNumber) + " test");
			int mode = -1;
			while (mode != 0)
			{
				cout << "Test mode selection : \n1 - view general purpose register set in cpu ; \n2 - view other registers in cpu ; \n3 - view memory unit value ; \n0 - continue to next clock ;\nSelect mode : ";
				cin >> mode;
				switch (mode)
				{
				case 1:
					cout << "Now view general purpose register set in cpu..." << endl;
					IOHelper::WriteLog("Now view general purpose register set in cpu...");
					TestHelper::ViewGeneralPurposeRegisterSet(cpu);
					break;
				case 2:
					cout << "Now view view other registers in cpu..." << endl;
					IOHelper::WriteLog("Now view view other registers in cpu...");
					TestHelper::ViewOtherRegisters(cpu);
					break;
				case 3:
					cout << "Now view memory unit value..." << endl;
					address a;
					cout << "Please enter memory address(dec format) : ";
					cin >> a;
					cout << "Now view memoru unit value near address " << a << "..." << endl;
					IOHelper::WriteLog("Now view memoru unit value near address " + to_string(a) + "...");
					TestHelper::ViewMemoryUnitValue(memory, a);
					break;
				case 0:
					cout << "Now continue to next clock..." << endl;
					IOHelper::WriteLog("Now continue to next clock...");
					break;
				default:
					cout << "Warning! Invalid mode id " << mode << " detected! Please retry..." << endl;
					break;
				}
				cout << endl;
				IOHelper::WriteLog(" ");
			}
		}

		clockNumber++;

	}

	cout << "By.bunnyxt 2018-9-20" << endl;
	IOHelper::WriteLog("By.bunnyxt 2018-9-20\n");

	IOHelper::WriteLog("Close log file.");
	IOHelper::CloseLogFileStream();

	return 0;
}