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
			add += 4;
		}
		else
		{
			//
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

		if (count == 32)
		{
			m.WriteWord(add, w);
		}
		else
		{
			//
		}
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

		if (count == 32)
		{
			gprs.Set(index, w);
		}
		else
		{
			//
		}
	}

	cout << "Register data loaded successfully!" << endl;
}