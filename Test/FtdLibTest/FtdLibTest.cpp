// FtdLibTest.cpp : �������̨Ӧ�ó������ڵ㡣
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
	// ���Դ�������session���ٶȣ��������֮����������		
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
//	// ����logʱ��
//	auto logTimes = 100000;
//	os << "Log�ٶ�" << _STD endl;
//	clock_t logStart = clock();
//	for (size_t i = 0; i < logTimes; i++)
//	{
//	//	trace->TracePoint("XXXXXXX");
//	}
//	clock_t logEnd = clock();
//	auto logTotal = logEnd - logStart;
//	os << "��" << logTotal << _STD endl;
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
	// �����ܵó���ȷ�����ֻ��Ϊ�ο�
	// ʱ�������˷����ڴ�Ĳ��֣���õĽ����new���ٶ���delete���ٶȵ�ͬʱ�Ľ��
	_STD ostringstream os;
	auto total = end - startNewSession;
	os << "������ʱ�� " << total << _STD endl;
	auto plus = endNewSession - startNewSession;
	os << "ִ��������new/delete������ʱ�� " << plus << _STD endl;
	auto avgNew = plus / (double)count;
	os << "ƽ��new/deleteʱ�� " << avgNew << _STD endl;

	os << "�ƺ�ʱ�� " << end - endNewSession << _STD endl;

	// ������������ͬʱ���еģ���֮ǰʵ�ֵĲ�ͬ
	os << "ÿ���new/delete " << (double)count /plus  << _STD endl;

	os << "����" << _STD endl;


	//TestLog(os, core.GetTrace());
	//Logger::WriteMessage(os.str().c_str());


	_STD cout << os.str();
	system("pause");
	return 0;
}

