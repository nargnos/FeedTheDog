// FtdLibTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <ServiceManager.h>
using namespace FeedTheDog;


clock_t startNewSession;
clock_t endNewSession;
clock_t end;
int count;
namespace FeedTheDog
{
	class WorkerFriendProxy
	{
	public:
		template<typename TProtocol, typename TWorker,typename TSession=TWorker::TSession<TProtocol, false>>
		static const unique_ptr<typename TWorker::template TSessionPool<TSession>>&
			GetSessionPool(TWorker* worker)
		{
			return worker->GetSessionPool<TSession>();
		}
	private:
		WorkerFriendProxy() = delete;

		~WorkerFriendProxy() = delete;

	};
}  // namespace FeedTheDog


void NewSessionTest(FeedTheDog::ServiceManager*core)
{
	//10000;//
	count =  3000000;
	// 测试创建析构session的速度，这里分配之后立马析构		
	//auto& sessionPool = *FeedTheDog::WorkerFriendProxy::GetSessionPool<_ASIO ip::tcp>(core->GetWorkerPool()->SelectIdleWorker());
	startNewSession = clock();
	for (size_t i = 0; i < count; i++)
	{
		//sessionPool.NewSession();
		FeedTheDog::WorkerFriendProxy::GetSessionPool<_ASIO ip::tcp>(core->GetWorkerPool()->SelectIdleWorker())->NewSession();
	}
	endNewSession = clock();
	core->Stop();
}

//void TestLog(_STD ostringstream& os,shared_ptr<Config::TTraceSource>& trace)
//{
//	// 计算log时间
//	auto logTimes = 100000;
//	os << "Log速度" << _STD endl;
//	clock_t logStart = clock();
//	for (size_t i = 0; i < logTimes; i++)
//	{
//	//	trace->TracePoint("XXXXXXX");
//	}
//	clock_t logEnd = clock();
//	auto logTotal = logEnd - logStart;
//	os << "总" << logTotal << _STD endl;
//	os << "avg" << logTotal / (double)logTimes << _STD endl;
//}
int main()
{
	{
		FeedTheDog::ServiceManager core;

		core.GetWorkerPool()->SelectIdleWorker()->GetIoService().post(_BOOST bind(&NewSessionTest, &core));
		core.Start();
	}
	end = clock();
	// 并不能得出正确结果，只作为参考
	// 时间中少了分配内存的部分，测得的结果是new的速度与delete的速度等同时的结果
	_STD ostringstream os;
	auto total = end - startNewSession;
	os << "运行总时间 " << total << _STD endl;
	auto plus = endNewSession - startNewSession;
	os << "执行完所有new/delete操作的时间 " << plus << _STD endl;
	auto avgNew = plus / (double)count;
	os << "平均new/delete时间 " << avgNew << _STD endl;

	os << "善后时间 " << end - endNewSession << _STD endl;

	// 创建和析构是同时进行的，跟之前实现的不同
	os << "每秒可new/delete " << (double)count /plus  << _STD endl;

	os << "结束" << _STD endl;


	//TestLog(os, core.GetTrace());
	//Logger::WriteMessage(os.str().c_str());


	_STD cout << os.str();
	system("pause");
	return 0;
}

