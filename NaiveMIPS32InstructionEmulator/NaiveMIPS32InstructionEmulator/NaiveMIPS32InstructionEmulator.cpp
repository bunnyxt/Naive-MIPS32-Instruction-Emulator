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
	IOHelper::LoadRegisterData(cpu.GetGeneralPurposeRegisterSet(), registerDataFileIn);
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

		cpu.ResetRunStatus();

		try
		{
			//WB
			cout << "[Emulator] -------------------- " << clockNumber << " - WB begin --------------------" << endl;
			IOHelper::WriteLog("[Emulator] -------------------- " + to_string(clockNumber) + " - WB begin --------------------");
			if (!cpu.IsReady(4))
			{
				cout << "[Emulator] No work now." << endl;
				IOHelper::WriteLog("[Emulator] No work now.");
				cpu.SetRunInterrupted(4);
			}
			else
			{
				if (cpu.GetMemWbNeedWriteBack() == 0)
				{
					cout << "[Emulator] Not need to write back now." << endl;
					IOHelper::WriteLog("[Emulator] Not need to write back now.");
				}
				else
				{
					//write back
					cout << "[Emulator] Write word " << ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetMemWbWord())) << "(" << cpu.GetMemWbWord() << ") back to r" << cpu.GetMemWbIndex() << "." << endl;
					IOHelper::WriteLog("[Emulator] Write word " + ConvertHelper::SeperateString(ConvertHelper::WordToString(cpu.GetMemWbWord())) + "(" + to_string(cpu.GetMemWbWord()) + ") back to r" + to_string(cpu.GetMemWbIndex()) + ".");
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
			cout << "[Emulator] -------------------- " << clockNumber << " - WB end --------------------" << endl;
			IOHelper::WriteLog("[Emulator] -------------------- " + to_string(clockNumber) + " - WB end --------------------");

			//MEM
			cout << "[Emulator] -------------------- " << clockNumber << " - MEM begin --------------------" << endl;
			IOHelper::WriteLog("[Emulator] -------------------- " + to_string(clockNumber) + " - MEM begin --------------------");
			if (!cpu.IsReady(3))
			{
				cout << "[Emulator] No work now." << endl;
				IOHelper::WriteLog("[Emulator] No work now.");
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
			cout << "[Emulator] -------------------- " << clockNumber << " - MEM end --------------------" << endl;
			IOHelper::WriteLog("[Emulator] -------------------- " + to_string(clockNumber) + " - MEM end --------------------");

			//EX
			cout << "[Emulator] --------------------" << clockNumber << " - EX begin --------------------" << endl;
			IOHelper::WriteLog("[Emulator] -------------------- " + to_string(clockNumber) + " - EX begin --------------------");
			if (!cpu.IsReady(2))
			{
				cout << "[Emulator] No work now." << endl;
				IOHelper::WriteLog("[Emulator] No work now.");
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
					cout << "[Emulator] Error! No valid instruction type detected! Now exit..." << endl;
					IOHelper::WriteLog("[Emulator] Error! No valid instruction type detected! Now exit...");
					exit(0);
				}

				//trans regs
				cpu.SetExMemNeedLoad(cpu.GetIdExNeedLoad());
				cpu.SetExMemNeedStore(cpu.GetIdExNeedStore());
				cpu.SetExMemRegValue(cpu.GetIdExRegValue());
				cpu.SetExMemNeedWriteBack(cpu.GetIdExNeedWriteBack());
				cpu.SetExMemIndex(cpu.GetIdExIndex());
			}
			cout << "[Emulator] -------------------- " << clockNumber << " - EX end --------------------" << endl;
			IOHelper::WriteLog("[Emulator] -------------------- " + to_string(clockNumber) + " - EX end --------------------");

			//ID
			cout << "[Emulator] -------------------- " << clockNumber << " - ID begin --------------------" << endl;
			IOHelper::WriteLog("[Emulator] -------------------- " + to_string(clockNumber) + " - ID begin -------------------- ");
			if (!cpu.IsReady(1))
			{
				cout << "[Emulator] No work now." << endl;
				IOHelper::WriteLog("[Emulator] No work now.");
				cpu.SetRunInterrupted(1);
			}
			else
			{
				//clear reg
				cout << "[Emulator] Now clear regsiters..." << endl;
				IOHelper::WriteLog("[Emulator] Now clear registers...");
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
				cpu.SetIdExRegValue(0);
				cpu.SetIdExNeedWriteBack(0);
				cpu.SetIdExIndex(0);
				cout << "[Emulator] Clear registers done." << endl;
				IOHelper::WriteLog("[Emulator] Clear registers done.");

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
						cout << "[Emulator] Error! Invalid instruction " << ConvertHelper::InstructionToString(cpu.GetIr()) << " detected! Now exit..." << endl;
						IOHelper::WriteLog("[Emulator] Error! Invalid instruction " + ConvertHelper::InstructionToString(cpu.GetIr()) + " detected! Now exit...");
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
					case 0x3F:
						//end of instructions (customized)
						cout << "[Emulator] End of instructions (customized) detected!" << endl;
						IOHelper::WriteLog("[Emulator] End of instructions (customized) detected!");
						eoiDetected = true;
						throw 1;
						break;
					default:
						//not support
						cout << "[Emulator] Error! Invalid instruction " << ConvertHelper::InstructionToString(cpu.GetIr()) << " detected! Now exit..." << endl;
						IOHelper::WriteLog("[Emulator] Error! Invalid instruction " + ConvertHelper::InstructionToString(cpu.GetIr()) + " detected! Now exit...");
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
			cout << "[Emulator] -------------------- " << clockNumber << " - ID end --------------------" << endl;
			IOHelper::WriteLog("[Emulator] -------------------- " + to_string(clockNumber) + " - ID end --------------------");

			//IF
			cout << "[Emulator] -------------------- " << clockNumber << " - IF begin --------------------" << endl;
			IOHelper::WriteLog("[Emulator] -------------------- " + to_string(clockNumber) + " - IF begin --------------------");
			if (!cpu.IsReady(0))
			{
				cout << "[Emulator] No work now." << endl;
				IOHelper::WriteLog("[Emulator] No work now.");
				cpu.SetRunInterrupted(0);
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
			cout << "[Emulator] -------------------- " << clockNumber << " - IF end --------------------" << endl;
			IOHelper::WriteLog("[Emulator] -------------------- " + to_string(clockNumber) + " - IF end --------------------");

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

		//set next clock time ready
		cpu.SetNewReady();

		//end of instructions detected
		if (eoiDetected)
		{
			cpu.SetNotReady(0);
		}

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
				cout << "[Emulator] Test mode selection : \n1 - view general purpose register set in cpu ; \n2 - view other registers in cpu ; \n3 - view memory unit value ; \n0 - continue to next clock ;\nSelect mode : ";
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
				cout << "[Emulator] Test mode selection : \n1 - view general purpose register set in cpu ; \n2 - view other registers in cpu ; \n3 - view memory unit value ; \n0 - exit last test ;\nSelect mode : ";
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

	cout << "[Emulator] By.bunnyxt 2018-9-20" << endl;
	IOHelper::WriteLog("[Emulator] By.bunnyxt 2018-9-20\n");

	IOHelper::WriteLog("[Emulator] Close log file.");
	IOHelper::CloseLogFileStream();

	return 0;
}