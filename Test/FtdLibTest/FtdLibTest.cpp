// FtdLibTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Core.h>
#include <worker.h>
using namespace FeedTheDog;


clock_t startNewSession;
clock_t endNewSession;
clock_t end;
int count;
void NewSessionTest(FeedTheDog::Core*core,Worker* worker)
{
	count = 100000;
	// �������Դ�������session���ٶȣ��������֮����������			
	startNewSession = clock();
	for (size_t i = 0; i < count; i++)
	{
		auto& tmpSession = worker->NewSession<_ASIO ip::tcp>();
	}
	// ����new����ʱ�䣬newsession�����ʱ�Ĳ���post��������
	endNewSession = clock();
	core->Stop();
}
void TestLog(_STD ostringstream& os,shared_ptr<Config::TTraceSource>& trace)
{
	// ����logʱ��
	auto logTimes = 100000;
	os << "Log�ٶ�" << _STD endl;
	clock_t logStart = clock();
	for (size_t i = 0; i < logTimes; i++)
	{
		trace->TracePoint("XXXXXXX");
	}
	clock_t logEnd = clock();
	auto logTotal = logEnd - logStart;
	os << "��" << logTotal << _STD endl;
	os << "avg" << logTotal / (double)logTimes << _STD endl;
}
int main()
{
	FeedTheDog::Core core;
	auto worker = core.SelectIdleWorker();
	worker->GetIoService().post(_BOOST bind(&NewSessionTest, &core, worker));
	core.Start();
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
	os << "ÿ���new " << 1 / avgNew << _STD endl;
	os << "�ƺ�ʱ�� " << end - endNewSession << _STD endl;

	// �ƺ���������ִӿ�ͷ���𣬼��ϵ�����new���ֵõ�����ȷ����ʱ��
	auto avgTime = (total + plus) / (double)count;

	os << "new/deleteʱ�� " << avgTime << _STD endl;
	os << "ÿ��ɴ���new/delete���� " << 1 / avgTime << _STD endl;

	os << "����" << _STD endl;
	
	TestLog(os, core.GetTrace());
	//Logger::WriteMessage(os.str().c_str());
	_STD cout << os.str();
	return 0;
}
