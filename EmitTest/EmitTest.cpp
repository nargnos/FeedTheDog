#include "stdafx.h"
#include "CppUnitTest.h"
#include <sstream>
#include <time.h>
#include <vector>
#include <functional>
#include <Emit\Emit.h>
#include <Emit\EmitTask.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct FuncTest1
{
	static void Do()
	{
		Logger::WriteMessage("Run1");
	}
};
struct FuncTest2
{
	static void Do()
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
				out << "���ԣ�" << i;
				Logger::WriteMessage(out.str().c_str());
				s(i)();
			}


		}
		TEST_METHOD(TestTaskThen)
		{
			EmitTask task;
			int runCount = 0;
			auto incCount = [&]()
			{
				++runCount;
			};
			task.Then(incCount);
			task();
			Assert::IsTrue(runCount == 1);
			task.Then(incCount);
			task.Run();
			Assert::IsTrue(runCount == 3);
			auto decCount = [&]()
			{
				--runCount;
			};
			task.Then(decCount)
				.Then(decCount)
				.Then([]() {return false; })
				.Then([]() {Assert::Fail(); return EmitStatusType::Stop; });
			_STD function<void()> func(task);
			func();
			Assert::IsTrue(runCount == 3);
		}
		TEST_METHOD(TestTaskIf)
		{
			int count = 0;
			bool flag = false;
			auto retTrue = []()
			{
				return true;
			};
			auto retFalse = []()
			{
				return false;
			};
			auto fail = []()
			{
				Assert::Fail();
			};

			EmitTask task;

			// ����if
			task.If(retTrue)
				/*	*/.CaseFalse(fail)
				.EndIf();

			task.If(retFalse)
				/*	*/.CaseTrue(fail)
				.EndIf();

			auto f = [&]()
			{
				++count;
				flag = true;
			};

			// ˫��if
			// Ū����һЩ��lambda��һ��

			auto f1 = [&]()
			{
				flag = false;
				return true;
			};

			auto f2 = [&]()
			{
				Assert::IsTrue(flag && count == 1);
			};

			auto f3 = [&]()
			{
				flag = false;
				return false;
			};
			auto f4 = [&]()
			{
				Assert::IsTrue(flag && count == 2);
			};


			task.If(f1)
				/*	*/.CaseTrue(f)
				/*	*/.CaseFalse(f)
				.EndIf()
				.Then(f2)
				.If(f3)
				/*	*/.CaseTrue(f)
				/*	*/.CaseFalse(f)
				.EndIf()
				.Then(f4);

			// ����if
			flag = false;

			auto f5 = [&]()
			{
				++count;
				flag = true;
			};
			auto f6 = [&]()
			{
				Assert::IsTrue(flag && count == 3);
			};
			auto f7 = [&]()
			{
				++count;
				flag = false;
			};
			auto f8 = [&]()
			{
				Assert::IsTrue(count == 4);
				Assert::IsFalse(flag);
				flag = true;
				++count;
			};
			// TODO: �����Ʋ��У���bug����Ҫ�ĸ�
			// owner����Ҫ�ĳɱ�ʾ���²���ߺ��ʵ����ƣ������е��
			// ��true��false��֧�в�Ӧ�ó��������֧
			// ֮����չfor��ʱ������ṹ���ܻ�ʹ�޸Ĺ���
			// ��do���ص�������Ԫ�������

			task.If(retTrue).CaseTrue()
				/*	*/.If(retFalse)
				/*		*/.CaseTrue(fail).Then(fail)
				/*		*/.CaseFalse(f5)
				/*			*/.Then(f6)
				/*			*/.Then(f7)
				/*	*/.EndIf()
				.EndIf()
				.Then(f8);

			task.Run();
			Assert::IsTrue(flag && count == 5);
		}
	};
}