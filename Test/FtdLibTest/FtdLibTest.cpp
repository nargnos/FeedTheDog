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
		template<typename TProtocol, typename TWorker>
		static const unique_ptr<typename TWorker::template TSessionPool<TProtocol>::TSessionPoolType>&
			GetSessionPool(TWorker* worker)
		{
			return worker->GetSessionPool<TProtocol>();
		}
	private:
		WorkerFriendProxy() = delete;

		~WorkerFriendProxy() = delete;

	};
}  // namespace FeedTheDog


void NewSessionTest(FeedTheDog::ServiceManager*core)
{
	count = 2000000;
	// 测试创建析构session的速度，这里分配之后立马析构			
	startNewSession = clock();
	for (size_t i = 0; i < count; i++)
	{
		auto tmpSession = FeedTheDog::WorkerFriendProxy::GetSessionPool<_ASIO ip::tcp>(core->GetWorkerPool()->SelectWorker())->NewSession();
	}
	// 不是new结束时间，newsession把最耗时的部分post进队列了
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
	os << "执行完new操作的时间 " << plus << _STD endl;
	auto avgNew = plus / (double)count;
	os << "平均new时间 " << avgNew << _STD endl;

	os << "善后时间 " << end - endNewSession << _STD endl;

	// 善后和析构部分从开头记起，加上单独的new部分得到较正确的总时间
	auto avgTime = (total + plus) / (double)count;

	os << "new/delete时间 " << avgTime << _STD endl;
	os << "每秒可new " << 1 / avgNew << _STD endl;
	os << "每秒可处理new/delete次数 " << 1 / avgTime << _STD endl;

	os << "结束" << _STD endl;


	//TestLog(os, core.GetTrace());
	//Logger::WriteMessage(os.str().c_str());


	_STD cout << os.str();
	//system("pause");
	return 0;
}

