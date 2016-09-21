#include "stdafx.h"
#include "CppUnitTest.h"
#include <sstream>
#include <time.h>
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
			// ¾²Ì¬º¯Êı²âÊÔ
			Emit::Combined<FuncTest1>()();
			Emit::Combined<FuncTest1, FuncTest2>()();

			// ×ª»»²âÊÔ
			auto tfunc = Emit::Convert<FuncTest2>();
			tfunc();

			// ºöÂÔ·µ»ØÖµ²âÊÔ
			auto func = Emit::Combined([]()
			{
				Logger::WriteMessage("¢Ù");
			}, []()
			{
				Logger::WriteMessage("¢Ú");
				return nullptr;
			}, []()
			{
				Logger::WriteMessage("¢Û");
			}, []()
			{
				Logger::WriteMessage("¢Ü");
				return 1;
			}, []()
			{
				Logger::WriteMessage("¢İ");
			}, []()
			{
				Logger::WriteMessage("¢Ş");
				return "";
			});
			func();

			// ²âÊÔÖ´ĞĞË³Ğò
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
					Logger::WriteMessage("Å¼Êı");
				}, []()
				{
					Logger::WriteMessage("ÆæÊı");
				}));
			}
			f();

		}
		TEST_METHOD(TestSwitch)
		{
			auto s = [](int val)
			{
				return Emit::SwitchDefault(Emit::ConstValue(val), []()
				{
					Logger::WriteMessage("default");
				}, Emit::Case(0, []()
				{
					Logger::WriteMessage("case 0");
				}), Emit::Case<int, 1>([]()
				{
					Logger::WriteMessage("case 1");
				}), Emit::RangeCase([](auto val)
				{
					return val >= 2 && val <= 3;
				}, []()
				{
					Logger::WriteMessage("case 2-3");
				}));
			};

			for (size_t i = 0; i < 5; i++)
			{
				ostringstream out;
				out << "²âÊÔ£º" << i;
				Logger::WriteMessage(out.str().c_str());
				s(i)();
			}


		}

		
	};
}