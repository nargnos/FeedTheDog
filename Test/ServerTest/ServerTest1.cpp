#include "stdafx.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ServerTest
{


	using namespace FeedTheDog;
	TEST_CLASS(OtherTest)
	{
	public:
		TEST_METHOD(Test)
		{
		}
	};
	TEST_CLASS(ServerTest1)
	{

	public:
		ServerTest1()
		{
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

		}

		TEST_METHOD(CoreTest)
		{
		}


	};
}