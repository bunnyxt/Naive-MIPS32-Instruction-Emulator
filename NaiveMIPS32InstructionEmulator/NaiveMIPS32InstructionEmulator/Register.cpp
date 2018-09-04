#include "NaiveMIPS32InstructionEmulator.h"

Register::Register()
{
	data = 0;
}

word Register::Get()
{
	return data;
}

void Register::Set(word w) 
{
	data = w;
}