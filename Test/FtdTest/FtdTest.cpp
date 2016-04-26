// FtdTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <ServiceManager.h>
#include <EchoService.h>
#include <Rfc1928.h>
#ifdef CRTDBG
class CrtSetDbgFlag
{
public:
	CrtSetDbgFlag(int flag = _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)
	{
		_CrtSetDbgFlag(flag);
		//_CrtSetBreakAlloc(650);
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
#endif
int main()
{

	using namespace FeedTheDog;

	auto& core = make_shared<ServiceManager>();
	auto a = make_shared<EchoService>(9999, "Echo No 1");
	core->AddService(a);
	auto b = make_shared<EchoService>(7788, "Echo No 2");
	core->AddService(b);
	auto c = make_shared<Rfc1928>(8080, "rfc1928 server No 1");
	core->AddService(c);
	auto d = make_shared<EchoService>(1234, "Echo No 3");
	core->AddService(d);
	auto e = make_shared<Rfc1928>(9090, "rfc1928 server No 2");
	core->AddService(e);
#if 0
	// 测试运行中途退出的情况
	auto& io = core->GetWorkerPool()->SelectIdleWorker()->GetIoService();
	_ASIO deadline_timer t(io);
	t.expires_from_now(_BOOST posix_time::seconds(60*30));
	t.async_wait(_BOOST bind(&ServiceManager::Stop, core));
#endif // fi

	core->Start();
	core->GetTrace()->TracePoint("Main End", FeedTheDog::TraceLevel::Level::Trace);

	return 0;
}

