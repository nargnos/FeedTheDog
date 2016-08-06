// FtdTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <ServiceManager.h>
#include <EchoService.h>
#include <Rfc1928.h>
#ifdef CRTDBG
CrtSetDbgFlag dbg;
#endif
int main()
{
	// FIX: 弄个manager目的是提供服务间统一的交流方式，但是好像各个服务间不需要什么交流啊。。。需要运行多个服务就改成分开成各个程序吧，manager删掉
	// 需要弄一个机制直接替换（或hook）掉发送接收函数，这样方便修改加密
	// （用队列asio已经有了，在出入队前后修改数据（需要注意加密后数据扩展的处理），发送接收需要桥接asio的函数，并且统一成较少的函数，这样不用写太多）
	// 整体这样设计不太好，业务逻辑想用lua替换，再想想怎么设计
	using namespace FeedTheDog;

	auto& core = make_shared<ServiceManager>();
	auto a = make_shared<EchoService>(1234, "Echo No 1");
	core->AddService(a);
	auto b = make_shared<EchoService>(7788, "Echo No 2");
	core->AddService(b);
	auto c = make_shared<Rfc1928>(8080, "rfc1928 server No 1");
	core->AddService(c);
	auto d = make_shared<EchoService>(9999, "Echo No 3");
	core->AddService(d);
	auto e = make_shared<Rfc1928>(9090, "rfc1928 server No 2");
	core->AddService(e);
#if 1
	// 测试运行中途退出的情况
	auto& io = core->GetWorkerPool()->GetIoService();
	_ASIO deadline_timer t(io);
	t.expires_from_now(_BOOST posix_time::seconds(60*2));
	t.async_wait(_BOOST bind(&ServiceManager::Stop, core));
#endif // fi

	core->Start();
	//core->GetTrace()->TracePoint("Main End", FeedTheDog::TraceLevel::Level::Trace);

	return 0;
}

