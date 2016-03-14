// FtdTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <Core.h>
#include <EchoService.h>

class CrtSetDbgFlag
{
public:
	CrtSetDbgFlag(int flag = _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)
	{
		_CrtSetDbgFlag(flag);
		//_CrtSetBreakAlloc(186);
		/*long bpGrp[] = { };
		for (size_t i = 0; i < _ARRAYSIZE(bpGrp); i++)
		{
			_CrtSetBreakAlloc(bpGrp[i]);
		}*/
	}

	~CrtSetDbgFlag()
	{
		_CrtDumpMemoryLeaks();
	}

private:

};

CrtSetDbgFlag dbg(_CRTDBG_ALLOC_MEM_DF);

int main()
{

	using namespace FeedTheDog;

	auto& core = make_shared<Core>();
	auto a = make_shared<EchoService>(9999, "Echo No1");
	core->AddService(a);
	auto b = make_shared<EchoService>(7788, "Echo No2");
	core->AddService(b);
#if 1
	auto& io = core->SelectIdleWorker()->GetIoService();
	_ASIO deadline_timer t(io);
	t.expires_from_now(_BOOST posix_time::seconds(60*9));
	t.async_wait(_BOOST bind(&Core::Stop, core));
#endif // fi

	core->Start();
	core->GetTrace()->TracePoint(LogMsg::MainEnd, FeedTheDog::TraceLevel::Trace);
	return 0;
}

