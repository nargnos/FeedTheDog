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
	// ���Դ�������session���ٶȣ��������֮����������			
	startNewSession = clock();
	for (size_t i = 0; i < count; i++)
	{
		auto tmpSession = FeedTheDog::WorkerFriendProxy::GetSessionPool<_ASIO ip::tcp>(core->GetWorkerPool()->SelectWorker())->NewSession();
	}
	// ����new����ʱ�䣬newsession�����ʱ�Ĳ���post��������
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
	os << "ִ����new������ʱ�� " << plus << _STD endl;
	auto avgNew = plus / (double)count;
	os << "ƽ��newʱ�� " << avgNew << _STD endl;

	os << "�ƺ�ʱ�� " << end - endNewSession << _STD endl;

	// �ƺ���������ִӿ�ͷ���𣬼��ϵ�����new���ֵõ�����ȷ����ʱ��
	auto avgTime = (total + plus) / (double)count;

	os << "new/deleteʱ�� " << avgTime << _STD endl;
	os << "ÿ���new " << 1 / avgNew << _STD endl;
	os << "ÿ��ɴ���new/delete���� " << 1 / avgTime << _STD endl;

	os << "����" << _STD endl;


	//TestLog(os, core.GetTrace());
	//Logger::WriteMessage(os.str().c_str());


	_STD cout << os.str();
	//system("pause");
	return 0;
}

