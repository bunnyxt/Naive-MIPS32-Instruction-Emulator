#include "NaiveMIPS32InstructionEmulator.h"

Cpu::Cpu()
{
	isReady[0] = true;
	for (int i = 1; i < 5; i++)
	{
		isReady[i] = false;
	}

	fw0_index.Set(999);
	fw1_index.Set(999);
}

GeneralPurposeRegisterSet & Cpu::GetGeneralPurposeRegisterSet()
{
	return this->gprs;
}

void Cpu::SetPc(address add)
{
	cout << "[Cpu] Set register PC with value " << ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) << "(" << add << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register PC with value " + ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) + "(" + to_string(add) + ").");
	pc.Set(add);
}

address Cpu::GetPc()
{
	return pc.Get();
}

void Cpu::SetIr(instruction i)
{
	cout << "[Cpu] Set register IR with value " << ConvertHelper::SeperateString(ConvertHelper::InstructionToString(i)) << "(" << i << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IR with value " + ConvertHelper::SeperateString(ConvertHelper::InstructionToString(i)) + "(" + to_string(i) + ").");
	ir.Set(i);
}

instruction Cpu::GetIr()
{
	return ir.Get();
}

void Cpu::LockReg(int index)
{
	if (index >= 0 && index < 32)
	{
		//set 1
		lockMarker.Set(lockMarker.Get() | (1 << index));
	}
	else
	{
		cout << "Warning! Invalid index " << index << " detected! No register locked in this call." << endl;
		IOHelper::WriteLog("Warning! Invalid index " + to_string(index) + " detected! No register locked in this call.");
	}
}

void Cpu::UnlockReg(int index)
{
	if (index >= 0 && index < 32)
	{
		//set 0
		lockMarker.Set(lockMarker.Get() & (~(1 << index)));
	}
	else
	{
		cout << "Warning! Invalid index " << index << " detected! No register unlocked in this call." << endl;
		IOHelper::WriteLog("Warning! Invalid index " + to_string(index) + " detected! No register unlocked in this call.");
	}
}

bool Cpu::IsRegLocked(int index)
{
	if (index >= 0 && index < 32)
	{
		//1 locked 0 not locked
		return (lockMarker.Get() & (1 << index)) >> index;
	}
	else
	{
		cout << "Warning! Invalid index " << index << " detected! Return false by default." << endl;
		IOHelper::WriteLog("Warning! Invalid index " + to_string(index) + " detected! Return false by default.");
		return false;
	}
}

void Cpu::SetFw0Value(word w)
{
	cout << "[Cpu] Set register Fw0Value with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register Fw0Value with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	fw0_value.Set(w);
}

word Cpu::GetFw0Value()
{
	return fw0_value.Get();
}

void Cpu::SetFw0Index(word w)
{
	cout << "[Cpu] Set register Fw0Index with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register Fw0Index with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	fw0_index.Set(w);
}

word Cpu::GetFw0Index()
{
	return fw0_index.Get();
}

void Cpu::SetFw1Value(word w)
{
	cout << "[Cpu] Set register Fw1Value with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register Fw1Value with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	fw1_value.Set(w);
}

word Cpu::GetFw1Value()
{
	return fw1_value.Get();
}

void Cpu::SetFw1Index(word w)
{
	cout << "[Cpu] Set register Fw1Index with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register Fw1Index with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	fw1_index.Set(w);
}

word Cpu::GetFw1Index()
{
	return fw1_index.Get();
}

Decoder Cpu::GetDecoder()
{
	return decoder;
}

Alu Cpu::GetAlu()
{
	return alu;
}

void Cpu::SetIfIdPc(address add)
{
	cout << "[Cpu] Set register IfIdPc with value " << ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) << "(" << add << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IfIdPc with value " + ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) + "(" + to_string(add) + ").");
	if_id_pc.Set(add);
}

address Cpu::GetIfIdPc()
{
	return if_id_pc.Get();
}

void Cpu::SetIdExPc(address add)
{
	cout << "[Cpu] Set register IdExPc with value " << ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) << "(" << add << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExPc with value " + ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) + "(" + to_string(add) + ").");
	id_ex_pc.Set(add);
}

address Cpu::GetIdExPc()
{
	return id_ex_pc.Get();
}

void Cpu::SetIdExTypeR(word w)
{
	cout << "[Cpu] Set register IdExTypeR with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExTypeR with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_type_R.Set(w);
}

word Cpu::GetIdExTypeR()
{
	return id_ex_type_R.Get();
}

void Cpu::SetIdExTypeI(word w)
{
	cout << "[Cpu] Set register IdExTypeI with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExTypeI with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_type_I.Set(w);
}

word Cpu::GetIdExTypeI()
{
	return id_ex_type_I.Get();
}

void Cpu::SetIdExTypeJ(word w)
{
	cout << "[Cpu] Set register IdExTypeJ with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExTypeJ with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_type_J.Set(w);
}

word Cpu::GetIdExTypeJ()
{
	return id_ex_type_J.Get();
}

void Cpu::SetIdExOp(word w)
{
	cout << "[Cpu] Set register IdExOp with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExOp with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_op.Set(w);
}

word Cpu::GetIdExOp()
{
	return id_ex_op.Get();
}

void Cpu::SetIdExRs(word w)
{
	cout << "[Cpu] Set register IdExRs with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExRs with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_rs.Set(w);
}

word Cpu::GetIdExRs()
{
	return id_ex_rs.Get();
}

void Cpu::SetIdExRt(word w)
{
	cout << "[Cpu] Set register IdExRt with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExRt with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_rt.Set(w);
}

word Cpu::GetIdExRt()
{
	return id_ex_rt.Get();
}

void Cpu::SetIdExRd(word w)
{
	cout << "[Cpu] Set register IdExRd with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExRd with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_rd.Set(w);
}

word Cpu::GetIdExRd()
{
	return id_ex_rd.Get();
}

void Cpu::SetIdExShamt(word w)
{
	cout << "[Cpu] Set register IdExShamt with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExShamt with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_shamt.Set(w);
}

word Cpu::GetIdExShamt()
{
	return id_ex_shamt.Get();
}

void Cpu::SetIdExFunc(word w)
{
	cout << "[Cpu] Set register IdExFunc with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExFunc with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_func.Set(w);
}

word Cpu::GetIdExFunc()
{
	return id_ex_func.Get();
}

void Cpu::SetIdExImmediate(word w)
{
	cout << "[Cpu] Set register IdExImmediate with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExImmediate with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_immediate.Set(w);
}

word Cpu::GetIdExImmediate()
{
	return id_ex_immediate.Get();
}

void Cpu::SetIdExAddress_(address add)
{
	cout << "[Cpu] Set register IdExAddress_ with value " << ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) << "(" << add << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExAddress_ with value " + ConvertHelper::SeperateString(ConvertHelper::AddressToString(add)) + "(" + to_string(add) + ").");
	id_ex_address_.Set(add);
}

address Cpu::GetIdExAddress_()
{
	return id_ex_address_.Get();
}

bool Cpu::IsReady(int index)
{
	if (index >= 0 && index < 5)
	{
		return isReady[index];
	}
	else
	{
		return true;
	}
}

void Cpu::MoveOnReady()
{
	for (int i = 4; i > 0; i--)
	{
		isReady[i] = isReady[i - 1];
	}
}

void Cpu::DropReady(int index)
{
	if (index >= 0 && index < 5)
	{
		for (int i = index; i > 0; i--)
		{
			isReady[i] = false;
		}
		isReady[0] = true;
		for (int i = 4; i > index; i--)
		{
			isReady[i] = isReady[i - 1];
		}
	}
}

void Cpu::SetIdExNeedLoad(word w)
{
	cout << "[Cpu] Set register IdExNeedLoad with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExNeedLoad with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_need_load.Set(w);
}

word Cpu::GetIdExNeedLoad()
{
	return id_ex_need_load.Get();
}

void Cpu::SetIdExNeedStore(word w)
{
	cout << "[Cpu] Set register IdExNeedStore with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExNeedStore with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_need_store.Set(w);
}

word Cpu::GetIdExNeedStore()
{
	return id_ex_need_store.Get();
}

//void Cpu::SetIdExAddress(address add)
//{
//	id_ex_address.Set(add);
//}
//
//address Cpu::GetIdExAddress()
//{
//	return id_ex_address.Get();
//}

void Cpu::SetIdExRegValue(word w)
{
	cout << "[Cpu] Set register IdExRegValue with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExRegValue with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_reg_value.Set(w);
}

word Cpu::GetIdExRegValue()
{
	return id_ex_reg_value.Get();
}

void Cpu::SetIdExNeedWriteBack(word w)
{
	cout << "[Cpu] Set register IdExNeedWriteBack with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExNeedWriteBack with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_need_write_back.Set(w);
}

word Cpu::GetIdExNeedWriteBack()
{
	return id_ex_need_write_back.Get();
}

void Cpu::SetIdExIndex(word w)
{
	cout << "[Cpu] Set register IdExIndex with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register IdExIndex with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	id_ex_index.Set(w);
}

word Cpu::GetIdExIndex()
{
	return id_ex_index.Get();
}

//void Cpu::SetIdExWord(word w)
//{
//	id_ex_word.Set(w);
//}
//
//word Cpu::GetIdExWord()
//{
//	return id_ex_word.Get();
//}

void Cpu::SetExMemNeedLoad(word w)
{
	cout << "[Cpu] Set register ExMemNeedLoad with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register ExMemNeedLoad with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	ex_mem_need_load.Set(w);
}

word Cpu::GetExMemNeedLoad()
{
	return ex_mem_need_load.Get();
}

void Cpu::SetExMemNeedStore(word w)
{
	cout << "[Cpu] Set register ExMemNeedStore with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register ExMemNeedStore with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	ex_mem_need_store.Set(w);
}

word Cpu::GetExMemNeedStore()
{
	return ex_mem_need_store.Get();
}

void Cpu::SetExMemAddress(address add)
{
	cout << "[Cpu] Set register ExMemNeedAddress with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(add)) << "(" << add << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register ExMemNeedAddress with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(add)) + "(" + to_string(add) + ").");
	ex_mem_address.Set(add);
}

address Cpu::GetExMemAddress()
{
	return ex_mem_address.Get();
}

void Cpu::SetExMemRegValue(word w)
{
	cout << "[Cpu] Set register ExMemRegValue with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register ExMemRegValue with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	ex_mem_reg_value.Set(w);
}

word Cpu::GetExMemRegValue()
{
	return ex_mem_reg_value.Get();
}

void Cpu::SetExMemNeedWriteBack(word w)
{
	cout << "[Cpu] Set register ExMemNeedWriteBack with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register ExMemNeedWriteBack with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	ex_mem_need_write_back.Set(w);
}

word Cpu::GetExMemNeedWriteBack()
{
	return ex_mem_need_write_back.Get();
}

void Cpu::SetExMemIndex(word w)
{
	cout << "[Cpu] Set register ExMemNeedIndex with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register ExMemNeedIndex with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	ex_mem_index.Set(w);
}

word Cpu::GetExMemIndex()
{
	return ex_mem_index.Get();
}

void Cpu::SetExMemWord(word w)
{
	cout << "[Cpu] Set register ExMemNeedWord with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register ExMemNeedWord with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	ex_mem_word.Set(w);
}

word Cpu::GetExMemWord()
{
	return ex_mem_word.Get();
}

void Cpu::SetMemIfAllow(word w)
{
	cout << "[Cpu] Set register MemIfAllow with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register MemIfAllow with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	mem_if_allow.Set(w);
}

word Cpu::GetMemIfAllow()
{
	return mem_if_allow.Get();
}

void Cpu::SetMemWbNeedWriteBack(word w)
{
	cout << "[Cpu] Set register MemWbNeedWriteBack with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register MemWbNeedWriteBack with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	mem_wb_need_write_back.Set(w);
}

word Cpu::GetMemWbNeedWriteBack()
{
	return mem_wb_need_write_back.Get();
}

void Cpu::SetMemWbIndex(word w)
{
	cout << "[Cpu] Set register MemWbIndex with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register MemWbIndex with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	mem_wb_index.Set(w);
}

word Cpu::GetMemWbIndex()
{
	return mem_wb_index.Get();
}

void Cpu::SetMemWbWord(word w)
{
	cout << "[Cpu] Set register MemWbWord with value " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << "(" << w << ")." << endl;
	IOHelper::WriteLog("[Cpu] Set register MemWbWord with value " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + "(" + to_string(w) + ").");
	mem_wb_word.Set(w);
}

word Cpu::GetMemWbWord()
{
	return mem_wb_word.Get();
}
