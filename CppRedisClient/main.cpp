#include <stdio.h>
#include <sstream>
#include <iostream>
#include "Protocol.h"

using namespace Protocol;
using namespace std;
namespace Protocol
{
	struct MyStruct
	{
		int Val;
	};

	template<>
	struct StrConv<MyStruct>
	{
		static std::string ToString(const MyStruct& val)
		{
			return std::to_string(val.Val);
		}
	};
}


void TestCommand()
{
	Command cmd{ "SET","test" };
	cmd << 123;
	cout << cmd;
	char valueData[] = { "hello world" };
	vector<char> value(begin(valueData), end(valueData));
	cmd.pop_back();
	cmd << value;
	cout << cmd;

	MyStruct test{ 456 };
	cmd.pop_back();
	cmd << test;
	cout << cmd;
}

int main(void)
{
	TestCommand();
	Command cmd;
	cmd << "get" << "test";
	cout << cmd;
}