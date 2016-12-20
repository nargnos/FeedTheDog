#include "stdafx.h"
#include "CppUnitTest.h"
#include <LuaUtil.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LuaCppTest
{
	TEST_CLASS(LuaTest)
	{
	public:
		TEST_METHOD(State)
		{
			auto lua = _STD make_shared<LuaState>();
			lua->OpenLibs_L();
			lua->DoString_L("return 'Hello World!'");
			LuaUtil util(lua);
			Assert::AreEqual("Hello World!", util.AsType<const char*>(-1));
		}
		TEST_METHOD(StackUtil)
		{
			LuaUtil lua;

			lua.Push(1);
			Assert::IsTrue(lua.State()->Type(-1) == LuaTypes::TNumber);
			Assert::IsTrue(lua.State()->ToInteger(-1) == 1);
			auto t0 = lua.ToTuple<int>(1);
			Assert::IsTrue(_STD get<0>(t0) == 1);

			lua.Push(2, true, nullptr);
			Assert::IsTrue(lua.State()->Type(-1) == LuaTypes::TNil);
			Assert::IsTrue(lua.State()->Type(-2) == LuaTypes::TBoolean);
			Assert::IsTrue(lua.State()->Type(-3) == LuaTypes::TNumber);
			Assert::IsTrue(lua.State()->ToBoolean(-2) == true);
			Assert::IsTrue(lua.State()->ToInteger(-3) == 2);
			// ´ÓÕ»¶¥¶Áµ½µ×
			auto t1 = lua.ToTuple<nullptr_t, bool, int >(-1);
			Assert::IsTrue(_STD get<0>(t1) == nullptr);
			Assert::IsTrue(_STD get<1>(t1) == true);
			Assert::IsTrue(_STD get<2>(t1) == 2);


			lua.Push(3, 1.0, "hi");
			Assert::IsTrue(lua.State()->Type(-1) == LuaTypes::TString);
			Assert::IsTrue(lua.State()->Type(-2) == LuaTypes::TNumber);
			Assert::IsTrue(lua.State()->Type(-3) == LuaTypes::TNumber);
			Assert::AreEqual(lua.State()->ToString(-1), "hi");
			Assert::IsTrue(lua.State()->ToNumber(-2) == 1.0);
			Assert::IsTrue(lua.State()->ToInteger(-3) == 3);

			auto t2 = lua.ToTuple<const char*, double, int >(-1);
			Assert::AreEqual(_STD get<0>(t2), "hi");
			Assert::IsTrue(_STD get<1>(t2) == 1.0);
			Assert::IsTrue(_STD get<2>(t2) == 3);

			auto t3 = lua.ToTuple<int, int, bool, nullptr_t, lua_Integer, double, const char*>(1);
			Assert::IsTrue(_STD get<0>(t3) == 1);
			Assert::IsTrue(_STD get<1>(t3) == 2);
			Assert::IsTrue(_STD get<2>(t3) == true);
			Assert::IsTrue(_STD get<3>(t3) == nullptr);
			Assert::IsTrue(_STD get<4>(t3) == 3);
			Assert::IsTrue(_STD get<5>(t3) == 1.0);
			Assert::AreEqual(_STD get<6>(t3), "hi");

			auto t4 = lua.ToTuple<lua_Integer, double, const char*>(5);
			Assert::IsTrue(_STD get<0>(t4) == 3);
			Assert::IsTrue(_STD get<1>(t4) == 1.0);
			Assert::AreEqual(_STD get<2>(t4), "hi");

			auto t5 = lua.ToTuple<lua_Integer, nullptr_t, bool>(-3);
			Assert::IsTrue(_STD get<0>(t5) == 3);
			Assert::IsTrue(_STD get<1>(t5) == nullptr);
			Assert::IsTrue(_STD get<2>(t5) == true);


			lua.State()->DoString_L("function Test(a,b) return a+2*b end");
			lua.State()->GetGlobal("Test");
			lua.Push(1, 3);
			lua.State()->Call(2, 1);
			auto ret = lua.AsType<int>(-1);
			Assert::IsTrue(ret == 1 + 2 * 3);
		}
		TEST_METHOD(CodeConvert)
		{
			LuaUtil lua;
			auto bin = lua.CodeToBin("function Print(val) print(val) return 123 end return Print 'Hello World!' ");

			auto testLoadBin = [](LuaUtil& l, auto buff)
			{
				auto isOk = l.LoadBin(buff) == LoadResults::Ok;
				Assert::IsTrue(isOk);
				l.State()->Call(0, 1);
				auto ret = l.State()->IsNumber(-1) ? l.State()->ToInteger(-1) : 0;
				Assert::IsTrue(ret == 123);
			};

			testLoadBin(lua, bin);

			LuaUtil lua2;
			testLoadBin(lua2, bin);
			lua2.State()->Pop(1);
			bool isOk = lua2.State()->DoString_L("return Print'Test'");
			Assert::IsTrue(isOk);
			Assert::IsTrue(lua2.State()->ToInteger(-1) == 123);

		}

	};
}