#include "NaiveMIPS32InstructionEmulator.h"

Cpu::Cpu()
{
	cout << "[Cpu] Initialize Cpu..." << endl;
	IOHelper::WriteLog("[Cpu] Initialize Cpu...");

	//initialize ready status
	for (int i = 4; i > 0; i--)
	{
		setNotReady(i);
	}
	setReady(0);

	//initialize fw
	setFw0Vacant();
	setFw1Vacant();
	setFw2Vacant();

	cout << "[Cpu] Cpu initialized." << endl;
	IOHelper::WriteLog("[Cpu] Cpu initialized.");
}

void Cpu::Run(Memory & memory, bool &eoiDetected, int clockNumber)
{

	resetRunStatus();

	try
	{
		//WB
		cout << "[Cpu-Run] -------------------- " << clockNumber << " - WB begin --------------------" << endl;
		IOHelper::WriteLog("[Cpu-Run] -------------------- " + to_string(clockNumber) + " - WB begin --------------------");
		if (!isReady(4))
		{
			cout << "[Cpu-Run] No work now." << endl;
			IOHelper::WriteLog("[Cpu-Run] No work now.");
			setRunInterrupted(4);
		}
		else
		{
			if (getMemWbNeedWriteBack() == 0)
			{
				cout << "[Cpu-Run] Not need to write back now." << endl;
				IOHelper::WriteLog("[Cpu-Run] Not need to write back now.");
			}
			else
			{
				//write back
				cout << "[Cpu-Run] Write word " << ConvertHelper::SeperateString(ConvertHelper::WordToString(getMemWbWord())) << "(" << getMemWbWord() << ") back to r" << getMemWbIndex() << "." << endl;
				IOHelper::WriteLog("[Cpu-Run] Write word " + ConvertHelper::SeperateString(ConvertHelper::WordToString(getMemWbWord())) + "(" + to_string(getMemWbWord()) + ") back to r" + to_string(getMemWbIndex()) + ".");
				getGeneralPurposeRegisterSet().Set(getMemWbIndex(), getMemWbWord());

				//unlock reg
				unlockReg(getMemWbIndex());

				//free fw
				if (getFw0Index() == getMemWbIndex())
				{
					setFw0Vacant();
				}
				else if (getFw1Index() == getMemWbIndex())
				{
					setFw1Vacant();
				}
				else if (getFw2Index() == getMemWbIndex())
				{
					setFw2Vacant();
				}
				else
				{
					//no index from found, something occurred
					cout << "[Cpu-Run] Warning! Not found fw which index is " << getMemWbIndex() << " to set it vacant." << endl;
					IOHelper::WriteLog("[Cpu-Run] Warning! Not found fw which index is " + to_string(getMemWbIndex()) + " to set it vacant.");
				}
			}
		}
		cout << "[Cpu-Run] -------------------- " << clockNumber << " - WB end --------------------" << endl;
		IOHelper::WriteLog("[Cpu-Run] -------------------- " + to_string(clockNumber) + " - WB end --------------------");

		//MEM
		cout << "[Cpu-Run] -------------------- " << clockNumber << " - MEM begin --------------------" << endl;
		IOHelper::WriteLog("[Cpu-Run] -------------------- " + to_string(clockNumber) + " - MEM begin --------------------");
		if (!isReady(3))
		{
			cout << "[Cpu-Run] No work now." << endl;
			IOHelper::WriteLog("[Cpu-Run] No work now.");
			setRunInterrupted(3);
		}
		else
		{
			if (getExMemNeedLoad() != 0)
			{
				cout << "[Cpu-Run] Now load from memory..." << endl;
				IOHelper::WriteLog("[Cpu-Run] Now load from memory...");

				setMemWbWord(memory.ReadWord(getExMemAddress()));

				//not allow IF
				cout << "[Cpu-Run] Not allow IF in this clock." << endl;
				IOHelper::WriteLog("[Cpu-Run] Not allow IF in this clock.");
				setMemIfAllow(1);

				//set FW					
				cout << "[Cpu-Run] Find a vacant fw and store the data in." << endl;
				IOHelper::WriteLog("[Cpu-Run] Find a vacant fw and store the data in.");
				if (isFw0Vacant())
				{
					setFw0Index(getExMemIndex());
					setFw0Value(getMemWbWord());
				}
				else if (isFw1Vacant())
				{
					setFw1Index(getExMemIndex());
					setFw1Value(getMemWbWord());
				}
				else if (isFw2Vacant())
				{
					setFw2Index(getExMemIndex());
					setFw2Value(getMemWbWord());
				}
				else
				{
					//no vacant fw now, something occurred
					cout << "[Cpu-Run] Warning! No vacant fw now." << endl;
					IOHelper::WriteLog("[Cpu-Run] Warning! Not vacant fw now.");
				}
			}
			else if (getExMemNeedStore() != 0)
			{
				cout << "[Cpu-Run] Now store to memory..." << endl;
				IOHelper::WriteLog("[Cpu-Run] Now store to memory...");

				memory.WriteWord(getExMemAddress(), getExMemRegValue());

				//not allow IF
				cout << "[Cpu-Run] Not allow IF in this clock." << endl;
				IOHelper::WriteLog("[Cpu-Run] Not allow IF in this clock.");
				setMemIfAllow(1);

				setMemWbWord(getExMemWord());
			}
			else
			{
				cout << "[Cpu-Run] Not need to load/store now." << endl;
				IOHelper::WriteLog("[Cpu-Run] Not need to load/store now.");

				//allow IF
				cout << "[Cpu-Run] Allow IF in this clock." << endl;
				IOHelper::WriteLog("[Cpu-Run] Allow IF in this clock.");
				setMemIfAllow(0);

				setMemWbWord(getExMemWord());
			}

			cout << "[Cpu-Run] Now transfer data..." << endl;
			IOHelper::WriteLog("[Cpu-Run] Now transfer data...");
			setMemWbNeedWriteBack(getExMemNeedWriteBack());
			setMemWbIndex(getExMemIndex());
			cout << "[Cpu-Run] Data transferred done." << endl;
			IOHelper::WriteLog("[Cpu-Run] Data transferred done.");
		}
		cout << "[Cpu-Run] -------------------- " << clockNumber << " - MEM end --------------------" << endl;
		IOHelper::WriteLog("[Cpu-Run] -------------------- " + to_string(clockNumber) + " - MEM end --------------------");

		//EX
		cout << "[Cpu-Run] --------------------" << clockNumber << " - EX begin --------------------" << endl;
		IOHelper::WriteLog("[Cpu-Run] -------------------- " + to_string(clockNumber) + " - EX begin --------------------");
		if (!isReady(2))
		{
			cout << "[Cpu-Run] No work now." << endl;
			IOHelper::WriteLog("[Cpu-Run] No work now.");
			setRunInterrupted(2);
		}
		else
		{
			//get instruction type
			if (getIdExTypeR() == 1)
			{
				cout << "[Cpu-Run] Type R instruction detected." << endl;
				IOHelper::WriteLog("[Cpu-Run] Type R instruction detected.");

				//set word
				setExMemWord(getAlu().CalculateR(getIdExRs(), getIdExRt(), getIdExShamt(), getIdExFunc()));

				//set FW
				cout << "[Cpu-Run] Find a vacant fw and store the data in." << endl;
				IOHelper::WriteLog("[Cpu-Run] Find a vacant fw and store the data in.");
				if (isFw0Vacant())
				{
					setFw0Index(getIdExIndex());
					setFw0Value(getExMemWord());
				}
				else if (isFw1Vacant())
				{
					setFw1Index(getIdExIndex());
					setFw1Value(getExMemWord());
				}
				else if (isFw2Vacant())
				{
					setFw2Index(getIdExIndex());
					setFw2Value(getExMemWord());
				}
				else
				{
					//no vacant fw now, something occurred
					cout << "[Cpu-Run] Warning! Not vacant fw now." << endl;
					IOHelper::WriteLog("[Cpu-Run] Warning! Not vacant fw now.");
				}
			}
			else if (getIdExTypeI() == 1)
			{
				cout << "[Cpu-Run] Type I instruction detected." << endl;
				IOHelper::WriteLog("[Cpu-Run] Type I instruction detected.");

				switch (getIdExOp())
				{
				case 0x0C:
				case 0x0D:
				case 0x0E:
					//andi ori xori

					//set word
					setExMemWord(getAlu().CalculateI(getIdExOp(), getIdExRs(), getIdExRt(), getIdExImmediate()));

					//set FW
					cout << "[Cpu-Run] Find a vacant fw and store the data in." << endl;
					IOHelper::WriteLog("[Cpu-Run] Find a vacant fw and store the data in.");
					if (isFw0Vacant())
					{
						setFw0Index(getIdExIndex());
						setFw0Value(getExMemWord());
					}
					else if (isFw1Vacant())
					{
						setFw1Index(getIdExIndex());
						setFw1Value(getExMemWord());
					}
					else if (isFw2Vacant())
					{
						setFw2Index(getIdExIndex());
						setFw2Value(getExMemWord());
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

					if (getIdExRs() == getIdExRt())
					{
						cout << "[Cpu-Run] Branch instruction beq detected and transition condition satisfied!" << endl;
						IOHelper::WriteLog("[Cpu-Run] Branch instruction beq detected and transition condition satisfied!");

						setPc(getIdExPc() + 4 + ConvertHelper::GetSignExtendWord(getIdExImmediate() << 2));

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

					if (getIdExRs() != getIdExRt())
					{
						cout << "[Cpu-Run] Branch instruction bne detected and transition condition satisfied!" << endl;
						IOHelper::WriteLog("[Cpu-Run] Branch instruction bne detected and transition condition satisfied!");

						setPc(getIdExPc() + 4 + ConvertHelper::GetSignExtendWord(getIdExImmediate() << 2));

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
					setExMemAddress(getAlu().CalculateI(getIdExOp(), getIdExRs(), getIdExRt(), getIdExImmediate()));

					break;
				case 0x2B:
					//sw

					//set address
					setExMemAddress(getAlu().CalculateI(getIdExOp(), getIdExRs(), getIdExRt(), getIdExImmediate()));

					break;
				default:
					break;
				}
			}
			else if (getIdExTypeJ() == 1)
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
			setExMemNeedLoad(getIdExNeedLoad());
			setExMemNeedStore(getIdExNeedStore());
			setExMemRegValue(getIdExRegValue());
			setExMemNeedWriteBack(getIdExNeedWriteBack());
			setExMemIndex(getIdExIndex());
			cout << "[Cpu-Run] Data transferred done." << endl;
			IOHelper::WriteLog("[Cpu-Run] Data transferred done.");
		}
		cout << "[Cpu-Run] -------------------- " << clockNumber << " - EX end --------------------" << endl;
		IOHelper::WriteLog("[Cpu-Run] -------------------- " + to_string(clockNumber) + " - EX end --------------------");

		//ID
		cout << "[Cpu-Run] -------------------- " << clockNumber << " - ID begin --------------------" << endl;
		IOHelper::WriteLog("[Cpu-Run] -------------------- " + to_string(clockNumber) + " - ID begin -------------------- ");
		if (!isReady(1))
		{
			cout << "[Cpu-Run] No work now." << endl;
			IOHelper::WriteLog("[Cpu-Run] No work now.");
			setRunInterrupted(1);
		}
		else
		{
			//clear reg
			cout << "[Cpu-Run] Now clear regsiters..." << endl;
			IOHelper::WriteLog("[Cpu-Run] Now clear registers...");
			setIdExTypeR(0);
			setIdExTypeJ(0);
			setIdExTypeI(0);
			setIdExOp(0);
			setIdExRs(0);
			setIdExRt(0);
			setIdExRd(0);
			setIdExShamt(0);
			setIdExFunc(0);
			setIdExImmediate(0);
			setIdExAddress_(0);
			setIdExNeedLoad(0);
			setIdExNeedStore(0);
			setIdExRegValue(0);
			setIdExNeedWriteBack(0);
			setIdExIndex(0);
			cout << "[Cpu-Run] Clear registers done." << endl;
			IOHelper::WriteLog("[Cpu-Run] Clear registers done.");

			//category instruction loaded in IR
			cout << "[Cpu-Run] Now category instruction loaded in IR..." << endl;
			IOHelper::WriteLog("[Cpu-Run] Now category instruction loaded in IR...");
			switch (getDecoder().GetOp(getIr()))
			{
			case 0x00:
				cout << "[Cpu-Run] Op is 0x00, Type R instruction ensured." << endl;
				IOHelper::WriteLog("[Cpu-Run] Op is 0x00, Type R instruction ensured.");

				//type R
				setIdExTypeR(1);

				//set op
				setIdExOp(0);

				//set func
				switch (getDecoder().GetFunc(getIr()))
				{
				case 0x21:
					//addu
					cout << "[Cpu-Run] Func is 0x21, addu ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Func is 0x21, addu ensured.");
					setIdExFunc(0x21);
					break;
				case 0x23:
					//subu
					cout << "[Cpu-Run] Func is 0x23, subu ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Func is 0x23, subu ensured.");
					setIdExFunc(0x23);
					break;
				case 0x24:
					//and
					cout << "[Cpu-Run] Func is 0x24, and ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Func is 0x24, and ensured.");
					setIdExFunc(0x24);
					break;
				case 0x25:
					//or
					cout << "[Cpu-Run] Func is 0x25, or ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Func is 0x25, or ensured.");
					setIdExFunc(0x25);
					break;
				case 0x26:
					//xor
					cout << "[Cpu-Run] Func is 0x26, xor ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Func is 0x26, xor ensured.");
					setIdExFunc(0x26);
					break;
				case 0x27:
					//nor
					cout << "[Cpu-Run] Func is 0x27, nor ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Func is 0x27, nor ensured.");
					setIdExFunc(0x27);
					break;
				case 0x00:
					//sll
					cout << "[Cpu-Run] Func is 0x00, sll ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Func is 0x00, sll ensured.");
					setIdExFunc(0x00);
					break;
				case 0x02:
					//srl
					cout << "[Cpu-Run] Func is 0x02, srl ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Func is 0x02, srl ensured.");
					setIdExFunc(0x02);
					break;
				case 0x08:
					//jr
					cout << "[Cpu-Run] Func is 0x08, jr ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Func is 0x08, jr ensured.");
					setIdExFunc(0x08);

					cout << "[Cpu-Run] Transition instruction jr detected!" << endl;
					IOHelper::WriteLog("[Cpu-Run] Transition instruction jr detected!");

					cout << "[Cpu-Run] Try get [rs] from fw." << endl;
					IOHelper::WriteLog("[Cpu-Run] Try get [rs] from fw.");
					//can rs get from fw
					if (getFw0Index() == getDecoder().GetRs(getIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw0." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw0.");
						setIdExRs(getFw0Value());
					}
					else if (getFw1Index() == getDecoder().GetRs(getIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw1." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw1.");
						setIdExRs(getFw1Value());
					}
					else if (getFw2Index() == getDecoder().GetRs(getIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw2." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw2.");
						setIdExRs(getFw2Value());
					}
					else
					{
						cout << "[Cpu-Run] Fail to get [rs] from fw." << endl;
						IOHelper::WriteLog("[Cpu-Run] Fail to get [rs] from fw.");

						//is reg locked
						if (isRegLocked(getDecoder().GetRs(getIr())))
						{
							cout << "[Cpu-Run] Register r" << getDecoder().GetRs(getIr()) << " locked! Now delay..." << endl;
							IOHelper::WriteLog("[Cpu-Run] Register r" + to_string(getDecoder().GetRs(getIr())) + " locked! Now delay...");

							//delay
							throw 1 + 5;
						}
						else
						{
							cout << "[Cpu-Run] Get [rs] from gprs." << endl;
							IOHelper::WriteLog("[Cpu-Run] Get [rs] from gprs.");

							//set rs
							setIdExRs(getGeneralPurposeRegisterSet().Get(getDecoder().GetRs(getIr())));
						}
					}

					//change pc
					setPc(getIdExRs());

					//reset
					cout << "[Cpu-Run] Reset left process in this clock." << endl;
					IOHelper::WriteLog("[Cpu-Run] Reset left process in this clock.");
					throw 1;

					break;
				default:
					//not support 
					cout << "[Cpu-Run] Error! Invalid instruction " << ConvertHelper::InstructionToString(getIr()) << " detected! Now exit..." << endl;
					IOHelper::WriteLog("[Cpu-Run] Error! Invalid instruction " + ConvertHelper::InstructionToString(getIr()) + " detected! Now exit...");
					exit(0);
					break;
				}

				//set shamt
				setIdExShamt(getDecoder().GetShamt(getIr()));

				cout << "[Cpu-Run] Final write back register index is in rd here." << endl;
				IOHelper::WriteLog("[Cpu-Run] Final write back register index is in rd here.");

				//set rd
				setIdExRd(getDecoder().GetRd(getIr()));

				//set index
				setIdExIndex(getDecoder().GetRd(getIr()));

				cout << "[Cpu-Run] Try get [rt] from fw." << endl;
				IOHelper::WriteLog("[Cpu-Run] Try get [rt] from fw.");
				//can rt get from fw
				if (getFw0Index() == getDecoder().GetRt(getIr()))
				{
					cout << "[Cpu-Run] Find [rt] from fw0." << endl;
					IOHelper::WriteLog("[Cpu-Run] Find [rt] from fw0.");
					setIdExRt(getFw0Value());
				}
				else if (getFw1Index() == getDecoder().GetRt(getIr()))
				{
					cout << "[Cpu-Run] Find [rt] from fw1." << endl;
					IOHelper::WriteLog("[Cpu-Run] Find [rt] from fw1.");
					setIdExRt(getFw1Value());
				}
				else if (getFw2Index() == getDecoder().GetRt(getIr()))
				{
					cout << "[Cpu-Run] Find [rt] from fw2." << endl;
					IOHelper::WriteLog("[Cpu-Run] Find [rt] from fw2.");
					setIdExRt(getFw2Value());
				}
				else
				{
					cout << "[Cpu-Run] Fail to get [rt] from fw." << endl;
					IOHelper::WriteLog("[Cpu-Run] Fail to get [rt] from fw.");

					//is reg locked
					if (isRegLocked(getDecoder().GetRt(getIr())))
					{
						cout << "[Cpu-Run] Register r" << getDecoder().GetRt(getIr()) << " locked! Now delay..." << endl;
						IOHelper::WriteLog("[Cpu-Run] Register r" + to_string(getDecoder().GetRt(getIr())) + " locked! Now delay...");

						//delay
						throw 1 + 5;
					}
					else
					{
						cout << "[Cpu-Run] Get [rt] from gprs." << endl;
						IOHelper::WriteLog("[Cpu-Run] Get [rt] from gprs.");

						//set rt
						setIdExRt(getGeneralPurposeRegisterSet().Get(getDecoder().GetRt(getIr())));
					}
				}

				cout << "[Cpu-Run] Try get [rs] from fw." << endl;
				IOHelper::WriteLog("[Cpu-Run] Try get [rs] from fw.");
				//can rs get from fw
				if (getFw0Index() == getDecoder().GetRs(getIr()))
				{
					cout << "[Cpu-Run] Find [rs] from fw0." << endl;
					IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw0.");
					setIdExRs(getFw0Value());
				}
				else if (getFw1Index() == getDecoder().GetRs(getIr()))
				{
					cout << "[Cpu-Run] Find [rs] from fw1." << endl;
					IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw1.");
					setIdExRs(getFw1Value());
				}
				else if (getFw2Index() == getDecoder().GetRs(getIr()))
				{
					cout << "[Cpu-Run] Find [rs] from fw2." << endl;
					IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw2.");
					setIdExRs(getFw2Value());
				}
				else
				{
					cout << "[Cpu-Run] Fail to get [rs] from fw." << endl;
					IOHelper::WriteLog("[Cpu-Run] Fail to get [rs] from fw.");

					//is reg locked
					if (isRegLocked(getDecoder().GetRs(getIr())))
					{
						cout << "[Cpu-Run] Register r" << getDecoder().GetRs(getIr()) << " locked! Now delay..." << endl;
						IOHelper::WriteLog("[Cpu-Run] Register r" + to_string(getDecoder().GetRs(getIr())) + " locked! Now delay...");

						//delay
						throw 1 + 5;
					}
					else
					{
						cout << "[Cpu-Run] Get [rs] from gprs." << endl;
						IOHelper::WriteLog("[Cpu-Run] Get [rs] from gprs.");

						//set rs
						setIdExRs(getGeneralPurposeRegisterSet().Get(getDecoder().GetRs(getIr())));
					}
				}

				//lock rd
				cout << "[Cpu-Run] Lock register." << endl;
				IOHelper::WriteLog("[Cpu-Run] Lock register.");
				lockReg(getDecoder().GetRd(getIr()));

				//set need write back
				cout << "[Cpu-Run] Set need write back." << endl;
				IOHelper::WriteLog("[Cpu-Run] Set need write back.");
				setIdExNeedWriteBack(1);

				break;
			case 0x02:
				cout << "[Cpu-Run] Op is 0x02, Type R instruction ensured." << endl;
				IOHelper::WriteLog("[Cpu-Run] Op is 0x02, Type R instruction ensured.");

				//type J
				setIdExTypeJ(1);

				//j

				cout << "[Cpu-Run] Transition instruction j detected!" << endl;
				IOHelper::WriteLog("[Cpu-Run] Transition instruction j detected!");

				//change pc
				setPc(getDecoder().GetAddress(getIr()));

				//reset
				cout << "[Cpu-Run] Reset left process in this clock." << endl;
				IOHelper::WriteLog("[Cpu-Run] Reset left process in this clock.");
				throw 1;

				break;
			default:
				cout << "[Cpu-Run] Op is not 0x00 and 0x02, Type I instruction ensured." << endl;
				IOHelper::WriteLog("[Cpu-Run] Op is not 0x00 and 0x02, Type I instruction ensured.");

				//case I
				setIdExTypeI(1);

				//set op
				switch (getDecoder().GetOp(getIr()))
				{
				case 0x0C:
					//andi
					cout << "[Cpu-Run] Op is 0x0C, andi ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Op is 0x0C, andi ensured.");
					setIdExOp(0x0C);
					break;
				case 0x0D:
					//ori
					cout << "[Cpu-Run] Op is 0x0D, ori ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Op is 0x0D, ori ensured.");
					setIdExOp(0x0D);
					break;
				case 0x0E:
					//xori
					cout << "[Cpu-Run] Op is 0x0E, xori ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Op is 0x0E, xori ensured.");
					setIdExOp(0x0E);
					break;
				case 0x04:
					//beq
					cout << "[Cpu-Run] Op is 0x04, beq ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Op is 0x04, beq ensured.");
					setIdExOp(0x04);
					break;
				case 0x05:
					//bne
					cout << "[Cpu-Run] Op is 0x05, bne ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Op is 0x05, bne ensured.");
					setIdExOp(0x05);
					break;
				case 0x23:
					//lw
					cout << "[Cpu-Run] Op is 0x23, lw ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Op is 0x23, lw ensured.");
					setIdExOp(0x23);
					cout << "[Cpu-Run] Set need load." << endl;
					IOHelper::WriteLog("[Cpu-Run] Set need load.");
					setIdExNeedLoad(1);
					break;
				case 0x2B:
					//sw
					cout << "[Cpu-Run] Op is 0x2B, sw ensured." << endl;
					IOHelper::WriteLog("[Cpu-Run] Op is 0x2B, sw ensured.");
					setIdExOp(0x2B);
					cout << "[Cpu-Run] Set need store." << endl;
					IOHelper::WriteLog("[Cpu-Run] Set need store.");
					setIdExNeedStore(1);
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
					cout << "[Cpu-Run] Error! Invalid instruction " << ConvertHelper::InstructionToString(getIr()) << " detected! Now exit..." << endl;
					IOHelper::WriteLog("[Cpu-Run] Error! Invalid instruction " + ConvertHelper::InstructionToString(getIr()) + " detected! Now exit...");
					exit(0);
					break;
				}

				//set immediate
				setIdExImmediate(getDecoder().GetImmediate(getIr()));

				//rs rt related
				switch (getDecoder().GetOp(getIr()))
				{
				case 0x0C:
				case 0x0D:
				case 0x0E:
					//andi ori xori

					cout << "[Cpu-Run] Final write back register index is in rt here." << endl;
					IOHelper::WriteLog("[Cpu-Run] Final write back register index is in rt here.");

					//set rt
					setIdExRt(getDecoder().GetRt(getIr()));

					//set index
					setIdExIndex(getDecoder().GetRt(getIr()));

					//set need write back
					cout << "[Cpu-Run] Set need write back." << endl;
					IOHelper::WriteLog("[Cpu-Run] Set need write back.");
					setIdExNeedWriteBack(1);

					cout << "[Cpu-Run] Try get [rs] from fw." << endl;
					IOHelper::WriteLog("[Cpu-Run] Try get [rs] from fw.");
					//can rs get from fw
					if (getFw0Index() == getDecoder().GetRs(getIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw0." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw0.");
						setIdExRs(getFw0Value());
					}
					else if (getFw1Index() == getDecoder().GetRs(getIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw1." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw1.");
						setIdExRs(getFw1Value());
					}
					else if (getFw2Index() == getDecoder().GetRs(getIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw2." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw2.");
						setIdExRs(getFw2Value());
					}
					else
					{
						cout << "[Cpu-Run] Fail to get [rs] from fw." << endl;
						IOHelper::WriteLog("[Cpu-Run] Fail to get [rs] from fw.");

						//is reg locked
						if (isRegLocked(getDecoder().GetRs(getIr())))
						{
							cout << "[Cpu-Run] Register r" << getDecoder().GetRs(getIr()) << " locked! Now delay..." << endl;
							IOHelper::WriteLog("[Cpu-Run] Register r" + to_string(getDecoder().GetRs(getIr())) + " locked! Now delay...");

							//delay
							throw 1 + 5;
						}
						else
						{
							cout << "[Cpu-Run] Get [rs] from gprs." << endl;
							IOHelper::WriteLog("[Cpu-Run] Get [rs] from gprs.");

							//set rs
							setIdExRs(getGeneralPurposeRegisterSet().Get(getDecoder().GetRs(getIr())));
						}
					}

					//lock rt
					cout << "[Cpu-Run] Lock register." << endl;
					IOHelper::WriteLog("[Cpu-Run] Lock register.");
					lockReg(getDecoder().GetRt(getIr()));

					break;
				case 0x04:
				case 0x05:
					//beq bne

					cout << "[Cpu-Run] Try get [rt] from fw." << endl;
					IOHelper::WriteLog("[Cpu-Run] Try get [rt] from fw.");
					//can rt get from fw
					if (getFw0Index() == getDecoder().GetRt(getIr()))
					{
						cout << "[Cpu-Run] Find [rt] from fw0." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rt] from fw0.");
						setIdExRt(getFw0Value());
					}
					else if (getFw1Index() == getDecoder().GetRt(getIr()))
					{
						cout << "[Cpu-Run] Find [rt] from fw1." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rt] from fw1.");
						setIdExRt(getFw1Value());
					}
					else if (getFw2Index() == getDecoder().GetRt(getIr()))
					{
						cout << "[Cpu-Run] Find [rt] from fw2." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rt] from fw2.");
						setIdExRt(getFw2Value());
					}
					else
					{
						cout << "[Cpu-Run] Fail to get [rt] from fw." << endl;
						IOHelper::WriteLog("[Cpu-Run] Fail to get [rt] from fw.");

						//is reg locked
						if (isRegLocked(getDecoder().GetRt(getIr())))
						{
							cout << "[Cpu-Run] Register r" << getDecoder().GetRt(getIr()) << " locked! Now delay..." << endl;
							IOHelper::WriteLog("[Cpu-Run] Register r" + to_string(getDecoder().GetRt(getIr())) + " locked! Now delay...");

							//delay
							throw 1 + 5;
						}
						else
						{
							cout << "[Cpu-Run] Get [rt] from gprs." << endl;
							IOHelper::WriteLog("[Cpu-Run] Get [rt] from gprs.");

							//set rt
							setIdExRt(getGeneralPurposeRegisterSet().Get(getDecoder().GetRt(getIr())));
						}
					}

					cout << "[Cpu-Run] Try get [rs] from fw." << endl;
					IOHelper::WriteLog("[Cpu-Run] Try get [rs] from fw.");
					//can rs get from fw
					if (getFw0Index() == getDecoder().GetRs(getIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw0." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw0.");
						setIdExRs(getFw0Value());
					}
					else if (getFw1Index() == getDecoder().GetRs(getIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw1." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw1.");
						setIdExRs(getFw1Value());
					}
					else if (getFw2Index() == getDecoder().GetRs(getIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw2." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw2.");
						setIdExRs(getFw2Value());
					}
					else
					{
						cout << "[Cpu-Run] Fail to get [rs] from fw." << endl;
						IOHelper::WriteLog("[Cpu-Run] Fail to get [rs] from fw.");

						//is reg locked
						if (isRegLocked(getDecoder().GetRs(getIr())))
						{
							cout << "[Cpu-Run] Register r" << getDecoder().GetRs(getIr()) << " locked! Now delay..." << endl;
							IOHelper::WriteLog("[Cpu-Run] Register r" + to_string(getDecoder().GetRs(getIr())) + " locked! Now delay...");

							//delay
							throw 1 + 5;
						}
						else
						{
							cout << "[Cpu-Run] Get [rs] from gprs." << endl;
							IOHelper::WriteLog("[Cpu-Run] Get [rs] from gprs.");

							//set rs
							setIdExRs(getGeneralPurposeRegisterSet().Get(getDecoder().GetRs(getIr())));
						}
					}

					break;
				case 0x23:
					//lw

					cout << "[Cpu-Run] Final write back register index is in rt here." << endl;
					IOHelper::WriteLog("[Cpu-Run] Final write back register index is in rt here.");

					//set rt
					setIdExRt(getDecoder().GetRt(getIr()));

					//set index
					setIdExIndex(getDecoder().GetRt(getIr()));

					//set need write back
					cout << "[Cpu-Run] Set need write back." << endl;
					IOHelper::WriteLog("[Cpu-Run] Set need write back.");
					setIdExNeedWriteBack(1);

					cout << "[Cpu-Run] Try get [rs] from fw." << endl;
					IOHelper::WriteLog("[Cpu-Run] Try get [rs] from fw.");
					//can rs get from fw
					if (getFw0Index() == getDecoder().GetRs(getIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw0." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw0.");
						setIdExRs(getFw0Value());
					}
					else if (getFw1Index() == getDecoder().GetRs(getIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw1." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw1.");
						setIdExRs(getFw1Value());
					}
					else if (getFw2Index() == getDecoder().GetRs(getIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw2." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw2.");
						setIdExRs(getFw2Value());
					}
					else
					{
						cout << "[Cpu-Run] Fail to get [rs] from fw." << endl;
						IOHelper::WriteLog("[Cpu-Run] Fail to get [rs] from fw.");

						//is reg locked
						if (isRegLocked(getDecoder().GetRs(getIr())))
						{
							cout << "[Cpu-Run] Register r" << getDecoder().GetRs(getIr()) << " locked! Now delay..." << endl;
							IOHelper::WriteLog("[Cpu-Run] Register r" + to_string(getDecoder().GetRs(getIr())) + " locked! Now delay...");

							//delay
							throw 1 + 5;
						}
						else
						{
							cout << "[Cpu-Run] Get [rs] from gprs." << endl;
							IOHelper::WriteLog("[Cpu-Run] Get [rs] from gprs.");

							//set rs
							setIdExRs(getGeneralPurposeRegisterSet().Get(getDecoder().GetRs(getIr())));
						}
					}

					//lock rt
					cout << "[Cpu-Run] Lock register." << endl;
					IOHelper::WriteLog("[Cpu-Run] Lock register.");
					lockReg(getDecoder().GetRt(getIr()));

					break;
				case 0x2B:
					//sw

					cout << "[Cpu-Run] Try get [rt] from fw." << endl;
					IOHelper::WriteLog("[Cpu-Run] Try get [rt] from fw.");
					//can rt get from fw
					if (getFw0Index() == getDecoder().GetRt(getIr()))
					{
						cout << "[Cpu-Run] Find [rt] from fw0." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rt] from fw0.");
						setIdExRt(getFw0Value());
					}
					else if (getFw1Index() == getDecoder().GetRt(getIr()))
					{
						cout << "[Cpu-Run] Find [rt] from fw1." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rt] from fw1.");
						setIdExRt(getFw1Value());
					}
					else if (getFw2Index() == getDecoder().GetRt(getIr()))
					{
						cout << "[Cpu-Run] Find [rt] from fw2." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rt] from fw2.");
						setIdExRt(getFw2Value());
					}
					else
					{
						cout << "[Cpu-Run] Fail to get [rt] from fw." << endl;
						IOHelper::WriteLog("[Cpu-Run] Fail to get [rt] from fw.");

						//is reg locked
						if (isRegLocked(getDecoder().GetRt(getIr())))
						{
							cout << "[Cpu-Run] Register r" << getDecoder().GetRt(getIr()) << " locked! Now delay..." << endl;
							IOHelper::WriteLog("[Cpu-Run] Register r" + to_string(getDecoder().GetRt(getIr())) + " locked! Now delay...");

							//delay
							throw 1 + 5;
						}
						else
						{
							cout << "[Cpu-Run] Get [rt] from gprs." << endl;
							IOHelper::WriteLog("[Cpu-Run] Get [rt] from gprs.");

							//set rt
							setIdExRt(getGeneralPurposeRegisterSet().Get(getDecoder().GetRt(getIr())));
						}
					}

					cout << "[Cpu-Run] Set value which to store to memory." << endl;
					IOHelper::WriteLog("[Cpu-Run] Set value which to store to memory.");

					//set reg value
					setIdExRegValue(getIdExRt());

					cout << "[Cpu-Run] Try get [rs] from fw." << endl;
					IOHelper::WriteLog("[Cpu-Run] Try get [rs] from fw.");
					//can rs get from fw
					if (getFw0Index() == getDecoder().GetRs(getIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw0." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw0.");
						setIdExRs(getFw0Value());
					}
					else if (getFw1Index() == getDecoder().GetRs(getIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw1." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw1.");
						setIdExRs(getFw1Value());
					}
					else if (getFw2Index() == getDecoder().GetRs(getIr()))
					{
						cout << "[Cpu-Run] Find [rs] from fw2." << endl;
						IOHelper::WriteLog("[Cpu-Run] Find [rs] from fw2.");
						setIdExRs(getFw2Value());
					}
					else
					{
						cout << "[Cpu-Run] Fail to get [rs] from fw." << endl;
						IOHelper::WriteLog("[Cpu-Run] Fail to get [rs] from fw.");

						//is reg locked
						if (isRegLocked(getDecoder().GetRs(getIr())))
						{
							cout << "[Cpu-Run] Register r" << getDecoder().GetRs(getIr()) << " locked! Now delay..." << endl;
							IOHelper::WriteLog("[Cpu-Run] Register r" + to_string(getDecoder().GetRs(getIr())) + " locked! Now delay...");

							//delay
							throw 1 + 5;
						}
						else
						{
							cout << "[Cpu-Run] Get [rs] from gprs." << endl;
							IOHelper::WriteLog("[Cpu-Run] Get [rs] from gprs.");

							//set rs
							setIdExRs(getGeneralPurposeRegisterSet().Get(getDecoder().GetRs(getIr())));
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
			setIdExPc(getIfIdPc());
			cout << "[Cpu-Run] Data transferred done." << endl;
			IOHelper::WriteLog("[Cpu-Run] Data transferred done.");
		}
		cout << "[Cpu-Run] -------------------- " << clockNumber << " - ID end --------------------" << endl;
		IOHelper::WriteLog("[Cpu-Run] -------------------- " + to_string(clockNumber) + " - ID end --------------------");

		//IF
		cout << "[Cpu-Run] -------------------- " << clockNumber << " - IF begin --------------------" << endl;
		IOHelper::WriteLog("[Cpu-Run] -------------------- " + to_string(clockNumber) + " - IF begin --------------------");
		if (!isReady(0))
		{
			cout << "[Cpu-Run] No work now." << endl;
			IOHelper::WriteLog("[Cpu-Run] No work now.");
			setRunInterrupted(0);
		}
		else
		{
			if (getMemIfAllow() == 1)
			{
				cout << "[Cpu-Run] Find that load/store done in this clock. Now delay..." << endl;
				IOHelper::WriteLog("[Cpu-Run] Find that load/store done in this clock. Now delay...");

				//allow IF
				cout << "[Cpu-Run] Allow IF in next clock if possible." << endl;
				IOHelper::WriteLog("[Cpu-Run] Allow IF in next clock if possible.");
				setMemIfAllow(0);

				//delay
				throw 0 + 5;
			}
			else
			{
				//fetch instruction from memory to IR
				cout << "[Cpu-Run] Fetch instruction from memory to IR." << endl;
				IOHelper::WriteLog("[Cpu-Run] Fetch instruction from memory to IR.");
				setIr(memory.ReadWord(getPc()));

				//set instruction pc
				setIfIdPc(getPc());

				//modify pc
				cout << "[Cpu-Run] PC + 1" << endl;
				IOHelper::WriteLog("[Cpu-Run] PC + 1");
				setPc(getPc() + 4);
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
			setRunInterrupted(4);
		case 3:
			setRunInterrupted(3);
		case 2:
			setRunInterrupted(2);
		case 1:
			setRunInterrupted(1);
		case 0:
			setRunInterrupted(0);
			break;
		default:
			break;
		}

		//delay process
		switch (index)
		{
		case 4 + 5:
			setRunDone(3);
		case 3 + 5:
			setRunDone(2);
		case 2 + 5:
			setRunDone(1);
		case 1 + 5:
			setRunDone(0);
		case 0 + 5:
			break;
		default:
			break;
		}
	}

	//set next clock time ready
	cout << "[Cpu-Run] Now set next clock time ready..." << endl;
	IOHelper::WriteLog("[Cpu-Run] Now set next clock time ready...");
	setNewReady();

	//end of instructions detected
	if (eoiDetected)
	{
		cout << "[Cpu-Run] Eoi detected, no more valid instructions to run." << endl;
		IOHelper::WriteLog("[Cpu-Run] Eoi detected, no more valid instructions to run.");
		setNotReady(0);
	}
}

GeneralPurposeRegisterSet & Cpu::getGeneralPurposeRegisterSet()
{
	return this->gprs;
}

void Cpu::setPc(address add)
{
	cout << "[Cpu] Set register PC with value " << ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) << "(" << add << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register PC with value " + ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) + "(" + to_string(add) + ").");
	pc.Set(add);
}

address Cpu::getPc()
{
	return pc.Get();
}

void Cpu::setIr(instruction i)
{
	cout << "[Cpu] Set register IR with value " << ConvertHelper::SeperateString(ConvertHelper::InstructionToString(i)) << "(" << i << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IR with value " + ConvertHelper::SeperateString(ConvertHelper::InstructionToString(i)) + "(" + to_string(i) + ").");
	ir.Set(i);
}

instruction Cpu::getIr()
{
	return ir.Get();
}

void Cpu::lockReg(int index)
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

void Cpu::unlockReg(int index)
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

bool Cpu::isRegLocked(int index)
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

bool Cpu::isFw0Vacant()
{
	return fw0_index.Get() == 999;
}

void Cpu::setFw0Vacant()
{
	cout << "[Cpu] Set register Fw0 vacant." << endl;
	IOHelper::WriteLog("[Cpu] Set register Fw0 vacant.");
	fw0_index.Set(999);
}

void Cpu::setFw0Value(word w)
{
	cout << "[Cpu] Set register Fw0Value with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register Fw0Value with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	fw0_value.Set(w);
}

word Cpu::getFw0Value()
{
	return fw0_value.Get();
}

void Cpu::setFw0Index(word w)
{
	cout << "[Cpu] Set register Fw0Index with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register Fw0Index with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	fw0_index.Set(w);
}

word Cpu::getFw0Index()
{
	return fw0_index.Get();
}

bool Cpu::isFw1Vacant()
{
	return fw1_index.Get() == 999;
}

void Cpu::setFw1Vacant()
{
	cout << "[Cpu] Set register Fw1 vacant." << endl;
	IOHelper::WriteLog("[Cpu] Set register Fw1 vacant.");
	fw1_index.Set(999);
}

void Cpu::setFw1Value(word w)
{
	cout << "[Cpu] Set register Fw1Value with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register Fw1Value with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	fw1_value.Set(w);
}

word Cpu::getFw1Value()
{
	return fw1_value.Get();
}

void Cpu::setFw1Index(word w)
{
	cout << "[Cpu] Set register Fw1Index with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register Fw1Index with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	fw1_index.Set(w);
}

word Cpu::getFw1Index()
{
	return fw1_index.Get();
}

bool Cpu::isFw2Vacant()
{
	return fw2_index.Get() == 999;
}

void Cpu::setFw2Vacant()
{
	cout << "[Cpu] Set register Fw2 vacant." << endl;
	IOHelper::WriteLog("[Cpu] Set register Fw2 vacant.");
	fw2_index.Set(999);
}

void Cpu::setFw2Value(word w)
{
	cout << "[Cpu] Set register Fw2Value with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register Fw2Value with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	fw2_value.Set(w);
}

word Cpu::getFw2Value()
{
	return fw2_value.Get();
}

void Cpu::setFw2Index(word w)
{
	cout << "[Cpu] Set register Fw2Index with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register Fw2Index with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	fw2_index.Set(w);
}

word Cpu::getFw2Index()
{
	return fw2_index.Get();
}

Decoder Cpu::getDecoder()
{
	return decoder;
}

Alu Cpu::getAlu()
{
	return alu;
}

void Cpu::setIfIdPc(address add)
{
	cout << "[Cpu] Set register IfIdPc with value " << ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) << "(" << add << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IfIdPc with value " + ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) + "(" + to_string(add) + ").");
	if_id_pc.Set(add);
}

address Cpu::getIfIdPc()
{
	return if_id_pc.Get();
}

void Cpu::setIdExPc(address add)
{
	cout << "[Cpu] Set register IdExPc with value " << ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) << "(" << add << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExPc with value " + ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) + "(" + to_string(add) + ").");
	id_ex_pc.Set(add);
}

address Cpu::getIdExPc()
{
	return id_ex_pc.Get();
}

void Cpu::setIdExTypeR(word w)
{
	cout << "[Cpu] Set register IdExTypeR with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExTypeR with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_type_R.Set(w);
}

word Cpu::getIdExTypeR()
{
	return id_ex_type_R.Get();
}

void Cpu::setIdExTypeI(word w)
{
	cout << "[Cpu] Set register IdExTypeI with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExTypeI with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_type_I.Set(w);
}

word Cpu::getIdExTypeI()
{
	return id_ex_type_I.Get();
}

void Cpu::setIdExTypeJ(word w)
{
	cout << "[Cpu] Set register IdExTypeJ with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExTypeJ with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_type_J.Set(w);
}

word Cpu::getIdExTypeJ()
{
	return id_ex_type_J.Get();
}

void Cpu::setIdExOp(word w)
{
	cout << "[Cpu] Set register IdExOp with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExOp with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_op.Set(w);
}

word Cpu::getIdExOp()
{
	return id_ex_op.Get();
}

void Cpu::setIdExRs(word w)
{
	cout << "[Cpu] Set register IdExRs with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExRs with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_rs.Set(w);
}

word Cpu::getIdExRs()
{
	return id_ex_rs.Get();
}

void Cpu::setIdExRt(word w)
{
	cout << "[Cpu] Set register IdExRt with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExRt with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_rt.Set(w);
}

word Cpu::getIdExRt()
{
	return id_ex_rt.Get();
}

void Cpu::setIdExRd(word w)
{
	cout << "[Cpu] Set register IdExRd with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExRd with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_rd.Set(w);
}

word Cpu::getIdExRd()
{
	return id_ex_rd.Get();
}

void Cpu::setIdExShamt(word w)
{
	cout << "[Cpu] Set register IdExShamt with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExShamt with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_shamt.Set(w);
}

word Cpu::getIdExShamt()
{
	return id_ex_shamt.Get();
}

void Cpu::setIdExFunc(word w)
{
	cout << "[Cpu] Set register IdExFunc with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExFunc with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_func.Set(w);
}

word Cpu::getIdExFunc()
{
	return id_ex_func.Get();
}

void Cpu::setIdExImmediate(word w)
{
	cout << "[Cpu] Set register IdExImmediate with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExImmediate with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_immediate.Set(w);
}

word Cpu::getIdExImmediate()
{
	return id_ex_immediate.Get();
}

void Cpu::setIdExAddress_(address add)
{
	cout << "[Cpu] Set register IdExAddress_ with value " << ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) << "(" << add << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExAddress_ with value " + ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) + "(" + to_string(add) + ").");
	id_ex_address_.Set(add);
}

address Cpu::getIdExAddress_()
{
	return id_ex_address_.Get();
}

bool Cpu::isReady(int index)
{
	if (index >= 0 && index < 5)
	{
		return (isReady_.Get() & (1 << index)) >> index;
	}
	else
	{
		cout << "[Cpu] Warning! Invalid index " << index << " detected! Return true by default." << endl;
		IOHelper::WriteLog("[Cpu] Warning! Invalid index " + to_string(index) + " detected! Return true by default.");
		return true;
	}
}

void Cpu::setReady(int index)
{
	if (index >= 0 && index < 5)
	{
		//set 1
		cout << "[Cpu] Set process " << index << " ready." << endl;
		IOHelper::WriteLog("[Cpu] Set process " + to_string(index) + " ready.");
		isReady_.Set(isReady_.Get() | (1 << index));
	}
	else
	{
		cout << "[Cpu] Warning! Invalid index " << index << " detected! No process set ready in this call." << endl;
		IOHelper::WriteLog("[Cpu] Warning! Invalid index " + to_string(index) + " detected! No process set ready in this call.");
	}
}

void Cpu::setNotReady(int index)
{
	if (index >= 0 && index < 5)
	{
		//set 0
		cout << "[Cpu] Set process " << index << " not ready." << endl;
		IOHelper::WriteLog("[Cpu] Set process " + to_string(index) + " not ready.");
		isReady_.Set(isReady_.Get() & (~(1 << index)));
	}
	else
	{
		cout << "[Cpu] Warning! Invalid index " << index << " detected! No process set not ready in this call." << endl;
		IOHelper::WriteLog("[Cpu] Warning! Invalid index " + to_string(index) + " detected! No process set not ready in this call.");
	}
}

void Cpu::setNewReady()
{
	for (int i = 4; i > 0; i--)
	{
		if (isRunDone(i - 1))
		{
			setReady(i);
		}
		else
		{
			setNotReady(i);
		}
	}
	setReady(0);
}

bool Cpu::HasWork()
{
	bool result = false;
	for (int i = 0; i < 5; i++)
	{
		if (isReady(i))
		{
			result = true;
			break;
		}
	}
	return result;
}

bool Cpu::isRunDone(int index)
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

void Cpu::setRunDone(int index)
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

void Cpu::setRunInterrupted(int index)
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

void Cpu::resetRunStatus()
{
	cout << "[Cpu] Reset all processes done." << endl;
	IOHelper::WriteLog("[Cpu] Reset all processes done.");
	for (int i = 4; i >= 0; i--)
	{
		setRunDone(i);
	}
}

void Cpu::setIdExNeedLoad(word w)
{
	cout << "[Cpu] Set register IdExNeedLoad with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExNeedLoad with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_need_load.Set(w);
}

word Cpu::getIdExNeedLoad()
{
	return id_ex_need_load.Get();
}

void Cpu::setIdExNeedStore(word w)
{
	cout << "[Cpu] Set register IdExNeedStore with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExNeedStore with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_need_store.Set(w);
}

word Cpu::getIdExNeedStore()
{
	return id_ex_need_store.Get();
}

void Cpu::setIdExRegValue(word w)
{
	cout << "[Cpu] Set register IdExRegValue with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExRegValue with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_reg_value.Set(w);
}

word Cpu::getIdExRegValue()
{
	return id_ex_reg_value.Get();
}

void Cpu::setIdExNeedWriteBack(word w)
{
	cout << "[Cpu] Set register IdExNeedWriteBack with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExNeedWriteBack with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_need_write_back.Set(w);
}

word Cpu::getIdExNeedWriteBack()
{
	return id_ex_need_write_back.Get();
}

void Cpu::setIdExIndex(word w)
{
	cout << "[Cpu] Set register IdExIndex with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExIndex with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_index.Set(w);
}

word Cpu::getIdExIndex()
{
	return id_ex_index.Get();
}

void Cpu::setExMemNeedLoad(word w)
{
	cout << "[Cpu] Set register ExMemNeedLoad with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register ExMemNeedLoad with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	ex_mem_need_load.Set(w);
}

word Cpu::getExMemNeedLoad()
{
	return ex_mem_need_load.Get();
}

void Cpu::setExMemNeedStore(word w)
{
	cout << "[Cpu] Set register ExMemNeedStore with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register ExMemNeedStore with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	ex_mem_need_store.Set(w);
}

word Cpu::getExMemNeedStore()
{
	return ex_mem_need_store.Get();
}

void Cpu::setExMemAddress(address add)
{
	cout << "[Cpu] Set register ExMemNeedAddress with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(add)) << "(" << add << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register ExMemNeedAddress with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(add)) + "(" + to_string(add) + ").");
	ex_mem_address.Set(add);
}

address Cpu::getExMemAddress()
{
	return ex_mem_address.Get();
}

void Cpu::setExMemRegValue(word w)
{
	cout << "[Cpu] Set register ExMemRegValue with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register ExMemRegValue with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	ex_mem_reg_value.Set(w);
}

word Cpu::getExMemRegValue()
{
	return ex_mem_reg_value.Get();
}

void Cpu::setExMemNeedWriteBack(word w)
{
	cout << "[Cpu] Set register ExMemNeedWriteBack with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register ExMemNeedWriteBack with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	ex_mem_need_write_back.Set(w);
}

word Cpu::getExMemNeedWriteBack()
{
	return ex_mem_need_write_back.Get();
}

void Cpu::setExMemIndex(word w)
{
	cout << "[Cpu] Set register ExMemNeedIndex with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register ExMemNeedIndex with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	ex_mem_index.Set(w);
}

word Cpu::getExMemIndex()
{
	return ex_mem_index.Get();
}

void Cpu::setExMemWord(word w)
{
	cout << "[Cpu] Set register ExMemNeedWord with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register ExMemNeedWord with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	ex_mem_word.Set(w);
}

word Cpu::getExMemWord()
{
	return ex_mem_word.Get();
}

void Cpu::setMemIfAllow(word w)
{
	cout << "[Cpu] Set register MemIfAllow with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register MemIfAllow with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	mem_if_allow.Set(w);
}

word Cpu::getMemIfAllow()
{
	return mem_if_allow.Get();
}

void Cpu::setMemWbNeedWriteBack(word w)
{
	cout << "[Cpu] Set register MemWbNeedWriteBack with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register MemWbNeedWriteBack with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	mem_wb_need_write_back.Set(w);
}

word Cpu::getMemWbNeedWriteBack()
{
	return mem_wb_need_write_back.Get();
}

void Cpu::setMemWbIndex(word w)
{
	cout << "[Cpu] Set register MemWbIndex with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register MemWbIndex with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	mem_wb_index.Set(w);
}

word Cpu::getMemWbIndex()
{
	return mem_wb_index.Get();
}

void Cpu::setMemWbWord(word w)
{
	cout << "[Cpu] Set register MemWbWord with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register MemWbWord with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	mem_wb_word.Set(w);
}

word Cpu::getMemWbWord()
{
	return mem_wb_word.Get();
}
