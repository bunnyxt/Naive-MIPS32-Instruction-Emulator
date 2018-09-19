#include "NaiveMIPS32InstructionEmulator.h"

word Alu::CalculateR(word rs, word rt, word shamt, word func)
{
	word result = 0;
	switch (func)
	{
	case 0x21:
		//addu
		result = rs + rt;
		break;
	case 0x23:
		//subu
		result = rs - rt;
		break;
	case 0x24:
		//and
		result = rs & rt;
		break;
	case 0x25:
		//or
		result = rs | rt;
		break;
	case 0x26:
		//xor
		result = rs ^ rt;
		break;
	case 0x27:
		//nor
		result = ~(rs | rt);
		break;
	case 0x00:
		//sll
		result = rt << shamt;
		break;
	case 0x02:
		//srl
		result = rt >> shamt;
		break;
	case 0x08:
		//jr
		break;
	default:
		break;
	}
	return result;
}

word Alu::CalculateI(word op, word rs, word rt, word immediate)
{
	word result = 0;
	switch (op)
	{
	case 0x0C:
		//andi
		result = rs & immediate;
		break;
	case 0x0D:
		//ori
		result = rs | immediate;
		break;
	case 0x0E:
		//xori
		result = rs ^ immediate;
		break;
	case 0x04:
		//beq
		break;
	case 0x05:
		//bne
		break;
	case 0x23:
		//lw
		result = rs & ConvertHelper::GetSignExtendWord(immediate, 16);
		break;
	case 0x2B:
		//sw
		result = rs & ConvertHelper::GetSignExtendWord(immediate, 16);
		break;
	default:
		break;
	}
	return result;
}

word Alu::CalculateJ(word op, word address)
{
	return word();
}
