#include "NaiveMIPS32InstructionEmulator.h"

GeneralPurposeRegisterSet::GeneralPurposeRegisterSet()
{

}

word GeneralPurposeRegisterSet::Get(int index)
{
	return gpr[index].Get();
}

void GeneralPurposeRegisterSet::Set(int index, word w)
{
	if (index == 0)
	{
		// cannot change data in r0
	}
	else
	{
		gpr[index].Set(w);
	}
}
