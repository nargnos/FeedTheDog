#include "stdafx.h"
#include "CppUnitTest.h"
#include <sstream>
#include <functional>
#include <Emit\Emit.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct FuncTest1
{
	static void Run()
	{
		Logger::WriteMessage("Run1");
	}
};
struct FuncTest2
{
	static void Run()
	{
		Logger::WriteMessage("Run2");
	}
};
namespace EmitTest
{
	using namespace std;
	TEST_CLASS(TestEmit)
	{
	public:
		TEST_METHOD(TestCombined)
		{
			// ��̬��������
			Emit::Combined<FuncTest1>()();
			Emit::Combined<FuncTest1, FuncTest2>()();

			// ת������
			auto tfunc = Emit::Convert<FuncTest2>();
			tfunc();

			// ���Է���ֵ����
			auto func = Emit::Combined([]()
			{
				Logger::WriteMessage("��");
			}, []()
			{
				Logger::WriteMessage("��");
				return nullptr;
			}, []()
			{
				Logger::WriteMessage("��");
			}, []()
			{
				Logger::WriteMessage("��");
				return 1;
			}, []()
			{
				Logger::WriteMessage("��");
			}, []()
			{
				Logger::WriteMessage("��");
				return "";
			});
			func();

			// ����ִ��˳��
			int i = 0;
			ostringstream out;
			function<void()> f = Emit::DoNothing();

			for (size_t i = 0; i < 10; i++)
			{
				if (i % 2 == 0)
				{
					f = Emit::Combined(f, [&, i]()
					{
						out << i << endl;
					});
				}
			}

			// 
			Emit::Combined(f, [&]()
			{
				Logger::WriteMessage(out.str().c_str());
			})();

		}
		TEST_METHOD(TestIf)
		{
			function<void()> f = Emit::DoNothing();
			for (size_t i = 0; i < 10; i++)
			{
				f = Emit::Combined(f, Emit::If(Emit::ConstValue(i % 2 == 0), []()
				{
					Logger::WriteMessage("ż��");
				}, []()
				{
					Logger::WriteMessage("����");
				}));
			}
			f();

		}

	};
}