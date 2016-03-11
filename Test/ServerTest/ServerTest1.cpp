#include "stdafx.h"
#include "CppUnitTest.h"
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <concurrent_unordered_map.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ServerTest
{
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