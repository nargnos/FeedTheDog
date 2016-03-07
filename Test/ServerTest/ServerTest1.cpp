#include "stdafx.h"
#include "CppUnitTest.h"
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <boost\lockfree\queue.hpp>
#include <boost\lexical_cast.hpp>
#include <Core\Core.h>
#include <Service\EchoService.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ServerTest
{
	using namespace FeedTheDog;

	TEST_CLASS(ServerTest1)
	{

	public:
		ServerTest1()
		{
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		}
		TEST_METHOD(MemoryPoolTest)
		{

		}
		TEST_METHOD(CoreTest)
		{

			Core k;
			auto j = make_shared<EchoService>(9999);
			k.AddService(j);
			k.Start();
		}
	};
}