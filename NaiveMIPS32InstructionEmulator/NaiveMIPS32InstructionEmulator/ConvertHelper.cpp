#include "NaiveMIPS32InstructionEmulator.h"

string ConvertHelper::InstructionToString(instruction i) {
	return unsignedIntToString(i);
}

string ConvertHelper::AddressToString(address a)
{
	return unsignedIntToString(a);
}

string ConvertHelper::WordToString(word w) {
	return unsignedIntToString(w);
}

string ConvertHelper::SeperateString(string s, int interval, char seperator)
{
	int pos = s.size() - interval;
	while (pos > 0) {
		s = s.insert(pos, 1, seperator);
		pos -= interval;
	}
	return s;
}

string ConvertHelper::unsignedIntToString(unsigned int ui)
{
	string s = "00000000000000000000000000000000";
	for (int i = 0; i < 32; i++)
	{
		if (((ui & (1 << (31 - i))) >> (31 - i)) == 1)
		{
			s[i] = '1';
		}
	}
	return s;
}
