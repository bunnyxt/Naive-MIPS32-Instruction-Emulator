#include "NaiveMIPS32InstructionEmulator.h"

Register::Register()
{
	//cout << "[Register] Initialize Register..." << endl;
	//IOHelper::WriteLog("[Register] Initialize Register...");

	data = 0;

	//cout << "[Register] Register initialized." << endl;
	//IOHelper::WriteLog("[Register] Register initialized.");
}

word Register::Get()
{
	return data;
}

void Register::Set(word w)
{
	data = w;
}