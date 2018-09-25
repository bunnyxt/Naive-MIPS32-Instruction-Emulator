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
					else if (cpu.GetFw2Index() == cpu.GetMemWbIndex())
					{
						cpu.SetFw2Vacant();
					}
					else
					{
						//no index from found, something occurred
						cout << "[Emulator] Warning! Not found fw which index is " << cpu.GetMemWbIndex() << " to set it vacant." << endl;
						IOHelper::WriteLog("[Emulator] Warning! Not found fw which index is " + to_string(cpu.GetMemWbIndex()) + " to set it vacant.");
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
					cout << "[Emulator] Now load from memory..." << endl;
					IOHelper::WriteLog("[Emulator] Now load from memory...");

					cpu.SetMemWbWord(memory.ReadWord(cpu.GetExMemAddress()));

					//not allow IF
					cout << "[Emulator] Not allow IF in this clock." << endl;
					IOHelper::WriteLog("[Emulator] Not allow IF in this clock.");
					cpu.SetMemIfAllow(1);

					//set FW					
					cout << "[Emulator] Find a vacant fw and store the data in." << endl;
					IOHelper::WriteLog("[Emulator] Find a vacant fw and store the data in.");
					if (cpu.IsFw0Vacant())
					{
						cpu.SetFw0Index(cpu.GetExMemIndex());
						cpu.SetFw0Value(cpu.GetMemWbWord());
					}
					else if (cpu.IsFw1Vacant())
					{
						cpu.SetFw1Index(cpu.GetExMemIndex());
						cpu.SetFw1Value(cpu.GetMemWbWord());
					}
					else if (cpu.IsFw2Vacant())
					{
						cpu.SetFw2Index(cpu.GetExMemIndex());
						cpu.SetFw2Value(cpu.GetMemWbWord());
					}
					else
					{
						//no vacant fw now, something occurred
						cout << "[Emulator] Warning! No vacant fw now." << endl;
						IOHelper::WriteLog("[Emulator] Warning! Not vacant fw now.");
					}
				}
				else if (cpu.GetExMemNeedStore() != 0)
				{
					cout << "[Emulator] Now store to memory..." << endl;
					IOHelper::WriteLog("[Emulator] Now store to memory...");

					memory.WriteWord(cpu.GetExMemAddress(), cpu.GetExMemRegValue());

					//not allow IF
					cout << "[Emulator] Not allow IF in this clock." << endl;
					IOHelper::WriteLog("[Emulator] Not allow IF in this clock.");
					cpu.SetMemIfAllow(1);

					cpu.SetMemWbWord(cpu.GetExMemWord());
				}
				else
				{
					cout << "[Emulator] Not need to load/store now." << endl;
					IOHelper::WriteLog("[Emulator] Not need to load/store now.");

					//allow IF
					cout << "[Emulator] Allow IF in this clock." << endl;
					IOHelper::WriteLog("[Emulator] Allow IF in this clock.");
					cpu.SetMemIfAllow(0);

					cpu.SetMemWbWord(cpu.GetExMemWord());
				}

				cout << "[Emulator] Now transfer data..." << endl;
				IOHelper::WriteLog("[Emulator] Now transfer data...");
				cpu.SetMemWbNeedWriteBack(cpu.GetExMemNeedWriteBack());
				cpu.SetMemWbIndex(cpu.GetExMemIndex());
				cout << "[Emulator] Data transferred done." << endl;
				IOHelper::WriteLog("[Emulator] Data transferred done.");
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
					cout << "[Emulator] Type R instruction detected." << endl;
					IOHelper::WriteLog("[Emulator] Type R instruction detected.");

					//set word
					cpu.SetExMemWord(cpu.GetAlu().CalculateR(cpu.GetIdExRs(), cpu.GetIdExRt(), cpu.GetIdExShamt(), cpu.GetIdExFunc()));

					//set FW
					cout << "[Emulator] Find a vacant fw and store the data in." << endl;
					IOHelper::WriteLog("[Emulator] Find a vacant fw and store the data in.");
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
					else if (cpu.IsFw2Vacant())
					{
						cpu.SetFw2Index(cpu.GetIdExIndex());
						cpu.SetFw2Value(cpu.GetExMemWord());
					}
					else
					{
						//no vacant fw now, something occurred
						cout << "[Emulator] Warning! Not vacant fw now." << endl;
						IOHelper::WriteLog("[Emulator] Warning! Not vacant fw now.");
					}
				}
				else if (cpu.GetIdExTypeI() == 1)
				{
					cout << "[Emulator] Type I instruction detected." << endl;
					IOHelper::WriteLog("[Emulator] Type I instruction detected.");

					switch (cpu.GetIdExOp())
					{
					case 0x0C:
					case 0x0D:
					case 0x0E:
						//andi ori xori

						//set word
						cpu.SetExMemWord(cpu.GetAlu().CalculateI(cpu.GetIdExOp(), cpu.GetIdExRs(), cpu.GetIdExRt(), cpu.GetIdExImmediate()));

						//set FW
						cout << "[Emulator] Find a vacant fw and store the data in." << endl;
						IOHelper::WriteLog("[Emulator] Find a vacant fw and store the data in.");
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
						else if (cpu.IsFw2Vacant())
						{
							cpu.SetFw2Index(cpu.GetIdExIndex());
							cpu.SetFw2Value(cpu.GetExMemWord());
						}
						else
						{
							//no vacant fw now, something occurred
							cout << "[Emulator] Warning! Not vacant fw now." << endl;
							IOHelper::WriteLog("[Emulator] Warning! Not vacant fw now.");
						}

						break;
					case 0x04:
						//beq

						if (cpu.GetIdExRs() == cpu.GetIdExRt())
						{
							cout << "[Emulator] Branch instruction beq detected and transition condition satisfied!" << endl;
							IOHelper::WriteLog("[Emulator] Branch instruction beq detected and transition condition satisfied!");

							cpu.SetPc(cpu.GetIdExPc() + 4 + ConvertHelper::GetSignExtendWord(cpu.GetIdExImmediate() << 2));

							//reset
							cout << "[Emulator] Reset left process in this clock." << endl;
							IOHelper::WriteLog("[Emulator] Reset left process in this clock.");
							throw 2;
						}
						else
						{
							cout << "[Emulator] Branch instruction beq detected but transition condition unsatisfied." << endl;
							IOHelper::WriteLog("[Emulator] Branch instruction beq detected but transition condition unsatisfied.");
						}

						break;
					case 0x05:
						//bne

						if (cpu.GetIdExRs() != cpu.GetIdExRt())
						{
							cout << "[Emulator] Branch instruction bne detected and transition condition satisfied!" << endl;
							IOHelper::WriteLog("[Emulator] Branch instruction bne detected and transition condition satisfied!");

							cpu.SetPc(cpu.GetIdExPc() + 4 + ConvertHelper::GetSignExtendWord(cpu.GetIdExImmediate() << 2));

							//reset
							cout << "[Emulator] Reset left process in this clock." << endl;
							IOHelper::WriteLog("[Emulator] Reset left process in this clock.");
							throw 2;
						}
						else
						{
							cout << "[Emulator] Branch instruction bne detected but transition condition unsatisfied." << endl;
							IOHelper::WriteLog("[Emulator] Branch instruction bne detected but transition condition unsatisfied.");
						}

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
					cout << "[Emulator] Type I instruction detected." << endl;
					IOHelper::WriteLog("[Emulator] Type I instruction detected.");
				}
				else
				{
					//not support
					cout << "[Emulator] Error! No valid instruction type detected! Now exit..." << endl;
					IOHelper::WriteLog("[Emulator] Error! No valid instruction type detected! Now exit...");
					exit(0);
				}

				//trans regs
				cout << "[Emulator] Now transfer data..." << endl;
				IOHelper::WriteLog("[Emulator] Now transfer data...");
				cpu.SetExMemNeedLoad(cpu.GetIdExNeedLoad());
				cpu.SetExMemNeedStore(cpu.GetIdExNeedStore());
				cpu.SetExMemRegValue(cpu.GetIdExRegValue());
				cpu.SetExMemNeedWriteBack(cpu.GetIdExNeedWriteBack());
				cpu.SetExMemIndex(cpu.GetIdExIndex());
				cout << "[Emulator] Data transferred done." << endl;
				IOHelper::WriteLog("[Emulator] Data transferred done.");
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
				cout << "[Emulator] Now category instruction loaded in IR..." << endl;
				IOHelper::WriteLog("[Emulator] Now category instruction loaded in IR...");
				switch (cpu.GetDecoder().GetOp(cpu.GetIr()))
				{
				case 0x00:
					cout << "[Emulator] Op is 0x00, Type R instruction ensured." << endl;
					IOHelper::WriteLog("[Emulator] Op is 0x00, Type R instruction ensured.");

					//type R
					cpu.SetIdExTypeR(1);

					//set op
					cpu.SetIdExOp(0);

					//set func
					switch (cpu.GetDecoder().GetFunc(cpu.GetIr()))
					{
					case 0x21:
						//addu
						cout << "[Emulator] Func is 0x21, addu ensured." << endl;
						IOHelper::WriteLog("[Emulator] Func is 0x21, addu ensured.");
						cpu.SetIdExFunc(0x21);
						break;
					case 0x23:
						//subu
						cout << "[Emulator] Func is 0x23, subu ensured." << endl;
						IOHelper::WriteLog("[Emulator] Func is 0x23, subu ensured.");
						cpu.SetIdExFunc(0x23);
						break;
					case 0x24:
						//and
						cout << "[Emulator] Func is 0x24, and ensured." << endl;
						IOHelper::WriteLog("[Emulator] Func is 0x24, and ensured.");
						cpu.SetIdExFunc(0x24);
						break;
					case 0x25:
						//or
						cout << "[Emulator] Func is 0x25, or ensured." << endl;
						IOHelper::WriteLog("[Emulator] Func is 0x25, or ensured.");
						cpu.SetIdExFunc(0x25);
						break;
					case 0x26:
						//xor
						cout << "[Emulator] Func is 0x26, xor ensured." << endl;
						IOHelper::WriteLog("[Emulator] Func is 0x26, xor ensured.");
						cpu.SetIdExFunc(0x26);
						break;
					case 0x27:
						//nor
						cout << "[Emulator] Func is 0x27, nor ensured." << endl;
						IOHelper::WriteLog("[Emulator] Func is 0x27, nor ensured.");
						cpu.SetIdExFunc(0x27);
						break;
					case 0x00:
						//sll
						cout << "[Emulator] Func is 0x00, sll ensured." << endl;
						IOHelper::WriteLog("[Emulator] Func is 0x00, sll ensured.");
						cpu.SetIdExFunc(0x00);
						break;
					case 0x02:
						//srl
						cout << "[Emulator] Func is 0x02, srl ensured." << endl;
						IOHelper::WriteLog("[Emulator] Func is 0x02, srl ensured.");
						cpu.SetIdExFunc(0x02);
						break;
					case 0x08:
						//jr
						cout << "[Emulator] Func is 0x08, jr ensured." << endl;
						IOHelper::WriteLog("[Emulator] Func is 0x08, jr ensured.");
						cpu.SetIdExFunc(0x08);

						cout << "[Emulator] Transition instruction jr detected!" << endl;
						IOHelper::WriteLog("[Emulator] Transition instruction jr detected!");

						cout << "[Emulator] Try get [rs] from fw." << endl;
						IOHelper::WriteLog("[Emulator] Try get [rs] from fw.");
						//can rs get from fw
						if (cpu.GetFw0Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
						{
							cout << "[Emulator] Find [rs] from fw0." << endl;
							IOHelper::WriteLog("[Emulator] Find [rs] from fw0.");
							cpu.SetIdExRs(cpu.GetFw0Value());
						}
						else if (cpu.GetFw1Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
						{
							cout << "[Emulator] Find [rs] from fw1." << endl;
							IOHelper::WriteLog("[Emulator] Find [rs] from fw1.");
							cpu.SetIdExRs(cpu.GetFw1Value());
						}
						else if (cpu.GetFw2Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
						{
							cout << "[Emulator] Find [rs] from fw2." << endl;
							IOHelper::WriteLog("[Emulator] Find [rs] from fw2.");
							cpu.SetIdExRs(cpu.GetFw2Value());
						}
						else
						{
							cout << "[Emulator] Fail to get [rs] from fw." << endl;
							IOHelper::WriteLog("[Emulator] Fail to get [rs] from fw.");

							//is reg locked
							if (cpu.IsRegLocked(cpu.GetDecoder().GetRs(cpu.GetIr())))
							{
								cout << "[Emulator] Register r" << cpu.GetDecoder().GetRs(cpu.GetIr()) << " locked! Now delay..." << endl;
								IOHelper::WriteLog("[Emulator] Register r" + to_string(cpu.GetDecoder().GetRs(cpu.GetIr())) + " locked! Now delay...");

								//delay
								throw 1 + 5;
							}
							else
							{
								cout << "[Emulator] Get [rs] from gprs." << endl;
								IOHelper::WriteLog("[Emulator] Get [rs] from gprs.");

								//set rs
								cpu.SetIdExRs(cpu.GetGeneralPurposeRegisterSet().Get(cpu.GetDecoder().GetRs(cpu.GetIr())));
							}
						}

						//change pc
						cpu.SetPc(cpu.GetIdExRs());

						//reset
						cout << "[Emulator] Reset left process in this clock." << endl;
						IOHelper::WriteLog("[Emulator] Reset left process in this clock.");
						throw 1;

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

					cout << "[Emulator] Final write back register index is in rd here." << endl;
					IOHelper::WriteLog("[Emulator] Final write back register index is in rd here.");

					//set rd
					cpu.SetIdExRd(cpu.GetDecoder().GetRd(cpu.GetIr()));

					//set index
					cpu.SetIdExIndex(cpu.GetDecoder().GetRd(cpu.GetIr()));

					cout << "[Emulator] Try get [rt] from fw." << endl;
					IOHelper::WriteLog("[Emulator] Try get [rt] from fw.");
					//can rt get from fw
					if (cpu.GetFw0Index() == cpu.GetDecoder().GetRt(cpu.GetIr()))
					{
						cout << "[Emulator] Find [rt] from fw0." << endl;
						IOHelper::WriteLog("[Emulator] Find [rt] from fw0.");
						cpu.SetIdExRt(cpu.GetFw0Value());
					}
					else if (cpu.GetFw1Index() == cpu.GetDecoder().GetRt(cpu.GetIr()))
					{
						cout << "[Emulator] Find [rt] from fw1." << endl;
						IOHelper::WriteLog("[Emulator] Find [rt] from fw1.");
						cpu.SetIdExRt(cpu.GetFw1Value());
					}
					else if (cpu.GetFw2Index() == cpu.GetDecoder().GetRt(cpu.GetIr()))
					{
						cout << "[Emulator] Find [rt] from fw2." << endl;
						IOHelper::WriteLog("[Emulator] Find [rt] from fw2.");
						cpu.SetIdExRt(cpu.GetFw2Value());
					}
					else
					{
						cout << "[Emulator] Fail to get [rt] from fw." << endl;
						IOHelper::WriteLog("[Emulator] Fail to get [rt] from fw.");

						//is reg locked
						if (cpu.IsRegLocked(cpu.GetDecoder().GetRt(cpu.GetIr())))
						{
							cout << "[Emulator] Register r" << cpu.GetDecoder().GetRt(cpu.GetIr()) << " locked! Now delay..." << endl;
							IOHelper::WriteLog("[Emulator] Register r" + to_string(cpu.GetDecoder().GetRt(cpu.GetIr())) + " locked! Now delay...");

							//delay
							throw 1 + 5;
						}
						else
						{
							cout << "[Emulator] Get [rt] from gprs." << endl;
							IOHelper::WriteLog("[Emulator] Get [rt] from gprs.");

							//set rt
							cpu.SetIdExRt(cpu.GetGeneralPurposeRegisterSet().Get(cpu.GetDecoder().GetRt(cpu.GetIr())));
						}
					}

					cout << "[Emulator] Try get [rs] from fw." << endl;
					IOHelper::WriteLog("[Emulator] Try get [rs] from fw.");
					//can rs get from fw
					if (cpu.GetFw0Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
					{
						cout << "[Emulator] Find [rs] from fw0." << endl;
						IOHelper::WriteLog("[Emulator] Find [rs] from fw0.");
						cpu.SetIdExRs(cpu.GetFw0Value());
					}
					else if (cpu.GetFw1Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
					{
						cout << "[Emulator] Find [rs] from fw1." << endl;
						IOHelper::WriteLog("[Emulator] Find [rs] from fw1.");
						cpu.SetIdExRs(cpu.GetFw1Value());
					}
					else if (cpu.GetFw2Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
					{
						cout << "[Emulator] Find [rs] from fw2." << endl;
						IOHelper::WriteLog("[Emulator] Find [rs] from fw2.");
						cpu.SetIdExRs(cpu.GetFw2Value());
					}
					else
					{
						cout << "[Emulator] Fail to get [rs] from fw." << endl;
						IOHelper::WriteLog("[Emulator] Fail to get [rs] from fw.");

						//is reg locked
						if (cpu.IsRegLocked(cpu.GetDecoder().GetRs(cpu.GetIr())))
						{
							cout << "[Emulator] Register r" << cpu.GetDecoder().GetRs(cpu.GetIr()) << " locked! Now delay..." << endl;
							IOHelper::WriteLog("[Emulator] Register r" + to_string(cpu.GetDecoder().GetRs(cpu.GetIr())) + " locked! Now delay...");

							//delay
							throw 1 + 5;
						}
						else
						{
							cout << "[Emulator] Get [rs] from gprs." << endl;
							IOHelper::WriteLog("[Emulator] Get [rs] from gprs.");

							//set rs
							cpu.SetIdExRs(cpu.GetGeneralPurposeRegisterSet().Get(cpu.GetDecoder().GetRs(cpu.GetIr())));
						}
					}

					//lock rd
					cout << "[Emulator] Lock register." << endl;
					IOHelper::WriteLog("[Emulator] Lock register.");
					cpu.LockReg(cpu.GetDecoder().GetRd(cpu.GetIr()));

					//set need write back
					cout << "[Emulator] Set need write back." << endl;
					IOHelper::WriteLog("[Emulator] Set need write back.");
					cpu.SetIdExNeedWriteBack(1);

					break;
				case 0x02:
					cout << "[Emulator] Op is 0x02, Type R instruction ensured." << endl;
					IOHelper::WriteLog("[Emulator] Op is 0x02, Type R instruction ensured.");

					//type J
					cpu.SetIdExTypeJ(1);

					//j

					cout << "[Emulator] Transition instruction j detected!" << endl;
					IOHelper::WriteLog("[Emulator] Transition instruction j detected!");

					//change pc
					cpu.SetPc(cpu.GetDecoder().GetAddress(cpu.GetIr()));

					//reset
					cout << "[Emulator] Reset left process in this clock." << endl;
					IOHelper::WriteLog("[Emulator] Reset left process in this clock.");
					throw 1;

					break;
				default:
					cout << "[Emulator] Op is not 0x00 and 0x02, Type I instruction ensured." << endl;
					IOHelper::WriteLog("[Emulator] Op is not 0x00 and 0x02, Type I instruction ensured.");

					//case I
					cpu.SetIdExTypeI(1);

					//set op
					switch (cpu.GetDecoder().GetOp(cpu.GetIr()))
					{
					case 0x0C:
						//andi
						cout << "[Emulator] Op is 0x0C, andi ensured." << endl;
						IOHelper::WriteLog("[Emulator] Op is 0x0C, andi ensured.");
						cpu.SetIdExOp(0x0C);
						break;
					case 0x0D:
						//ori
						cout << "[Emulator] Op is 0x0D, ori ensured." << endl;
						IOHelper::WriteLog("[Emulator] Op is 0x0D, ori ensured.");
						cpu.SetIdExOp(0x0D);
						break;
					case 0x0E:
						//xori
						cout << "[Emulator] Op is 0x0E, xori ensured." << endl;
						IOHelper::WriteLog("[Emulator] Op is 0x0E, xori ensured.");
						cpu.SetIdExOp(0x0E);
						break;
					case 0x04:
						//beq
						cout << "[Emulator] Op is 0x04, beq ensured." << endl;
						IOHelper::WriteLog("[Emulator] Op is 0x04, beq ensured.");
						cpu.SetIdExOp(0x04);
						break;
					case 0x05:
						//bne
						cout << "[Emulator] Op is 0x05, bne ensured." << endl;
						IOHelper::WriteLog("[Emulator] Op is 0x05, bne ensured.");
						cpu.SetIdExOp(0x05);
						break;
					case 0x23:
						//lw
						cout << "[Emulator] Op is 0x23, lw ensured." << endl;
						IOHelper::WriteLog("[Emulator] Op is 0x23, lw ensured.");
						cpu.SetIdExOp(0x23);
						cout << "[Emulator] Set need load." << endl;
						IOHelper::WriteLog("[Emulator] Set need load.");
						cpu.SetIdExNeedLoad(1);
						break;
					case 0x2B:
						//sw
						cout << "[Emulator] Op is 0x2B, sw ensured." << endl;
						IOHelper::WriteLog("[Emulator] Op is 0x2B, sw ensured.");
						cpu.SetIdExOp(0x2B);
						cout << "[Emulator] Set need store." << endl;
						IOHelper::WriteLog("[Emulator] Set need store.");
						cpu.SetIdExNeedStore(1);
						break;
					case 0x3F:
						//end of instructions (customized)
						cout << "[Emulator] End of instructions (customized) detected!" << endl;
						IOHelper::WriteLog("[Emulator] End of instructions (customized) detected!");

						//change eoi flag to true
						eoiDetected = true;

						//reset
						cout << "[Emulator] Reset left process in this clock." << endl;
						IOHelper::WriteLog("[Emulator] Reset left process in this clock.");
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

						cout << "[Emulator] Final write back register index is in rt here." << endl;
						IOHelper::WriteLog("[Emulator] Final write back register index is in rt here.");

						//set rt
						cpu.SetIdExRt(cpu.GetDecoder().GetRt(cpu.GetIr()));

						//set index
						cpu.SetIdExIndex(cpu.GetDecoder().GetRt(cpu.GetIr()));

						//set need write back
						cout << "[Emulator] Set need write back." << endl;
						IOHelper::WriteLog("[Emulator] Set need write back.");
						cpu.SetIdExNeedWriteBack(1);

						cout << "[Emulator] Try get [rs] from fw." << endl;
						IOHelper::WriteLog("[Emulator] Try get [rs] from fw.");
						//can rs get from fw
						if (cpu.GetFw0Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
						{
							cout << "[Emulator] Find [rs] from fw0." << endl;
							IOHelper::WriteLog("[Emulator] Find [rs] from fw0.");
							cpu.SetIdExRs(cpu.GetFw0Value());
						}
						else if (cpu.GetFw1Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
						{
							cout << "[Emulator] Find [rs] from fw1." << endl;
							IOHelper::WriteLog("[Emulator] Find [rs] from fw1.");
							cpu.SetIdExRs(cpu.GetFw1Value());
						}
						else if (cpu.GetFw2Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
						{
							cout << "[Emulator] Find [rs] from fw2." << endl;
							IOHelper::WriteLog("[Emulator] Find [rs] from fw2.");
							cpu.SetIdExRs(cpu.GetFw2Value());
						}
						else
						{
							cout << "[Emulator] Fail to get [rs] from fw." << endl;
							IOHelper::WriteLog("[Emulator] Fail to get [rs] from fw.");

							//is reg locked
							if (cpu.IsRegLocked(cpu.GetDecoder().GetRs(cpu.GetIr())))
							{
								cout << "[Emulator] Register r" << cpu.GetDecoder().GetRs(cpu.GetIr()) << " locked! Now delay..." << endl;
								IOHelper::WriteLog("[Emulator] Register r" + to_string(cpu.GetDecoder().GetRs(cpu.GetIr())) + " locked! Now delay...");

								//delay
								throw 1 + 5;
							}
							else
							{
								cout << "[Emulator] Get [rs] from gprs." << endl;
								IOHelper::WriteLog("[Emulator] Get [rs] from gprs.");

								//set rs
								cpu.SetIdExRs(cpu.GetGeneralPurposeRegisterSet().Get(cpu.GetDecoder().GetRs(cpu.GetIr())));
							}
						}

						//lock rt
						cout << "[Emulator] Lock register." << endl;
						IOHelper::WriteLog("[Emulator] Lock register.");
						cpu.LockReg(cpu.GetDecoder().GetRt(cpu.GetIr()));

						break;
					case 0x04:
					case 0x05:
						//beq bne

						cout << "[Emulator] Try get [rt] from fw." << endl;
						IOHelper::WriteLog("[Emulator] Try get [rt] from fw.");
						//can rt get from fw
						if (cpu.GetFw0Index() == cpu.GetDecoder().GetRt(cpu.GetIr()))
						{
							cout << "[Emulator] Find [rt] from fw0." << endl;
							IOHelper::WriteLog("[Emulator] Find [rt] from fw0.");
							cpu.SetIdExRt(cpu.GetFw0Value());
						}
						else if (cpu.GetFw1Index() == cpu.GetDecoder().GetRt(cpu.GetIr()))
						{
							cout << "[Emulator] Find [rt] from fw1." << endl;
							IOHelper::WriteLog("[Emulator] Find [rt] from fw1.");
							cpu.SetIdExRt(cpu.GetFw1Value());
						}
						else if (cpu.GetFw2Index() == cpu.GetDecoder().GetRt(cpu.GetIr()))
						{
							cout << "[Emulator] Find [rt] from fw2." << endl;
							IOHelper::WriteLog("[Emulator] Find [rt] from fw2.");
							cpu.SetIdExRt(cpu.GetFw2Value());
						}
						else
						{
							cout << "[Emulator] Fail to get [rt] from fw." << endl;
							IOHelper::WriteLog("[Emulator] Fail to get [rt] from fw.");

							//is reg locked
							if (cpu.IsRegLocked(cpu.GetDecoder().GetRt(cpu.GetIr())))
							{
								cout << "[Emulator] Register r" << cpu.GetDecoder().GetRt(cpu.GetIr()) << " locked! Now delay..." << endl;
								IOHelper::WriteLog("[Emulator] Register r" + to_string(cpu.GetDecoder().GetRt(cpu.GetIr())) + " locked! Now delay...");

								//delay
								throw 1 + 5;
							}
							else
							{
								cout << "[Emulator] Get [rt] from gprs." << endl;
								IOHelper::WriteLog("[Emulator] Get [rt] from gprs.");

								//set rt
								cpu.SetIdExRt(cpu.GetGeneralPurposeRegisterSet().Get(cpu.GetDecoder().GetRt(cpu.GetIr())));
							}
						}

						cout << "[Emulator] Try get [rs] from fw." << endl;
						IOHelper::WriteLog("[Emulator] Try get [rs] from fw.");
						//can rs get from fw
						if (cpu.GetFw0Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
						{
							cout << "[Emulator] Find [rs] from fw0." << endl;
							IOHelper::WriteLog("[Emulator] Find [rs] from fw0.");
							cpu.SetIdExRs(cpu.GetFw0Value());
						}
						else if (cpu.GetFw1Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
						{
							cout << "[Emulator] Find [rs] from fw1." << endl;
							IOHelper::WriteLog("[Emulator] Find [rs] from fw1.");
							cpu.SetIdExRs(cpu.GetFw1Value());
						}
						else if (cpu.GetFw2Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
						{
							cout << "[Emulator] Find [rs] from fw2." << endl;
							IOHelper::WriteLog("[Emulator] Find [rs] from fw2.");
							cpu.SetIdExRs(cpu.GetFw2Value());
						}
						else
						{
							cout << "[Emulator] Fail to get [rs] from fw." << endl;
							IOHelper::WriteLog("[Emulator] Fail to get [rs] from fw.");

							//is reg locked
							if (cpu.IsRegLocked(cpu.GetDecoder().GetRs(cpu.GetIr())))
							{
								cout << "[Emulator] Register r" << cpu.GetDecoder().GetRs(cpu.GetIr()) << " locked! Now delay..." << endl;
								IOHelper::WriteLog("[Emulator] Register r" + to_string(cpu.GetDecoder().GetRs(cpu.GetIr())) + " locked! Now delay...");

								//delay
								throw 1 + 5;
							}
							else
							{
								cout << "[Emulator] Get [rs] from gprs." << endl;
								IOHelper::WriteLog("[Emulator] Get [rs] from gprs.");

								//set rs
								cpu.SetIdExRs(cpu.GetGeneralPurposeRegisterSet().Get(cpu.GetDecoder().GetRs(cpu.GetIr())));
							}
						}

						break;
					case 0x23:
						//lw

						cout << "[Emulator] Final write back register index is in rt here." << endl;
						IOHelper::WriteLog("[Emulator] Final write back register index is in rt here.");

						//set rt
						cpu.SetIdExRt(cpu.GetDecoder().GetRt(cpu.GetIr()));

						//set index
						cpu.SetIdExIndex(cpu.GetDecoder().GetRt(cpu.GetIr()));

						//set need write back
						cout << "[Emulator] Set need write back." << endl;
						IOHelper::WriteLog("[Emulator] Set need write back.");
						cpu.SetIdExNeedWriteBack(1);

						cout << "[Emulator] Try get [rs] from fw." << endl;
						IOHelper::WriteLog("[Emulator] Try get [rs] from fw.");
						//can rs get from fw
						if (cpu.GetFw0Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
						{
							cout << "[Emulator] Find [rs] from fw0." << endl;
							IOHelper::WriteLog("[Emulator] Find [rs] from fw0.");
							cpu.SetIdExRs(cpu.GetFw0Value());
						}
						else if (cpu.GetFw1Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
						{
							cout << "[Emulator] Find [rs] from fw1." << endl;
							IOHelper::WriteLog("[Emulator] Find [rs] from fw1.");
							cpu.SetIdExRs(cpu.GetFw1Value());
						}
						else if (cpu.GetFw2Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
						{
							cout << "[Emulator] Find [rs] from fw2." << endl;
							IOHelper::WriteLog("[Emulator] Find [rs] from fw2.");
							cpu.SetIdExRs(cpu.GetFw2Value());
						}
						else
						{
							cout << "[Emulator] Fail to get [rs] from fw." << endl;
							IOHelper::WriteLog("[Emulator] Fail to get [rs] from fw.");

							//is reg locked
							if (cpu.IsRegLocked(cpu.GetDecoder().GetRs(cpu.GetIr())))
							{
								cout << "[Emulator] Register r" << cpu.GetDecoder().GetRs(cpu.GetIr()) << " locked! Now delay..." << endl;
								IOHelper::WriteLog("[Emulator] Register r" + to_string(cpu.GetDecoder().GetRs(cpu.GetIr())) + " locked! Now delay...");

								//delay
								throw 1 + 5;
							}
							else
							{
								cout << "[Emulator] Get [rs] from gprs." << endl;
								IOHelper::WriteLog("[Emulator] Get [rs] from gprs.");

								//set rs
								cpu.SetIdExRs(cpu.GetGeneralPurposeRegisterSet().Get(cpu.GetDecoder().GetRs(cpu.GetIr())));
							}
						}

						//lock rt
						cout << "[Emulator] Lock register." << endl;
						IOHelper::WriteLog("[Emulator] Lock register.");
						cpu.LockReg(cpu.GetDecoder().GetRt(cpu.GetIr()));

						break;
					case 0x2B:
						//sw

						cout << "[Emulator] Try get [rt] from fw." << endl;
						IOHelper::WriteLog("[Emulator] Try get [rt] from fw.");
						//can rt get from fw
						if (cpu.GetFw0Index() == cpu.GetDecoder().GetRt(cpu.GetIr()))
						{
							cout << "[Emulator] Find [rt] from fw0." << endl;
							IOHelper::WriteLog("[Emulator] Find [rt] from fw0.");
							cpu.SetIdExRt(cpu.GetFw0Value());
						}
						else if (cpu.GetFw1Index() == cpu.GetDecoder().GetRt(cpu.GetIr()))
						{
							cout << "[Emulator] Find [rt] from fw1." << endl;
							IOHelper::WriteLog("[Emulator] Find [rt] from fw1.");
							cpu.SetIdExRt(cpu.GetFw1Value());
						}
						else if (cpu.GetFw2Index() == cpu.GetDecoder().GetRt(cpu.GetIr()))
						{
							cout << "[Emulator] Find [rt] from fw2." << endl;
							IOHelper::WriteLog("[Emulator] Find [rt] from fw2.");
							cpu.SetIdExRt(cpu.GetFw2Value());
						}
						else
						{
							cout << "[Emulator] Fail to get [rt] from fw." << endl;
							IOHelper::WriteLog("[Emulator] Fail to get [rt] from fw.");

							//is reg locked
							if (cpu.IsRegLocked(cpu.GetDecoder().GetRt(cpu.GetIr())))
							{
								cout << "[Emulator] Register r" << cpu.GetDecoder().GetRt(cpu.GetIr()) << " locked! Now delay..." << endl;
								IOHelper::WriteLog("[Emulator] Register r" + to_string(cpu.GetDecoder().GetRt(cpu.GetIr())) + " locked! Now delay...");

								//delay
								throw 1 + 5;
							}
							else
							{
								cout << "[Emulator] Get [rt] from gprs." << endl;
								IOHelper::WriteLog("[Emulator] Get [rt] from gprs.");

								//set rt
								cpu.SetIdExRt(cpu.GetGeneralPurposeRegisterSet().Get(cpu.GetDecoder().GetRt(cpu.GetIr())));
							}
						}

						cout << "[Emulator] Set value which to store to memory." << endl;
						IOHelper::WriteLog("[Emulator] Set value which to store to memory.");

						//set reg value
						cpu.SetIdExRegValue(cpu.GetIdExRt());

						cout << "[Emulator] Try get [rs] from fw." << endl;
						IOHelper::WriteLog("[Emulator] Try get [rs] from fw.");
						//can rs get from fw
						if (cpu.GetFw0Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
						{
							cout << "[Emulator] Find [rs] from fw0." << endl;
							IOHelper::WriteLog("[Emulator] Find [rs] from fw0.");
							cpu.SetIdExRs(cpu.GetFw0Value());
						}
						else if (cpu.GetFw1Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
						{
							cout << "[Emulator] Find [rs] from fw1." << endl;
							IOHelper::WriteLog("[Emulator] Find [rs] from fw1.");
							cpu.SetIdExRs(cpu.GetFw1Value());
						}
						else if (cpu.GetFw2Index() == cpu.GetDecoder().GetRs(cpu.GetIr()))
						{
							cout << "[Emulator] Find [rs] from fw2." << endl;
							IOHelper::WriteLog("[Emulator] Find [rs] from fw2.");
							cpu.SetIdExRs(cpu.GetFw2Value());
						}
						else
						{
							cout << "[Emulator] Fail to get [rs] from fw." << endl;
							IOHelper::WriteLog("[Emulator] Fail to get [rs] from fw.");

							//is reg locked
							if (cpu.IsRegLocked(cpu.GetDecoder().GetRs(cpu.GetIr())))
							{
								cout << "[Emulator] Register r" << cpu.GetDecoder().GetRs(cpu.GetIr()) << " locked! Now delay..." << endl;
								IOHelper::WriteLog("[Emulator] Register r" + to_string(cpu.GetDecoder().GetRs(cpu.GetIr())) + " locked! Now delay...");

								//delay
								throw 1 + 5;
							}
							else
							{
								cout << "[Emulator] Get [rs] from gprs." << endl;
								IOHelper::WriteLog("[Emulator] Get [rs] from gprs.");

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

				//trans regs
				cout << "[Emulator] Now transfer data..." << endl;
				IOHelper::WriteLog("[Emulator] Now transfer data...");
				cpu.SetIdExPc(cpu.GetIfIdPc());
				cout << "[Emulator] Data transferred done." << endl;
				IOHelper::WriteLog("[Emulator] Data transferred done.");
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
					cout << "[Emulator] Find that load/store done in this clock. Now delay..." << endl;
					IOHelper::WriteLog("[Emulator] Find that load/store done in this clock. Now delay...");

					//allow IF
					cout << "[Emulator] Allow IF in next clock if possible." << endl;
					IOHelper::WriteLog("[Emulator] Allow IF in next clock if possible.");
					cpu.SetMemIfAllow(0);

					//delay
					throw 0 + 5;
				}
				else
				{
					//fetch instruction from memory to IR
					cout << "[Emulator] Fetch instruction from memory to IR." << endl;
					IOHelper::WriteLog("[Emulator] Fetch instruction from memory to IR.");
					cpu.SetIr(memory.ReadWord(cpu.GetPc()));

					//set instruction pc
					cpu.SetIfIdPc(cpu.GetPc());

					//modify pc
					cout << "[Emulator] PC + 1" << endl;
					IOHelper::WriteLog("[Emulator] PC + 1");
					cpu.SetPc(cpu.GetPc() + 4);
				}
			}
			cout << "[Emulator] -------------------- " << clockNumber << " - IF end --------------------" << endl;
			IOHelper::WriteLog("[Emulator] -------------------- " + to_string(clockNumber) + " - IF end --------------------");

		}
		catch (int index)
		{
			cout << "[Emulator] Now set run status..." << endl;
			IOHelper::WriteLog("[Emulator] Now set run status...");

			//reset process
			switch (index % 5)
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

			//delay process
			switch (index)
			{
			case 4 + 5:
				cpu.SetRunDone(3);
			case 3 + 5:
				cpu.SetRunDone(2);
			case 2 + 5:
				cpu.SetRunDone(1);
			case 1 + 5:
				cpu.SetRunDone(0);
			case 0 + 5:
				break;
			default:
				break;
			}
		}

		//set next clock time ready
		cout << "[Emulator] Now set next clock time ready..." << endl;
		IOHelper::WriteLog("[Emulator] Now set next clock time ready...");
		cpu.SetNewReady();

		//end of instructions detected
		if (eoiDetected)
		{
			cout << "[Emulator] Eoi detected, no more valid instructions to run." << endl;
			IOHelper::WriteLog("[Emulator] Eoi detected, no more valid instructions to run.");
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
	IOHelper::WriteLog("[Emulator] By.bunnyxt 2018-9-24\n");

	IOHelper::WriteLog("[Emulator] Close log file.");
	IOHelper::CloseLogFileStream();

	return 0;
}