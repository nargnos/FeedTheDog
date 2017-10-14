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
//void Test(const std::string& str, ResponseType flag, size_t left, size_t right)
//{
//	auto rsp = GetLine(str.c_str(), str.length());
//	assert(std::make_tuple(flag, left, right) == rsp);
//}

//int main()
//{
//	Test("+\r\n", ResponseType::Ok, 1, 1);
//	Test("-\r\n", ResponseType::Error, 1, 1);
//	Test("*\r\n", ResponseType::Multi, 1, 1);
//	Test("$\r\n", ResponseType::Bytes, 1, 1);
//	Test(":\r\n", ResponseType::Int, 1, 1);

//	Test("+OK\r\n", ResponseType::Ok, 1, 3);
//	Test("-ERR\r\n", ResponseType::Error, 1, 4);
//	Test("*1234\r\n", ResponseType::Multi, 1, 5);
//	Test("$12345\r\n", ResponseType::Bytes, 1, 6);
//	Test(":123456\r\n", ResponseType::Int, 1, 7);

//	Test("+OK\r\n123456", ResponseType::Ok, 1, 3);
//	Test("-ERR\r\n12345", ResponseType::Error, 1, 4);
//	Test("*1234\r\n1234", ResponseType::Multi, 1, 5);
//	Test("$12345\r\n123", ResponseType::Bytes, 1, 6);
//	Test(":123456\r\n12", ResponseType::Int, 1, 7);

//	Test("123\r\n", ResponseType::DataError, 0, 0);
//	Test("12\r\n", ResponseType::DataError, 0, 0);
//	Test("1\r\n", ResponseType::DataError, 0, 0);
//	Test("\r\n", ResponseType::DataError, 0, 0);
//	Test("123", ResponseType::DataError, 0, 0);
//	Test("12", ResponseType::DataError, 0, 0);
//	Test("1", ResponseType::DataError, 0, 0);
//	Test("", ResponseType::DataError, 0, 0);

//	return 0;
//}

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
	BuildCommand(cout, "get", "test");
}