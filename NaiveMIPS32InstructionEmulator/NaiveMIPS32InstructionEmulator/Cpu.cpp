#include "NaiveMIPS32InstructionEmulator.h"

Cpu::Cpu()
{
	cout << "[Cpu] Initialize Cpu..." << endl;
	IOHelper::WriteLog("[Cpu] Initialize Cpu...");

	//initialize ready status
	for (int i = 4; i > 0; i--)
	{
		SetNotReady(i);
	}
	SetReady(0);

	//initialize fw
	SetFw0Vacant();
	SetFw1Vacant();
	SetFw2Vacant();

	cout << "[Cpu] Cpu initialized." << endl;
	IOHelper::WriteLog("[Cpu] Cpu initialized.");
}

void Cpu::Run(Memory & memory, bool &eoiDetected, int clockNumber)
{

	ResetRunStatus();

	try
	{
		//WB
		cout << "[Cpu-Run] -------------------- " << clockNumber << " - WB begin --------------------" << endl;
		IOHelper::WriteLog("[Cpu-Run] -------------------- " + to_string(clockNumber) + " - WB begin --------------------");
		if (!IsReady(4))
		{
			cout << "[Cpu-Run] No work now." << endl;
			IOHelper::WriteLog("[Cpu-Run] No work now.");
			SetRunInterrupted(4);
		}
		else
		{
			if (GetMemWbNeedWriteBack() == 0)
			{
				cout << "[Cpu-Run] Not need to write back now." << endl;
				IOHelper::WriteLog("[Cpu-Run] Not need to write back now.");
			}
			else
			{
				//write back
				cout << "[Cpu-Run] Write word " << ConvertHelper::SeperateString(ConvertHelper::WordToString(GetMemWbWord())) << "(" << GetMemWbWord() << ") back to r" << GetMemWbIndex() << "." << endl;
				IOHelper::WriteLog("[Cpu-Run] Write word " + ConvertHelper::SeperateString(ConvertHelper::WordToString(GetMemWbWord())) + "(" + to_string(GetMemWbWord()) + ") back to r" + to_string(GetMemWbIndex()) + ".");
				GetGeneralPurposeRegisterSet().Set(GetMemWbIndex(), GetMemWbWord());

				//unlock reg
				UnlockReg(GetMemWbIndex());

				//free fw
				if (GetFw0Index() == GetMemWbIndex())
				{
					SetFw0Vacant();
				}
				else if (GetFw1Index() == GetMemWbIndex())
				{
					SetFw1Vacant();
				}
				else if (GetFw2Index() == GetMemWbIndex())
				{
					SetFw2Vacant();
				}
				else
				{
					//no index from found, something occurred
					cout << "[Cpu-Run] Warning! Not found fw which index is " << GetMemWbIndex() << " to set it vacant." << endl;
					IOHelper::WriteLog("[Cpu-Run] Warning! Not found fw which index is " + to_string(GetMemWbIndex()) + " to set it vacant.");
				}
			}
		}
		cout << "[Cpu-Run] -------------------- " << clockNumber << " - WB end --------------------" << endl;
		IOHelper::WriteLog("[Cpu-Run] -------------------- " + to_string(clockNumber) + " - WB end --------------------");

		//MEM
		cout << "[Cpu-Run] -------------------- " << clockNumber << " - MEM begin --------------------" << endl;
		IOHelper::WriteLog("[Cpu-Run] -------------------- " + to_string(clockNumber) + " - MEM begin --------------------");
		if (!IsReady(3))
		{
			cout << "[Cpu-Run] No work now." << endl;
			IOHelper::WriteLog("[Cpu-Run] No work now.");
			SetRunInterrupted(3);
		}
		else
		{
			if (GetExMemNeedLoad() != 0)
			{
				cout << "[Cpu-Run] Now load from memory..." << endl;
				IOHelper::WriteLog("[Cpu-Run] Now load from memory...");

				SetMemWbWord(memory.ReadWord(GetExMemAddress()));

				//not allow IF
				cout << "[Cpu-Run] Not allow IF in this clock." << endl;
				IOHelper::WriteLog("[Cpu-Run] Not allow IF in this clock.");
				SetMemIfAllow(1);

				//set FW					
				cout << "[Cpu-Run] Find a vacant fw and store the data in." << endl;
				IOHelper::WriteLog("[Cpu-Run] Find a vacant fw and store the data in.");
				if (IsFw0Vacant())
				{
					SetFw0Index(GetExMemIndex());
					SetFw0Value(GetMemWbWord());
				}
				else if (IsFw1Vacant())
				{
					SetFw1Index(GetExMemIndex());
					SetFw1Value(GetMemWbWord());
				}
				else if (IsFw2Vacant())
				{
					SetFw2Index(GetExMemIndex());
					SetFw2Value(GetMemWbWord());
				}
				else
				{
					//no vacant fw now, something occurred
					cout << "[Cpu-Run] Warning! No vacant fw now." << endl;
					IOHelper::WriteLog("[Cpu-Run] Warning! Not vacant fw now.");
				}
			}
			else if (GetExMemNeedStore() != 0)
			{
				cout << "[Cpu-Run] Now store to memory..." << endl;
				IOHelper::WriteLog("[Cpu-Run] Now store to memory...");

				memory.WriteWord(GetExMemAddress(), GetExMemRegValue());

				//not allow IF
				cout << "[Cpu-Run] Not allow IF in this clock." << endl;
				IOHelper::WriteLog("[Cpu-Run] Not allow IF in this clock.");
				SetMemIfAllow(1);

				SetMemWbWord(GetExMemWord());
			}
			else
			{
				cout << "[Cpu-Run] Not need to load/store now." << endl;
				IOHelper::WriteLog("[Cpu-Run] Not need to load/store now.");

				//allow IF
				cout << "[Cpu-Run] Allow IF in this clock." << endl;
				IOHelper::WriteLog("[Cpu-Run] Allow IF in this clock.");
				SetMemIfAllow(0);

				SetMemWbWord(GetExMemWord());
			}

			cout << "[Cpu-Run] Now transfer data..." << endl;
			IOHelper::WriteLog("[Cpu-Run] Now transfer data...");
			SetMemWbNeedWriteBack(GetExMemNeedWriteBack());
			SetMemWbIndex(GetExMemIndex());
			cout << "[Cpu-Run] Data transferred done." << endl;
			IOHelper::WriteLog("[Cpu-Run] Data transferred done.");
		}
		cout << "[Cpu-Run] -------------------- " << clockNumber << " - MEM end --------------------" << endl;
		IOHelper::WriteLog("[Cpu-Run] -------------------- " + to_string(clockNumber) + " - MEM end --------------------");

		//EX
		cout << "[Cpu-Run] --------------------" << clockNumber << " - EX begin --------------------" << endl;
		IOHelper::WriteLog("[Cpu-Run] -------------------- " + to_string(clockNumber) + " - EX begin --------------------");
		if (!IsReady(2))
		{
			cout << "[Cpu-Run] No work now." << endl;
			IOHelper::WriteLog("[Cpu-Run] No work now.");
			SetRunInterrupted(2);
		}
		else
		{
			//get instruction type
			if (GetIdExTypeR() == 1)
			{
				cout << "[Cpu-Run] Type R instruction detected." << endl;
				IOHelper::WriteLog("[Cpu-Run] Type R instruction detected.");

				//set word
				SetExMemWord(GetAlu().CalculateR(GetIdExRs(), GetIdExRt(), GetIdExShamt(), GetIdExFunc()));

				//set FW
				cout << "[Cpu-Run] Find a vacant fw and store the data in." << endl;
				IOHelper::WriteLog("[Cpu-Run] Find a vacant fw and store the data in.");
				if (IsFw0Vacant())
				{
					SetFw0Index(GetIdExIndex());
					SetFw0Value(GetExMemWord());
				}
				else if (IsFw1Vacant())
				{
					SetFw1Index(GetIdExIndex());
					SetFw1Value(GetExMemWord());
				}
				else if (IsFw2Vacant())
				{
					SetFw2Index(GetIdExIndex());
					SetFw2Value(GetExMemWord());
				}
				else
				{
					//no vacant fw now, something occurred
					cout << "[Cpu-Run] Warning! Not vacant fw now." << endl;
					IOHelper::WriteLog("[Cpu-Run] Warning! Not vacant fw now.");
				}
			}
			else if (GetIdExTypeI() == 1)
			{
				cout << "[Cpu-Run] Type I instruction detected." << endl;
				IOHelper::WriteLog("[Cpu-Run] Type I instruction detected.");

				switch (GetIdExOp())
				{
				case 0x0C:
				case 0x0D:
				case 0x0E:
					//andi ori xori

					//set word
					SetExMemWord(GetAlu().CalculateI(GetIdExOp(), GetIdExRs(), GetIdExRt(), GetIdExImmediate()));

					//set FW
					cout << "[Cpu-Run] Find a vacant fw and store the data in." << endl;
					IOHelper::WriteLog("[Cpu-Run] Find a vacant fw and store the data in.");
					if (IsFw0Vacant())
					{
						SetFw0Index(GetIdExIndex());
						SetFw0Value(GetExMemWord());
					}
					else if (IsFw1Vacant())
					{
						SetFw1Index(GetIdExIndex());
						SetFw1Value(GetExMemWord());
					}
					else if (IsFw2Vacant())
					{
						SetFw2Index(GetIdExIndex());
						SetFw2Value(GetExMemWord());
					}
					else
					{
						//no vacant fw now, something occurred
						cout << "[Cpu-Run] Warning! Not vacant fw now." << endl;
						IOHelper::WriteLog("[Cpu-Run] Warning! Not vacant fw now.");
					}

					break;
				case 0x04:
					//beq

					if (GetIdExRs() == GetIdExRt())
					{
						cout << "[Cpu-Run] Branch instruction beq detected and transition condition satisfied!" << endl;
						IOHelper::WriteLog("[Cpu-Run] Branch instruction beq detected and transition condition satisfied!");

						SetPc(GetIdExPc() + 4 + ConvertHelper::GetSignExtendWord(GetIdExImmediate() << 2));

						//reset
						cout << "[Cpu-Run] Reset left process in this clock." << endl;
						IOHelper::WriteLog("[Cpu-Run] Reset left process in this clock.");
						throw 2;
					}
					else
					{
						cout << "[Cpu-Run] Branch instruction beq detected but transition condition unsatisfied." << endl;
						IOHelper::WriteLog("[Cpu-Run] Branch instruction beq detected but transition condition unsatisfied.");
					}

					break;
				case 0x05:
					//bne

					if (GetIdExRs() != GetIdExRt())
					{
						cout << "[Cpu-Run] Branch instruction bne detected and transition condition satisfied!" << endl;
						IOHelper::WriteLog("[Cpu-Run] Branch instruction bne detected and transition condition satisfied!");

						SetPc(GetIdExPc() + 4 + ConvertHelper::GetSignExtendWord(GetIdExImmediate() << 2));

						//reset
						cout << "[Cpu-Run] Reset left process in this clock." << endl;
						IOHelper::WriteLog("[Cpu-Run] Reset left process in this clock.");
						throw 2;
					}
					else
					{
						cout << "[Cpu-Run] Branch instruction bne detected but transition condition unsatisfied." << endl;
						IOHelper::WriteLog("[Cpu-Run] Branch instruction bne detected but transition condition unsatisfied.");
					}

					break;
				case 0x23:
					//lw

					//set address
					SetExMemAddress(GetAlu().CalculateI(GetIdExOp(), GetIdExRs(), GetIdExRt(), GetIdExImmediate()));

					break;
				case 0x2B:
					//sw

					//set address
					SetExMemAddress(GetAlu().CalculateI(GetIdExOp(), GetIdExRs(), GetIdExRt(), GetIdExImmediate()));

					break;
				default:
					break;
				}
			}
			else if (GetIdExTypeJ() == 1)
			{
				cout << "[Cpu-Run] Type I instruction detected." << endl;
				IOHelper::WriteLog("[Cpu-Run] Type I instruction detected.");
			}
			else
			{
				//not support
				cout << "[Cpu-Run] Error! No valid instruction type detected! Now exit..." << endl;
				IOHelper::WriteLog("[Cpu-Run] Error! No valid instruction type detected! Now exit...");
				exit(0);
			}

			//trans regs
			cout << "[Cpu-Run] Now transfer data..." << endl;
			IOHelper::WriteLog("[Cpu-Run] Now transfer data...");
			SetExMemNeedLoad(GetIdExNeedLoad());
			SetExMemNeedStore(GetIdExNeedStore());
			SetExMemRegValue(GetIdExRegValue());
			SetExMemNeedWriteBack(GetIdExNeedWriteBack());
			SetExMemIndex(GetIdExIndex());
			cout << "[Cpu-Run] Data transferred done." << endl;
			IOHelper::WriteLog("[Cpu-Run] Data transferred done.");
		}
		cout << "[Cpu-Run] -------------------- " << clockNumber << " - EX end --------------------" << endl;
		IOHelper::WriteLog("[Cpu-Run] -------------------- " + to_string(clockNumber) + " - EX end --------------------");

		//ID
		cout << "[Cpu-Run] -------------------- " << clockNumber << " - ID begin --------------------" << endl;
		IOHelper::WriteLog("[Cpu-Run] -------------------- " + to_string(clockNumber) + " - ID begin -------------------- ");
		if (!IsReady(1))
		{
			cout << "[Cpu-Run] No work now." << endl;
			IOHelper::WriteLog("[Cpu-Run] No work now.");
			SetRunInterrupted(1);
		}
		else
		{
			//clear reg
			cout << "[Cpu-Run] Now clear regsiters..." << endl;
			IOHelper::WriteLog("[Cpu-Run] Now clear registers...");
			SetIdExTypeR(0);
			SetIdExTypeJ(0);
			SetIdExTypeI(0);
			SetIdExOp(0);
			SetIdExRs(0);
			SetIdExRt(0);
			SetIdExRd(0);
			SetIdExShamt(0);
			SetIdExFunc(0);
			SetIdExImmediate(0);
			SetIdExAddress_(0);
			SetIdExNeedLoad(0);
			SetIdExNeedStore(0);
			SetIdExRegValue(0);
			SetIdExNeedWriteBack(0);
			SetIdExIndex(0);
			cout << "[Cpu-Run] Clear registers done." << endl;
			IOHelper::WriteLog("[Cpu-Run] Clear registers done.");

			//category instruction loaded in IR
			cout << "[Cpu-Run] Now category instruction loaded in IR..." << endl;
			IOHelper::WriteLog("[Cpu-Run] Now category instruction loaded in IR...");
			switch (GetDecoder().GetOp(GetIr()))
			{
			case 0x00:
				cout << "[Cpu-Run] Op is 0x00, Type R instruction ensured." << endl;
				IOHelper::WriteLog("[Cpu-Run] Op is 0x00, Type R instruction ensured.");

				//type R
				SetIdExTypeR(1);

				//set op
				SetIdExOp(0);

				//set func
				switch (GetDecoder().GetFunc(GetIr()))
				{
				case 0x21:
					//addu
					cout << "[Cpu-Run] Func is 0x21, addu ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Func is 0x21, addu ensured.");
					SetIdExFunc(0x21);
					break;
				case 0x23:
					//subu
					cout << "[Cpu-Run] Func is 0x23, subu ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Func is 0x23, subu ensured.");
					SetIdExFunc(0x23);
					break;
				case 0x24:
					//and
					cout << "[Cpu-Run] Func is 0x24, and ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Func is 0x24, and ensured.");
					SetIdExFunc(0x24);
					break;
				case 0x25:
					//or
					cout << "[Cpu-Run] Func is 0x25, or ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Func is 0x25, or ensured.");
					SetIdExFunc(0x25);
					break;
				case 0x26:
					//xor
					cout << "[Cpu-Run] Func is 0x26, xor ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Func is 0x26, xor ensured.");
					SetIdExFunc(0x26);
					break;
				case 0x27:
					//nor
					cout << "[Cpu-Run] Func is 0x27, nor ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Func is 0x27, nor ensured.");
					SetIdExFunc(0x27);
					break;
				case 0x00:
					//sll
					cout << "[Cpu-Run] Func is 0x00, sll ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Func is 0x00, sll ensured.");
					SetIdExFunc(0x00);
					break;
				case 0x02:
					//srl
					cout << "[Cpu-Run] Func is 0x02, srl ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Func is 0x02, srl ensured.");
					SetIdExFunc(0x02);
					break;
				case 0x08:
					//jr
					cout << "[Cpu-Run] Func is 0x08, jr ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Func is 0x08, jr ensured.");
					SetIdExFunc(0x08);

					cout << "[Cpu-Run] Transition instruction jr detected!" << endl;
					IOHelper::WriteLog("[Cpu-Run] Transition instruction jr detected!");

					cout << "[Cpu-Run] Try get [rs] from fw." << endl;
					IOHelper::WriteLog("[Cpu-Run] Try get [rs] from fw.");
					//can rs get from fw
					if (GetFw0Index() == GetDecoder().GetRs(GetIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw0." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw0.");
						SetIdExRs(GetFw0Value());
					}
					else if (GetFw1Index() == GetDecoder().GetRs(GetIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw1." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw1.");
						SetIdExRs(GetFw1Value());
					}
					else if (GetFw2Index() == GetDecoder().GetRs(GetIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw2." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw2.");
						SetIdExRs(GetFw2Value());
					}
					else
					{
						cout << "[Cpu-Run] Fail to get [rs] from fw." << endl;
						IOHelper::WriteLog("[Cpu-Run] Fail to get [rs] from fw.");

						//is reg locked
						if (IsRegLocked(GetDecoder().GetRs(GetIr())))
						{
							cout << "[Cpu-Run] Register r" << GetDecoder().GetRs(GetIr()) << " locked! Now delay..." << endl;
							IOHelper::WriteLog("[Cpu-Run] Register r" + to_string(GetDecoder().GetRs(GetIr())) + " locked! Now delay...");

							//delay
							throw 1 + 5;
						}
						else
						{
							cout << "[Cpu-Run] Get [rs] from gprs." << endl;
							IOHelper::WriteLog("[Cpu-Run] Get [rs] from gprs.");

							//set rs
							SetIdExRs(GetGeneralPurposeRegisterSet().Get(GetDecoder().GetRs(GetIr())));
						}
					}

					//change pc
					SetPc(GetIdExRs());

					//reset
					cout << "[Cpu-Run] Reset left process in this clock." << endl;
					IOHelper::WriteLog("[Cpu-Run] Reset left process in this clock.");
					throw 1;

					break;
				default:
					//not support 
					cout << "[Cpu-Run] Error! Invalid instruction " << ConvertHelper::InstructionToString(GetIr()) << " detected! Now exit..." << endl;
					IOHelper::WriteLog("[Cpu-Run] Error! Invalid instruction " + ConvertHelper::InstructionToString(GetIr()) + " detected! Now exit...");
					exit(0);
					break;
				}

				//set shamt
				SetIdExShamt(GetDecoder().GetShamt(GetIr()));

				cout << "[Cpu-Run] Final write back register index is in rd here." << endl;
				IOHelper::WriteLog("[Cpu-Run] Final write back register index is in rd here.");

				//set rd
				SetIdExRd(GetDecoder().GetRd(GetIr()));

				//set index
				SetIdExIndex(GetDecoder().GetRd(GetIr()));

				cout << "[Cpu-Run] Try get [rt] from fw." << endl;
				IOHelper::WriteLog("[Cpu-Run] Try get [rt] from fw.");
				//can rt get from fw
				if (GetFw0Index() == GetDecoder().GetRt(GetIr()))
				{
					cout << "[Cpu-Run] Find [rt] from fw0." << endl;
					IOHelper::WriteLog("[Cpu-Run] Find [rt] from fw0.");
					SetIdExRt(GetFw0Value());
				}
				else if (GetFw1Index() == GetDecoder().GetRt(GetIr()))
				{
					cout << "[Cpu-Run] Find [rt] from fw1." << endl;
					IOHelper::WriteLog("[Cpu-Run] Find [rt] from fw1.");
					SetIdExRt(GetFw1Value());
				}
				else if (GetFw2Index() == GetDecoder().GetRt(GetIr()))
				{
					cout << "[Cpu-Run] Find [rt] from fw2." << endl;
					IOHelper::WriteLog("[Cpu-Run] Find [rt] from fw2.");
					SetIdExRt(GetFw2Value());
				}
				else
				{
					cout << "[Cpu-Run] Fail to get [rt] from fw." << endl;
					IOHelper::WriteLog("[Cpu-Run] Fail to get [rt] from fw.");

					//is reg locked
					if (IsRegLocked(GetDecoder().GetRt(GetIr())))
					{
						cout << "[Cpu-Run] Register r" << GetDecoder().GetRt(GetIr()) << " locked! Now delay..." << endl;
						IOHelper::WriteLog("[Cpu-Run] Register r" + to_string(GetDecoder().GetRt(GetIr())) + " locked! Now delay...");

						//delay
						throw 1 + 5;
					}
					else
					{
						cout << "[Cpu-Run] Get [rt] from gprs." << endl;
						IOHelper::WriteLog("[Cpu-Run] Get [rt] from gprs.");

						//set rt
						SetIdExRt(GetGeneralPurposeRegisterSet().Get(GetDecoder().GetRt(GetIr())));
					}
				}

				cout << "[Cpu-Run] Try get [rs] from fw." << endl;
				IOHelper::WriteLog("[Cpu-Run] Try get [rs] from fw.");
				//can rs get from fw
				if (GetFw0Index() == GetDecoder().GetRs(GetIr()))
				{
					cout << "[Cpu-Run] Find [rs] from fw0." << endl;
					IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw0.");
					SetIdExRs(GetFw0Value());
				}
				else if (GetFw1Index() == GetDecoder().GetRs(GetIr()))
				{
					cout << "[Cpu-Run] Find [rs] from fw1." << endl;
					IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw1.");
					SetIdExRs(GetFw1Value());
				}
				else if (GetFw2Index() == GetDecoder().GetRs(GetIr()))
				{
					cout << "[Cpu-Run] Find [rs] from fw2." << endl;
					IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw2.");
					SetIdExRs(GetFw2Value());
				}
				else
				{
					cout << "[Cpu-Run] Fail to get [rs] from fw." << endl;
					IOHelper::WriteLog("[Cpu-Run] Fail to get [rs] from fw.");

					//is reg locked
					if (IsRegLocked(GetDecoder().GetRs(GetIr())))
					{
						cout << "[Cpu-Run] Register r" << GetDecoder().GetRs(GetIr()) << " locked! Now delay..." << endl;
						IOHelper::WriteLog("[Cpu-Run] Register r" + to_string(GetDecoder().GetRs(GetIr())) + " locked! Now delay...");

						//delay
						throw 1 + 5;
					}
					else
					{
						cout << "[Cpu-Run] Get [rs] from gprs." << endl;
						IOHelper::WriteLog("[Cpu-Run] Get [rs] from gprs.");

						//set rs
						SetIdExRs(GetGeneralPurposeRegisterSet().Get(GetDecoder().GetRs(GetIr())));
					}
				}

				//lock rd
				cout << "[Cpu-Run] Lock register." << endl;
				IOHelper::WriteLog("[Cpu-Run] Lock register.");
				LockReg(GetDecoder().GetRd(GetIr()));

				//set need write back
				cout << "[Cpu-Run] Set need write back." << endl;
				IOHelper::WriteLog("[Cpu-Run] Set need write back.");
				SetIdExNeedWriteBack(1);

				break;
			case 0x02:
				cout << "[Cpu-Run] Op is 0x02, Type R instruction ensured." << endl;
				IOHelper::WriteLog("[Cpu-Run] Op is 0x02, Type R instruction ensured.");

				//type J
				SetIdExTypeJ(1);

				//j

				cout << "[Cpu-Run] Transition instruction j detected!" << endl;
				IOHelper::WriteLog("[Cpu-Run] Transition instruction j detected!");

				//change pc
				SetPc(GetDecoder().GetAddress(GetIr()));

				//reset
				cout << "[Cpu-Run] Reset left process in this clock." << endl;
				IOHelper::WriteLog("[Cpu-Run] Reset left process in this clock.");
				throw 1;

				break;
			default:
				cout << "[Cpu-Run] Op is not 0x00 and 0x02, Type I instruction ensured." << endl;
				IOHelper::WriteLog("[Cpu-Run] Op is not 0x00 and 0x02, Type I instruction ensured.");

				//case I
				SetIdExTypeI(1);

				//set op
				switch (GetDecoder().GetOp(GetIr()))
				{
				case 0x0C:
					//andi
					cout << "[Cpu-Run] Op is 0x0C, andi ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Op is 0x0C, andi ensured.");
					SetIdExOp(0x0C);
					break;
				case 0x0D:
					//ori
					cout << "[Cpu-Run] Op is 0x0D, ori ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Op is 0x0D, ori ensured.");
					SetIdExOp(0x0D);
					break;
				case 0x0E:
					//xori
					cout << "[Cpu-Run] Op is 0x0E, xori ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Op is 0x0E, xori ensured.");
					SetIdExOp(0x0E);
					break;
				case 0x04:
					//beq
					cout << "[Cpu-Run] Op is 0x04, beq ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Op is 0x04, beq ensured.");
					SetIdExOp(0x04);
					break;
				case 0x05:
					//bne
					cout << "[Cpu-Run] Op is 0x05, bne ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Op is 0x05, bne ensured.");
					SetIdExOp(0x05);
					break;
				case 0x23:
					//lw
					cout << "[Cpu-Run] Op is 0x23, lw ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Op is 0x23, lw ensured.");
					SetIdExOp(0x23);
					cout << "[Cpu-Run] Set need load." << endl;
					IOHelper::WriteLog("[Cpu-Run] Set need load.");
					SetIdExNeedLoad(1);
					break;
				case 0x2B:
					//sw
					cout << "[Cpu-Run] Op is 0x2B, sw ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Op is 0x2B, sw ensured.");
					SetIdExOp(0x2B);
					cout << "[Cpu-Run] Set need store." << endl;
					IOHelper::WriteLog("[Cpu-Run] Set need store.");
					SetIdExNeedStore(1);
					break;
				case 0x3F:
					//end of instructions (customized)
					cout << "[Cpu-Run] End of instructions (customized) detected!" << endl;
					IOHelper::WriteLog("[Cpu-Run] End of instructions (customized) detected!");

					//change eoi flag to true
					eoiDetected = true;

					//reset
					cout << "[Cpu-Run] Reset left process in this clock." << endl;
					IOHelper::WriteLog("[Cpu-Run] Reset left process in this clock.");
					throw 1;

					break;
				default:
					//not support
					cout << "[Cpu-Run] Error! Invalid instruction " << ConvertHelper::InstructionToString(GetIr()) << " detected! Now exit..." << endl;
					IOHelper::WriteLog("[Cpu-Run] Error! Invalid instruction " + ConvertHelper::InstructionToString(GetIr()) + " detected! Now exit...");
					exit(0);
					break;
				}

				//set immediate
				SetIdExImmediate(GetDecoder().GetImmediate(GetIr()));

				//rs rt related
				switch (GetDecoder().GetOp(GetIr()))
				{
				case 0x0C:
				case 0x0D:
				case 0x0E:
					//andi ori xori

					cout << "[Cpu-Run] Final write back register index is in rt here." << endl;
					IOHelper::WriteLog("[Cpu-Run] Final write back register index is in rt here.");

					//set rt
					SetIdExRt(GetDecoder().GetRt(GetIr()));

					//set index
					SetIdExIndex(GetDecoder().GetRt(GetIr()));

					//set need write back
					cout << "[Cpu-Run] Set need write back." << endl;
					IOHelper::WriteLog("[Cpu-Run] Set need write back.");
					SetIdExNeedWriteBack(1);

					cout << "[Cpu-Run] Try get [rs] from fw." << endl;
					IOHelper::WriteLog("[Cpu-Run] Try get [rs] from fw.");
					//can rs get from fw
					if (GetFw0Index() == GetDecoder().GetRs(GetIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw0." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw0.");
						SetIdExRs(GetFw0Value());
					}
					else if (GetFw1Index() == GetDecoder().GetRs(GetIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw1." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw1.");
						SetIdExRs(GetFw1Value());
					}
					else if (GetFw2Index() == GetDecoder().GetRs(GetIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw2." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw2.");
						SetIdExRs(GetFw2Value());
					}
					else
					{
						cout << "[Cpu-Run] Fail to get [rs] from fw." << endl;
						IOHelper::WriteLog("[Cpu-Run] Fail to get [rs] from fw.");

						//is reg locked
						if (IsRegLocked(GetDecoder().GetRs(GetIr())))
						{
							cout << "[Cpu-Run] Register r" << GetDecoder().GetRs(GetIr()) << " locked! Now delay..." << endl;
							IOHelper::WriteLog("[Cpu-Run] Register r" + to_string(GetDecoder().GetRs(GetIr())) + " locked! Now delay...");

							//delay
							throw 1 + 5;
						}
						else
						{
							cout << "[Cpu-Run] Get [rs] from gprs." << endl;
							IOHelper::WriteLog("[Cpu-Run] Get [rs] from gprs.");

							//set rs
							SetIdExRs(GetGeneralPurposeRegisterSet().Get(GetDecoder().GetRs(GetIr())));
						}
					}

					//lock rt
					cout << "[Cpu-Run] Lock register." << endl;
					IOHelper::WriteLog("[Cpu-Run] Lock register.");
					LockReg(GetDecoder().GetRt(GetIr()));

					break;
				case 0x04:
				case 0x05:
					//beq bne

					cout << "[Cpu-Run] Try get [rt] from fw." << endl;
					IOHelper::WriteLog("[Cpu-Run] Try get [rt] from fw.");
					//can rt get from fw
					if (GetFw0Index() == GetDecoder().GetRt(GetIr()))
					{
						cout << "[Cpu-Run] Find [rt] from fw0." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rt] from fw0.");
						SetIdExRt(GetFw0Value());
					}
					else if (GetFw1Index() == GetDecoder().GetRt(GetIr()))
					{
						cout << "[Cpu-Run] Find [rt] from fw1." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rt] from fw1.");
						SetIdExRt(GetFw1Value());
					}
					else if (GetFw2Index() == GetDecoder().GetRt(GetIr()))
					{
						cout << "[Cpu-Run] Find [rt] from fw2." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rt] from fw2.");
						SetIdExRt(GetFw2Value());
					}
					else
					{
						cout << "[Cpu-Run] Fail to get [rt] from fw." << endl;
						IOHelper::WriteLog("[Cpu-Run] Fail to get [rt] from fw.");

						//is reg locked
						if (IsRegLocked(GetDecoder().GetRt(GetIr())))
						{
							cout << "[Cpu-Run] Register r" << GetDecoder().GetRt(GetIr()) << " locked! Now delay..." << endl;
							IOHelper::WriteLog("[Cpu-Run] Register r" + to_string(GetDecoder().GetRt(GetIr())) + " locked! Now delay...");

							//delay
							throw 1 + 5;
						}
						else
						{
							cout << "[Cpu-Run] Get [rt] from gprs." << endl;
							IOHelper::WriteLog("[Cpu-Run] Get [rt] from gprs.");

							//set rt
							SetIdExRt(GetGeneralPurposeRegisterSet().Get(GetDecoder().GetRt(GetIr())));
						}
					}

					cout << "[Cpu-Run] Try get [rs] from fw." << endl;
					IOHelper::WriteLog("[Cpu-Run] Try get [rs] from fw.");
					//can rs get from fw
					if (GetFw0Index() == GetDecoder().GetRs(GetIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw0." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw0.");
						SetIdExRs(GetFw0Value());
					}
					else if (GetFw1Index() == GetDecoder().GetRs(GetIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw1." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw1.");
						SetIdExRs(GetFw1Value());
					}
					else if (GetFw2Index() == GetDecoder().GetRs(GetIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw2." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw2.");
						SetIdExRs(GetFw2Value());
					}
					else
					{
						cout << "[Cpu-Run] Fail to get [rs] from fw." << endl;
						IOHelper::WriteLog("[Cpu-Run] Fail to get [rs] from fw.");

						//is reg locked
						if (IsRegLocked(GetDecoder().GetRs(GetIr())))
						{
							cout << "[Cpu-Run] Register r" << GetDecoder().GetRs(GetIr()) << " locked! Now delay..." << endl;
							IOHelper::WriteLog("[Cpu-Run] Register r" + to_string(GetDecoder().GetRs(GetIr())) + " locked! Now delay...");

							//delay
							throw 1 + 5;
						}
						else
						{
							cout << "[Cpu-Run] Get [rs] from gprs." << endl;
							IOHelper::WriteLog("[Cpu-Run] Get [rs] from gprs.");

							//set rs
							SetIdExRs(GetGeneralPurposeRegisterSet().Get(GetDecoder().GetRs(GetIr())));
						}
					}

					break;
				case 0x23:
					//lw

					cout << "[Cpu-Run] Final write back register index is in rt here." << endl;
					IOHelper::WriteLog("[Cpu-Run] Final write back register index is in rt here.");

					//set rt
					SetIdExRt(GetDecoder().GetRt(GetIr()));

					//set index
					SetIdExIndex(GetDecoder().GetRt(GetIr()));

					//set need write back
					cout << "[Cpu-Run] Set need write back." << endl;
					IOHelper::WriteLog("[Cpu-Run] Set need write back.");
					SetIdExNeedWriteBack(1);

					cout << "[Cpu-Run] Try get [rs] from fw." << endl;
					IOHelper::WriteLog("[Cpu-Run] Try get [rs] from fw.");
					//can rs get from fw
					if (GetFw0Index() == GetDecoder().GetRs(GetIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw0." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw0.");
						SetIdExRs(GetFw0Value());
					}
					else if (GetFw1Index() == GetDecoder().GetRs(GetIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw1." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw1.");
						SetIdExRs(GetFw1Value());
					}
					else if (GetFw2Index() == GetDecoder().GetRs(GetIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw2." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw2.");
						SetIdExRs(GetFw2Value());
					}
					else
					{
						cout << "[Cpu-Run] Fail to get [rs] from fw." << endl;
						IOHelper::WriteLog("[Cpu-Run] Fail to get [rs] from fw.");

						//is reg locked
						if (IsRegLocked(GetDecoder().GetRs(GetIr())))
						{
							cout << "[Cpu-Run] Register r" << GetDecoder().GetRs(GetIr()) << " locked! Now delay..." << endl;
							IOHelper::WriteLog("[Cpu-Run] Register r" + to_string(GetDecoder().GetRs(GetIr())) + " locked! Now delay...");

							//delay
							throw 1 + 5;
						}
						else
						{
							cout << "[Cpu-Run] Get [rs] from gprs." << endl;
							IOHelper::WriteLog("[Cpu-Run] Get [rs] from gprs.");

							//set rs
							SetIdExRs(GetGeneralPurposeRegisterSet().Get(GetDecoder().GetRs(GetIr())));
						}
					}

					//lock rt
					cout << "[Cpu-Run] Lock register." << endl;
					IOHelper::WriteLog("[Cpu-Run] Lock register.");
					LockReg(GetDecoder().GetRt(GetIr()));

					break;
				case 0x2B:
					//sw

					cout << "[Cpu-Run] Try get [rt] from fw." << endl;
					IOHelper::WriteLog("[Cpu-Run] Try get [rt] from fw.");
					//can rt get from fw
					if (GetFw0Index() == GetDecoder().GetRt(GetIr()))
					{
						cout << "[Cpu-Run] Find [rt] from fw0." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rt] from fw0.");
						SetIdExRt(GetFw0Value());
					}
					else if (GetFw1Index() == GetDecoder().GetRt(GetIr()))
					{
						cout << "[Cpu-Run] Find [rt] from fw1." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rt] from fw1.");
						SetIdExRt(GetFw1Value());
					}
					else if (GetFw2Index() == GetDecoder().GetRt(GetIr()))
					{
						cout << "[Cpu-Run] Find [rt] from fw2." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rt] from fw2.");
						SetIdExRt(GetFw2Value());
					}
					else
					{
						cout << "[Cpu-Run] Fail to get [rt] from fw." << endl;
						IOHelper::WriteLog("[Cpu-Run] Fail to get [rt] from fw.");

						//is reg locked
						if (IsRegLocked(GetDecoder().GetRt(GetIr())))
						{
							cout << "[Cpu-Run] Register r" << GetDecoder().GetRt(GetIr()) << " locked! Now delay..." << endl;
							IOHelper::WriteLog("[Cpu-Run] Register r" + to_string(GetDecoder().GetRt(GetIr())) + " locked! Now delay...");

							//delay
							throw 1 + 5;
						}
						else
						{
							cout << "[Cpu-Run] Get [rt] from gprs." << endl;
							IOHelper::WriteLog("[Cpu-Run] Get [rt] from gprs.");

							//set rt
							SetIdExRt(GetGeneralPurposeRegisterSet().Get(GetDecoder().GetRt(GetIr())));
						}
					}

					cout << "[Cpu-Run] Set value which to store to memory." << endl;
					IOHelper::WriteLog("[Cpu-Run] Set value which to store to memory.");

					//set reg value
					SetIdExRegValue(GetIdExRt());

					cout << "[Cpu-Run] Try get [rs] from fw." << endl;
					IOHelper::WriteLog("[Cpu-Run] Try get [rs] from fw.");
					//can rs get from fw
					if (GetFw0Index() == GetDecoder().GetRs(GetIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw0." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw0.");
						SetIdExRs(GetFw0Value());
					}
					else if (GetFw1Index() == GetDecoder().GetRs(GetIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw1." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw1.");
						SetIdExRs(GetFw1Value());
					}
					else if (GetFw2Index() == GetDecoder().GetRs(GetIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw2." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw2.");
						SetIdExRs(GetFw2Value());
					}
					else
					{
						cout << "[Cpu-Run] Fail to get [rs] from fw." << endl;
						IOHelper::WriteLog("[Cpu-Run] Fail to get [rs] from fw.");

						//is reg locked
						if (IsRegLocked(GetDecoder().GetRs(GetIr())))
						{
							cout << "[Cpu-Run] Register r" << GetDecoder().GetRs(GetIr()) << " locked! Now delay..." << endl;
							IOHelper::WriteLog("[Cpu-Run] Register r" + to_string(GetDecoder().GetRs(GetIr())) + " locked! Now delay...");

							//delay
							throw 1 + 5;
						}
						else
						{
							cout << "[Cpu-Run] Get [rs] from gprs." << endl;
							IOHelper::WriteLog("[Cpu-Run] Get [rs] from gprs.");

							//set rs
							SetIdExRs(GetGeneralPurposeRegisterSet().Get(GetDecoder().GetRs(GetIr())));
						}
					}
					break;
				default:
					break;
				}
				break;
			}

			//trans regs
			cout << "[Cpu-Run] Now transfer data..." << endl;
			IOHelper::WriteLog("[Cpu-Run] Now transfer data...");
			SetIdExPc(GetIfIdPc());
			cout << "[Cpu-Run] Data transferred done." << endl;
			IOHelper::WriteLog("[Cpu-Run] Data transferred done.");
		}
		cout << "[Cpu-Run] -------------------- " << clockNumber << " - ID end --------------------" << endl;
		IOHelper::WriteLog("[Cpu-Run] -------------------- " + to_string(clockNumber) + " - ID end --------------------");

		//IF
		cout << "[Cpu-Run] -------------------- " << clockNumber << " - IF begin --------------------" << endl;
		IOHelper::WriteLog("[Cpu-Run] -------------------- " + to_string(clockNumber) + " - IF begin --------------------");
		if (!IsReady(0))
		{
			cout << "[Cpu-Run] No work now." << endl;
			IOHelper::WriteLog("[Cpu-Run] No work now.");
			SetRunInterrupted(0);
		}
		else
		{
			if (GetMemIfAllow() == 1)
			{
				cout << "[Cpu-Run] Find that load/store done in this clock. Now delay..." << endl;
				IOHelper::WriteLog("[Cpu-Run] Find that load/store done in this clock. Now delay...");

				//allow IF
				cout << "[Cpu-Run] Allow IF in next clock if possible." << endl;
				IOHelper::WriteLog("[Cpu-Run] Allow IF in next clock if possible.");
				SetMemIfAllow(0);

				//delay
				throw 0 + 5;
			}
			else
			{
				//fetch instruction from memory to IR
				cout << "[Cpu-Run] Fetch instruction from memory to IR." << endl;
				IOHelper::WriteLog("[Cpu-Run] Fetch instruction from memory to IR.");
				SetIr(memory.ReadWord(GetPc()));

				//set instruction pc
				SetIfIdPc(GetPc());

				//modify pc
				cout << "[Cpu-Run] PC + 1" << endl;
				IOHelper::WriteLog("[Cpu-Run] PC + 1");
				SetPc(GetPc() + 4);
			}
		}
		cout << "[Cpu-Run] -------------------- " << clockNumber << " - IF end --------------------" << endl;
		IOHelper::WriteLog("[Cpu-Run] -------------------- " + to_string(clockNumber) + " - IF end --------------------");

	}
	catch (int index)
	{
		cout << "[Cpu-Run] Now set run status..." << endl;
		IOHelper::WriteLog("[Cpu-Run] Now set run status...");

		//reset process
		switch (index % 5)
		{
		case 4:
			SetRunInterrupted(4);
		case 3:
			SetRunInterrupted(3);
		case 2:
			SetRunInterrupted(2);
		case 1:
			SetRunInterrupted(1);
		case 0:
			SetRunInterrupted(0);
			break;
		default:
			break;
		}

		//delay process
		switch (index)
		{
		case 4 + 5:
			SetRunDone(3);
		case 3 + 5:
			SetRunDone(2);
		case 2 + 5:
			SetRunDone(1);
		case 1 + 5:
			SetRunDone(0);
		case 0 + 5:
			break;
		default:
			break;
		}
	}

	//set next clock time ready
	cout << "[Cpu-Run] Now set next clock time ready..." << endl;
	IOHelper::WriteLog("[Cpu-Run] Now set next clock time ready...");
	SetNewReady();

	//end of instructions detected
	if (eoiDetected)
	{
		cout << "[Cpu-Run] Eoi detected, no more valid instructions to run." << endl;
		IOHelper::WriteLog("[Cpu-Run] Eoi detected, no more valid instructions to run.");
		SetNotReady(0);
	}
}

GeneralPurposeRegisterSet & Cpu::GetGeneralPurposeRegisterSet()
{
	return this->gprs;
}

void Cpu::SetPc(address add)
{
	cout << "[Cpu] Set register PC with value " << ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) << "(" << add << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register PC with value " + ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) + "(" + to_string(add) + ").");
	pc.Set(add);
}

address Cpu::GetPc()
{
	return pc.Get();
}

void Cpu::SetIr(instruction i)
{
	cout << "[Cpu] Set register IR with value " << ConvertHelper::SeperateString(ConvertHelper::InstructionToString(i)) << "(" << i << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IR with value " + ConvertHelper::SeperateString(ConvertHelper::InstructionToString(i)) + "(" + to_string(i) + ").");
	ir.Set(i);
}

instruction Cpu::GetIr()
{
	return ir.Get();
}

void Cpu::LockReg(int index)
{
	if (index >= 0 && index < 32)
	{
		//set 1
		cout << "[Cpu] Lock register r" << index << "." << endl;
		IOHelper::WriteLog("[Cpu] Lock register r" + to_string(index) + ".");
		lockMarker.Set(lockMarker.Get() | (1 << index));
	}
	else
	{
		cout << "[Cpu] Warning! Invalid index " << index << " detected! No register locked in this call." << endl;
		IOHelper::WriteLog("[Cpu] Warning! Invalid index " + to_string(index) + " detected! No register locked in this call.");
	}
}

void Cpu::UnlockReg(int index)
{
	if (index >= 0 && index < 32)
	{
		//set 0
		cout << "[Cpu] Unlock register r" << index << "." << endl;
		IOHelper::WriteLog("[Cpu] Unlock register r" + to_string(index) + ".");
		lockMarker.Set(lockMarker.Get() & (~(1 << index)));
	}
	else
	{
		cout << "[Cpu] Warning! Invalid index " << index << " detected! No register unlocked in this call." << endl;
		IOHelper::WriteLog("[Cpu] Warning! Invalid index " + to_string(index) + " detected! No register unlocked in this call.");
	}
}

bool Cpu::IsRegLocked(int index)
{
	if (index >= 0 && index < 32)
	{
		//1 locked 0 not locked
		return (lockMarker.Get() & (1 << index)) >> index;
	}
	else
	{
		cout << "[Cpu] Warning! Invalid index " << index << " detected! Return false by default." << endl;
		IOHelper::WriteLog("[Cpu] Warning! Invalid index " + to_string(index) + " detected! Return false by default.");
		return false;
	}
}

bool Cpu::IsFw0Vacant()
{
	return fw0_index.Get() == 999;
}

void Cpu::SetFw0Vacant()
{
	cout << "[Cpu] Set register Fw0 vacant." << endl;
	IOHelper::WriteLog("[Cpu] Set register Fw0 vacant.");
	fw0_index.Set(999);
}

void Cpu::SetFw0Value(word w)
{
	cout << "[Cpu] Set register Fw0Value with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register Fw0Value with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	fw0_value.Set(w);
}

word Cpu::GetFw0Value()
{
	return fw0_value.Get();
}

void Cpu::SetFw0Index(word w)
{
	cout << "[Cpu] Set register Fw0Index with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register Fw0Index with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	fw0_index.Set(w);
}

word Cpu::GetFw0Index()
{
	return fw0_index.Get();
}

bool Cpu::IsFw1Vacant()
{
	return fw1_index.Get() == 999;
}

void Cpu::SetFw1Vacant()
{
	cout << "[Cpu] Set register Fw1 vacant." << endl;
	IOHelper::WriteLog("[Cpu] Set register Fw1 vacant.");
	fw1_index.Set(999);
}

void Cpu::SetFw1Value(word w)
{
	cout << "[Cpu] Set register Fw1Value with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register Fw1Value with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	fw1_value.Set(w);
}

word Cpu::GetFw1Value()
{
	return fw1_value.Get();
}

void Cpu::SetFw1Index(word w)
{
	cout << "[Cpu] Set register Fw1Index with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register Fw1Index with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	fw1_index.Set(w);
}

word Cpu::GetFw1Index()
{
	return fw1_index.Get();
}

bool Cpu::IsFw2Vacant()
{
	return fw2_index.Get() == 999;
}

void Cpu::SetFw2Vacant()
{
	cout << "[Cpu] Set register Fw2 vacant." << endl;
	IOHelper::WriteLog("[Cpu] Set register Fw2 vacant.");
	fw2_index.Set(999);
}

void Cpu::SetFw2Value(word w)
{
	cout << "[Cpu] Set register Fw2Value with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register Fw2Value with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	fw2_value.Set(w);
}

word Cpu::GetFw2Value()
{
	return fw2_value.Get();
}

void Cpu::SetFw2Index(word w)
{
	cout << "[Cpu] Set register Fw2Index with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register Fw2Index with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	fw2_index.Set(w);
}

word Cpu::GetFw2Index()
{
	return fw2_index.Get();
}

Decoder Cpu::GetDecoder()
{
	return decoder;
}

Alu Cpu::GetAlu()
{
	return alu;
}

void Cpu::SetIfIdPc(address add)
{
	cout << "[Cpu] Set register IfIdPc with value " << ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) << "(" << add << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IfIdPc with value " + ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) + "(" + to_string(add) + ").");
	if_id_pc.Set(add);
}

address Cpu::GetIfIdPc()
{
	return if_id_pc.Get();
}

void Cpu::SetIdExPc(address add)
{
	cout << "[Cpu] Set register IdExPc with value " << ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) << "(" << add << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExPc with value " + ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) + "(" + to_string(add) + ").");
	id_ex_pc.Set(add);
}

address Cpu::GetIdExPc()
{
	return id_ex_pc.Get();
}

void Cpu::SetIdExTypeR(word w)
{
	cout << "[Cpu] Set register IdExTypeR with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExTypeR with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_type_R.Set(w);
}

word Cpu::GetIdExTypeR()
{
	return id_ex_type_R.Get();
}

void Cpu::SetIdExTypeI(word w)
{
	cout << "[Cpu] Set register IdExTypeI with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExTypeI with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_type_I.Set(w);
}

word Cpu::GetIdExTypeI()
{
	return id_ex_type_I.Get();
}

void Cpu::SetIdExTypeJ(word w)
{
	cout << "[Cpu] Set register IdExTypeJ with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExTypeJ with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_type_J.Set(w);
}

word Cpu::GetIdExTypeJ()
{
	return id_ex_type_J.Get();
}

void Cpu::SetIdExOp(word w)
{
	cout << "[Cpu] Set register IdExOp with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExOp with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_op.Set(w);
}

word Cpu::GetIdExOp()
{
	return id_ex_op.Get();
}

void Cpu::SetIdExRs(word w)
{
	cout << "[Cpu] Set register IdExRs with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExRs with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_rs.Set(w);
}

word Cpu::GetIdExRs()
{
	return id_ex_rs.Get();
}

void Cpu::SetIdExRt(word w)
{
	cout << "[Cpu] Set register IdExRt with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExRt with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_rt.Set(w);
}

word Cpu::GetIdExRt()
{
	return id_ex_rt.Get();
}

void Cpu::SetIdExRd(word w)
{
	cout << "[Cpu] Set register IdExRd with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExRd with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_rd.Set(w);
}

word Cpu::GetIdExRd()
{
	return id_ex_rd.Get();
}

void Cpu::SetIdExShamt(word w)
{
	cout << "[Cpu] Set register IdExShamt with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExShamt with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_shamt.Set(w);
}

word Cpu::GetIdExShamt()
{
	return id_ex_shamt.Get();
}

void Cpu::SetIdExFunc(word w)
{
	cout << "[Cpu] Set register IdExFunc with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExFunc with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_func.Set(w);
}

word Cpu::GetIdExFunc()
{
	return id_ex_func.Get();
}

void Cpu::SetIdExImmediate(word w)
{
	cout << "[Cpu] Set register IdExImmediate with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExImmediate with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_immediate.Set(w);
}

word Cpu::GetIdExImmediate()
{
	return id_ex_immediate.Get();
}

void Cpu::SetIdExAddress_(address add)
{
	cout << "[Cpu] Set register IdExAddress_ with value " << ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) << "(" << add << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExAddress_ with value " + ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) + "(" + to_string(add) + ").");
	id_ex_address_.Set(add);
}

address Cpu::GetIdExAddress_()
{
	return id_ex_address_.Get();
}

bool Cpu::IsReady(int index)
{
	if (index >= 0 && index < 5)
	{
		return (isReady.Get() & (1 << index)) >> index;
	}
	else
	{
		cout << "[Cpu] Warning! Invalid index " << index << " detected! Return true by default." << endl;
		IOHelper::WriteLog("[Cpu] Warning! Invalid index " + to_string(index) + " detected! Return true by default.");
		return true;
	}
}

void Cpu::SetReady(int index)
{
	if (index >= 0 && index < 5)
	{
		//set 1
		cout << "[Cpu] Set process " << index << " ready." << endl;
		IOHelper::WriteLog("[Cpu] Set process " + to_string(index) + " ready.");
		isReady.Set(isReady.Get() | (1 << index));
	}
	else
	{
		cout << "[Cpu] Warning! Invalid index " << index << " detected! No process set ready in this call." << endl;
		IOHelper::WriteLog("[Cpu] Warning! Invalid index " + to_string(index) + " detected! No process set ready in this call.");
	}
}

void Cpu::SetNotReady(int index)
{
	if (index >= 0 && index < 5)
	{
		//set 0
		cout << "[Cpu] Set process " << index << " not ready." << endl;
		IOHelper::WriteLog("[Cpu] Set process " + to_string(index) + " not ready.");
		isReady.Set(isReady.Get() & (~(1 << index)));
	}
	else
	{
		cout << "[Cpu] Warning! Invalid index " << index << " detected! No process set not ready in this call." << endl;
		IOHelper::WriteLog("[Cpu] Warning! Invalid index " + to_string(index) + " detected! No process set not ready in this call.");
	}
}

void Cpu::SetNewReady()
{
	for (int i = 4; i > 0; i--)
	{
		if (IsRunDone(i - 1))
		{
			SetReady(i);
		}
		else
		{
			SetNotReady(i);
		}
	}
	SetReady(0);
}

bool Cpu::HasWork()
{
	bool result = false;
	for (int i = 0; i < 5; i++)
	{
		if (IsReady(i))
		{
			result = true;
			break;
		}
	}
	return result;
}

bool Cpu::IsRunDone(int index)
{
	if (index >= 0 && index < 5)
	{
		return (runStatus.Get() & (1 << index)) >> index;
	}
	else
	{
		cout << "[Cpu] Warning! Invalid index " << index << " detected! Return true by default." << endl;
		IOHelper::WriteLog("[Cpu] Warning! Invalid index " + to_string(index) + " detected! Return true by default.");
		return true;
	}
}

void Cpu::SetRunDone(int index)
{
	if (index >= 0 && index < 5)
	{
		//set 1
		cout << "[Cpu] Set process " << index << " done." << endl;
		IOHelper::WriteLog("[Cpu] Set process " + to_string(index) + " done.");
		runStatus.Set(runStatus.Get() | (1 << index));
	}
	else
	{
		cout << "[Cpu] Warning! Invalid index " << index << " detected! No process set done in this call." << endl;
		IOHelper::WriteLog("[Cpu] Warning! Invalid index " + to_string(index) + " detected! No process set done in this call.");
	}
}

void Cpu::SetRunInterrupted(int index)
{
	if (index >= 0 && index < 5)
	{
		//set 0
		cout << "[Cpu] Set process " << index << " interrupted." << endl;
		IOHelper::WriteLog("[Cpu] Set process " + to_string(index) + " interrupted.");
		runStatus.Set(runStatus.Get() & (~(1 << index)));
	}
	else
	{
		cout << "[Cpu] Warning! Invalid index " << index << " detected! No process set interrupted in this call." << endl;
		IOHelper::WriteLog("[Cpu] Warning! Invalid index " + to_string(index) + " detected! No process set interrupted in this call.");
	}
}

void Cpu::ResetRunStatus()
{
	cout << "[Cpu] Reset all processes done." << endl;
	IOHelper::WriteLog("[Cpu] Reset all processes done.");
	for (int i = 4; i >= 0; i--)
	{
		SetRunDone(i);
	}
}

void Cpu::SetIdExNeedLoad(word w)
{
	cout << "[Cpu] Set register IdExNeedLoad with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExNeedLoad with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_need_load.Set(w);
}

word Cpu::GetIdExNeedLoad()
{
	return id_ex_need_load.Get();
}

void Cpu::SetIdExNeedStore(word w)
{
	cout << "[Cpu] Set register IdExNeedStore with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExNeedStore with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_need_store.Set(w);
}

word Cpu::GetIdExNeedStore()
{
	return id_ex_need_store.Get();
}

void Cpu::SetIdExRegValue(word w)
{
	cout << "[Cpu] Set register IdExRegValue with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExRegValue with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_reg_value.Set(w);
}

word Cpu::GetIdExRegValue()
{
	return id_ex_reg_value.Get();
}

void Cpu::SetIdExNeedWriteBack(word w)
{
	cout << "[Cpu] Set register IdExNeedWriteBack with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExNeedWriteBack with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_need_write_back.Set(w);
}

word Cpu::GetIdExNeedWriteBack()
{
	return id_ex_need_write_back.Get();
}

void Cpu::SetIdExIndex(word w)
{
	cout << "[Cpu] Set register IdExIndex with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExIndex with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_index.Set(w);
}

word Cpu::GetIdExIndex()
{
	return id_ex_index.Get();
}

void Cpu::SetExMemNeedLoad(word w)
{
	cout << "[Cpu] Set register ExMemNeedLoad with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register ExMemNeedLoad with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	ex_mem_need_load.Set(w);
}

word Cpu::GetExMemNeedLoad()
{
	return ex_mem_need_load.Get();
}

void Cpu::SetExMemNeedStore(word w)
{
	cout << "[Cpu] Set register ExMemNeedStore with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register ExMemNeedStore with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	ex_mem_need_store.Set(w);
}

word Cpu::GetExMemNeedStore()
{
	return ex_mem_need_store.Get();
}

void Cpu::SetExMemAddress(address add)
{
	cout << "[Cpu] Set register ExMemNeedAddress with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(add)) << "(" << add << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register ExMemNeedAddress with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(add)) + "(" + to_string(add) + ").");
	ex_mem_address.Set(add);
}

address Cpu::GetExMemAddress()
{
	return ex_mem_address.Get();
}

void Cpu::SetExMemRegValue(word w)
{
	cout << "[Cpu] Set register ExMemRegValue with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register ExMemRegValue with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	ex_mem_reg_value.Set(w);
}

word Cpu::GetExMemRegValue()
{
	return ex_mem_reg_value.Get();
}

void Cpu::SetExMemNeedWriteBack(word w)
{
	cout << "[Cpu] Set register ExMemNeedWriteBack with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register ExMemNeedWriteBack with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	ex_mem_need_write_back.Set(w);
}

word Cpu::GetExMemNeedWriteBack()
{
	return ex_mem_need_write_back.Get();
}

void Cpu::SetExMemIndex(word w)
{
	cout << "[Cpu] Set register ExMemNeedIndex with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register ExMemNeedIndex with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	ex_mem_index.Set(w);
}

word Cpu::GetExMemIndex()
{
	return ex_mem_index.Get();
}

void Cpu::SetExMemWord(word w)
{
	cout << "[Cpu] Set register ExMemNeedWord with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register ExMemNeedWord with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	ex_mem_word.Set(w);
}

word Cpu::GetExMemWord()
{
	return ex_mem_word.Get();
}

void Cpu::SetMemIfAllow(word w)
{
	cout << "[Cpu] Set register MemIfAllow with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register MemIfAllow with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	mem_if_allow.Set(w);
}

word Cpu::GetMemIfAllow()
{
	return mem_if_allow.Get();
}

void Cpu::SetMemWbNeedWriteBack(word w)
{
	cout << "[Cpu] Set register MemWbNeedWriteBack with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register MemWbNeedWriteBack with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	mem_wb_need_write_back.Set(w);
}

word Cpu::GetMemWbNeedWriteBack()
{
	return mem_wb_need_write_back.Get();
}

void Cpu::SetMemWbIndex(word w)
{
	cout << "[Cpu] Set register MemWbIndex with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register MemWbIndex with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	mem_wb_index.Set(w);
}

word Cpu::GetMemWbIndex()
{
	return mem_wb_index.Get();
}

void Cpu::SetMemWbWord(word w)
{
	cout << "[Cpu] Set register MemWbWord with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register MemWbWord with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	mem_wb_word.Set(w);
}

word Cpu::GetMemWbWord()
{
	return mem_wb_word.Get();
}
