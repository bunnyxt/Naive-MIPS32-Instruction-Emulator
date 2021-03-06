#include "NaiveMIPS32InstructionEmulator.h"

GeneralPurposeRegisterSet::GeneralPurposeRegisterSet()
{
	cout << "[GeneralPurposeRegisterSet] Initialize GeneralPurposeRegisterSet..." << endl;
	IOHelper::WriteLog("[GeneralPurposeRegisterSet] Initialize GeneralPurposeRegisterSet...");

	cout << "[GeneralPurposeRegisterSet] GeneralPurposeRegisterSet initialized." << endl;
	IOHelper::WriteLog("[GeneralPurposeRegisterSet] GeneralPurposeRegisterSet initialized.");
}

word GeneralPurposeRegisterSet::Get(int index)
{
	if (index >= 0 && index < 32)
	{
		return gpr[index].Get();
	}
	else
	{
		cout << "[GeneralPurposeRegisterSet] Warning! Cannot find register r" << index << " in gprs! Now return 0." << endl;
		IOHelper::WriteLog("[GeneralPurposeRegisterSet] Warning! Cannot find register r" + to_string(index) + " in gprs! Now return 0.");
		return 0;
	}
}

void GeneralPurposeRegisterSet::Set(int index, word w)
{
	if (index == 0)
	{
		// cannot change data in r0
		cout << "[GeneralPurposeRegisterSet] Warning! Cannot change value of register r0 since r0 remains 0 constantly." << endl;
		IOHelper::WriteLog("[GeneralPurposeRegisterSet] Warning! Cannot change value of register r0 since r0 remains 0 constantly.");
	}
	else if (index > 0 && index < 32)
	{
		cout << "[GeneralPurposeRegisterSet] Set register r" << index << " with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
		IOHelper::WriteLog("[GeneralPurposeRegisterSet] Set register r" + to_string(index) + " with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
		gpr[index].Set(w);
	}
	else
	{
		cout << "[GeneralPurposeRegisterSet] Warning! Cannot find register r" << index << " in gprs!" << endl;
		IOHelper::WriteLog("[GeneralPurposeRegisterSet] Warning! Cannot find register r" + to_string(index) + " in gprs!");
	}
}
