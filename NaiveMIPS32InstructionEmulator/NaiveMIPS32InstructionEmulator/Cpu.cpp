#include "NaiveMIPS32InstructionEmulator.h"

Cpu::Cpu() 
{

}

GeneralPurposeRegisterSet & Cpu::GetGeneralPurposeRegisterSet()
{
	return this->gprs;
}
