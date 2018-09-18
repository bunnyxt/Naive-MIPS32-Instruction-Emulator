#include "NaiveMIPS32InstructionEmulator.h"

word Decoder::GetOp(instruction i)
{
	return getSubNumInUnsignedInt(i, 31, 6);
}

word Decoder::GetRs(instruction i)
{
	return getSubNumInUnsignedInt(i, 25, 5);
}

word Decoder::GetRt(instruction i)
{
	return getSubNumInUnsignedInt(i, 20, 5);
}

word Decoder::GetRd(instruction i)
{
	return getSubNumInUnsignedInt(i, 15, 5);
}

word Decoder::GetShamt(instruction i)
{
	return getSubNumInUnsignedInt(i, 10, 5);
}

word Decoder::GetFunc(instruction i)
{
	return getSubNumInUnsignedInt(i, 5, 6);
}

word Decoder::GetImmediate(instruction i)
{
	return getSubNumInUnsignedInt(i, 15, 16);
}

address Decoder::GetAddress(instruction i)
{
	return getSubNumInUnsignedInt(i, 25, 26);
}

unsigned int Decoder::getSubNumInUnsignedInt(unsigned int ui, int start, int length)
{
	unsigned int t = 0;
	for (int i = 0; i < length; i++)
	{
		t <<= 1;
		t += 1;
	}
	t <<= start - length + 1;
	return (ui & t) >> (start - length + 1);
}
