#include "NaiveMIPS32InstructionEmulator.h"

Memory::Memory() {
	cout << "[Memory] Initialize Memory..." << endl;
	IOHelper::WriteLog("[Memory] Initialize Memory...");

	//initialize memory data
	for (int i = 0; i < 4096; i++)//
	{
		data[i] = 0;
	}

	cout << "[Memory] Memory initialized." << endl;
	IOHelper::WriteLog("[Memory] Memory initialized.");
}

word Memory::ReadWord(address add)
{
	word result = 0;

	//remodify address
	add %= MEMSIZE;

	//align address
	add -= add % 4;

	//assemble result word
	for (int i = 3; i >= 0; i--)
	{
		result += data[add + i];
		if (i != 0)
		{
			result <<= 8;
		}
	}

	return result;
}

byte Memory::ReadByte(address add)
{
	byte result = 0;

	//remodify address
	add %= MEMSIZE;

	result = data[add];

	return result;
}

void Memory::WriteWord(address add, word w)
{
	//remodify address
	add %= MEMSIZE;

	//align address
	add -= add % 4;

	//write word
	cout << "[Memory] Set value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << to_string(w) << ")" << " in address " << add << "." << endl;
	IOHelper::WriteLog("[Memory] Set value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ")" + " in address " + to_string(add) + ".");
	for (int i = 0; i < 4; i++)
	{
		data[add + i] = (w & ((0x000000FF) << (i * 8))) >> (i * 8);
	}
}

void Memory::WriteByte(address add, byte b)
{
	//remodify address
	add %= MEMSIZE;

	//write byte
	cout << "[Memory] Set value " << ConvertHelper::SeperateString(ConvertHelper::WordToString((word)b)) << "(" << to_string((word)b) << ")" << " in address " << add << "." << endl;
	IOHelper::WriteLog("[Memory] Set value " + ConvertHelper::SeperateString(ConvertHelper::WordToString((word)b)) + "(" + to_string((word)b) + ")" + " in address " + to_string(add) + ".");
	data[add] = b;
}