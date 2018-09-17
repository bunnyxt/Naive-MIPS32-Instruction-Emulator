#include "NaiveMIPS32InstructionEmulator.h"

void IOHelper::LoadCode(Memory &m, ifstream &in)
{
	address add = 0;//code loaded from address 0
	string s;
	instruction i;

	cout << "Now loading codes..." << endl;

	while (getline(in, s))
	{
		i = 0;
		int pos = 0, count = 0;
		char c = s[pos];

		while (c != '\0')
		{
			if (c == '0')
			{
				i <<= 1;
				count++;
			}
			if (c == '1')
			{
				i <<= 1;
				i++;
				count++;
			}
			c = s[++pos];
			if (count == 32)
			{
				break;
			}
		}

		if (count == 32)
		{
			m.WriteWord(add, i);
			cout << "Instruction " << ConvertHelper::SeperateString(ConvertHelper::InstructionToString(i)) << " loaded in address " << add << "." << endl;
			WriteLog("Instruction " + ConvertHelper::SeperateString(ConvertHelper::InstructionToString(i)) + " loaded in address " + to_string(add) + ".");
			add += 4;
		}
		else
		{
			cout << "Warning! Cannot load invalid instruction " << ConvertHelper::SeperateString(ConvertHelper::InstructionToString(i)) << "." << endl;
			WriteLog("Warning! Cannot load invalid instruction " + ConvertHelper::SeperateString(ConvertHelper::InstructionToString(i)) + ".");
		}
	}

	cout << "Code loaded successfully!" << endl;
}

void IOHelper::LoadMemoryData(Memory &m, ifstream &in)
{
	address add;
	string s;
	word w;

	cout << "Now loading memory data..." << endl;

	while (getline(in, s))
	{
		int pos = 0;
		char c = s[pos];
		while (c >= '0' && c <= '9')
		{
			c = s[++pos];
		}
		string sub = s.substr(0, pos);
		add = atoi(sub.c_str());

		w = 0;
		int count = 0;

		while (c != '\0')
		{
			if (c == '0')
			{
				w <<= 1;
				count++;
			}
			if (c == '1')
			{
				w <<= 1;
				w++;
				count++;
			}
			c = s[++pos];
			if (count == 32)
			{
				break;
			}
		}

		//if (count == 32)
		//{
		m.WriteWord(add, w);
		cout << "Data " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << " loaded in address " << add << "." << endl;
		WriteLog("Data " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + " loaded in address " + to_string(add) + ".");
		//}
		//else
		//{
		//	cout << "Cannot load invalid data " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << " in address " << add << "!" << endl;
		//}
	}

	cout << "Memory data loaded successfully!" << endl;
}

void IOHelper::LoadRegisterData(GeneralPurposeRegisterSet &gprs, ifstream &in)
{
	int index;
	string s;
	word w;

	cout << "Now loading register data..." << endl;

	while (getline(in, s))
	{
		int pos = 0;
		char c = s[pos];
		while (c >= '0' && c <= '9')
		{
			c = s[++pos];
		}
		string sub = s.substr(0, pos);
		index = atoi(sub.c_str());

		w = 0;
		int count = 0;

		while (c != '\0')
		{
			if (c == '0')
			{
				w <<= 1;
				count++;
			}
			if (c == '1')
			{
				w <<= 1;
				w++;
				count++;
			}
			c = s[++pos];
			if (count == 32)
			{
				break;
			}
		}

		//if (count == 32)
		//{
		if (index >= 0 && index <= 31)
		{
			gprs.Set(index, w);
			if (index != 0)
			{
				cout << "Data " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << " loaded in register " << index << "." << endl;
				WriteLog("Data " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + " loaded in register " + to_string(index) + ".");
			}
		}
		else
		{
			cout << "Warning! Cannot load data " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << " in invalid register " << index << "." << endl;
			WriteLog("Warning! Cannot load data " + ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) + " in invalid register " + to_string(index) + ".");
		}
		//}
		//else
		//{
		//	cout << "Cannot load invalid data " << ConvertHelper::SeperateString(ConvertHelper::WordToString(w)) << " in register " << index << "!" << endl;
		//}
	}

	cout << "Register data loaded successfully!" << endl;
}

bool IOHelper::OpenLogFileStream()
{
	string fileName = "log_";
	time_t t = time(NULL);
	tm* local = localtime(&t);
	char buf[128] = { 0 };
	strftime(buf, 64, "%Y-%m-%d_%H-%M-%S", local);
	fileName += buf;
	fileName += ".txt";
	log.open(fileName);
	if (!log.is_open())
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool IOHelper::WriteLog(string s)
{
	if (log.is_open())
	{
		time_t t = time(NULL);
		tm* local = localtime(&t);
		char buf[128] = { 0 };
		strftime(buf, 64, "%Y-%m-%d_%H:%M:%S", local);
		log << buf << " " << s << endl;;
		return true;
	}
	else
	{
		return false;
	}
}

bool IOHelper::CloseLogFileStream()
{
	if (log.is_open())
	{
		log.close();
		return true;
	}
	else
	{
		return false;
	}
}
