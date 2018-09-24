#include "NaiveMIPS32InstructionEmulator.h"

Alu::Alu()
{
	cout << "[Alu] Initialize Alu..." << endl;
	IOHelper::WriteLog("[Alu] Initialize Alu...");

	cout << "[Alu] Alu initialized." << endl;
	IOHelper::WriteLog("[Alu] Alu initialized.");
}

word Alu::CalculateR(word rs, word rt, word shamt, word func)
{
	word result = 0;
	switch (func)
	{
	case 0x21:
		//addu
		result = rs + rt;
		cout << "[Alu] addu : " << result << " = " << rs << " + " << rt << endl;
		IOHelper::WriteLog("[Alu] addu : " + to_string(result) + " = " + to_string(rs) + " + " + to_string(rt));
		break;
	case 0x23:
		//subu
		result = rs - rt;
		cout << "[Alu] subu : " << result << " = " << rs << " - " << rt << endl;
		IOHelper::WriteLog("[Alu] subu : " + to_string(result) + " = " + to_string(rs) + " - " + to_string(rt));
		break;
	case 0x24:
		//and
		result = rs & rt;
		cout << "[Alu] and : " << result << " = " << rs << " & " << rt << endl;
		IOHelper::WriteLog("[Alu] and : " + to_string(result) + " = " + to_string(rs) + " & " + to_string(rt));
		break;
	case 0x25:
		//or
		result = rs | rt;
		cout << "[Alu] or : " << result << " = " << rs << " | " << rt << endl;
		IOHelper::WriteLog("[Alu] or : " + to_string(result) + " = " + to_string(rs) + " | " + to_string(rt));
		break;
	case 0x26:
		//xor
		result = rs ^ rt;
		cout << "[Alu] xor : " << result << " = " << rs << " ^ " << rt << endl;
		IOHelper::WriteLog("[Alu] xor : " + to_string(result) + " = " + to_string(rs) + " ^ " + to_string(rt));
		break;
	case 0x27:
		//nor
		result = ~(rs | rt);
		cout << "[Alu] nor : " << result << " = ~(" << rs << " | " << rt << ")" << endl;
		IOHelper::WriteLog("[Alu] nor : " + to_string(result) + " = ~(" + to_string(rs) + " | " + to_string(rt)) + ")";
		break;
	case 0x00:
		//sll
		result = rt << shamt;
		cout << "[Alu] sll : " << result << " = " << rt << " << " << shamt << endl;
		IOHelper::WriteLog("[Alu] sll : " + to_string(result) + " = " + to_string(rt) + " << " + to_string(shamt));
		break;
	case 0x02:
		//srl
		result = rt >> shamt;
		cout << "[Alu] srl : " << result << " = " << rt << " >> " << shamt << endl;
		IOHelper::WriteLog("[Alu] srl : " + to_string(result) + " = " + to_string(rt) + " >> " + to_string(shamt));
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
		cout << "[Alu] andi : " << result << " = " << rs << " & " << immediate << endl;
		IOHelper::WriteLog("[Alu] andi : " + to_string(result) + " = " + to_string(rs) + " & " + to_string(immediate));
		break;
	case 0x0D:
		//ori
		result = rs | immediate;
		cout << "[Alu] ori : " << result << " = " << rs << " | " << immediate << endl;
		IOHelper::WriteLog("[Alu] ori : " + to_string(result) + " = " + to_string(rs) + " | " + to_string(immediate));
		break;
	case 0x0E:
		//xori
		result = rs ^ immediate;
		cout << "[Alu] xori : " << result << " = " << rs << " * " << immediate << endl;
		IOHelper::WriteLog("[Alu] xori : " + to_string(result) + " = " + to_string(rs) + " ^ " + to_string(immediate));
		break;
	case 0x04:
		//beq
		break;
	case 0x05:
		//bne
		break;
	case 0x23:
		//lw
		result = rs + ConvertHelper::GetSignExtendWord(immediate, 16);
		cout << "[Alu] lw : " << result << " = " << rs << " + " << ConvertHelper::GetSignExtendWord(immediate, 16) << endl;
		IOHelper::WriteLog("[Alu] lw : " + to_string(result) + " = " + to_string(rs) + " + " + to_string(ConvertHelper::GetSignExtendWord(immediate, 16)));
		break;
	case 0x2B:
		//sw
		result = rs + ConvertHelper::GetSignExtendWord(immediate, 16);
		cout << "[Alu] sw : " << result << " = " << rs << " + " << ConvertHelper::GetSignExtendWord(immediate, 16) << endl;
		IOHelper::WriteLog("[Alu] sw : " + to_string(result) + " = " + to_string(rs) + " + " + to_string(ConvertHelper::GetSignExtendWord(immediate, 16)));
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
