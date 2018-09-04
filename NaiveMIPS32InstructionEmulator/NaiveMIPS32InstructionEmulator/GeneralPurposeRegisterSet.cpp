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
		cout << "Warning! Cannot change value of register r0 since r0 remains 0 constantly." << endl;
	}
	else
	{
		gpr[index].Set(w);
	}
}
