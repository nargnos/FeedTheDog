#include "stdafx.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ServerTest
{
	using namespace FeedTheDog;
	TEST_CLASS(OtherTest)
	{
	public:
		TEST_METHOD(Test)
		{
		}
	};
	TEST_CLASS(ServerTest1)
	{

	public:
		ServerTest1()
		{
			//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
			core = make_shared<Core>();

		}
		TEST_METHOD(CoreTest)
		{
			auto worker = core->SelectIdleWorker();
			worker->GetIoService().post(_BOOST bind(&ServerTest1::NewSessionTest, this, worker));
			core->Start();
			end = clock();
			// �����ܵó���ȷ�����ֻ��Ϊ�ο�
			// ʱ�������˷����ڴ�Ĳ��֣���õĽ����new���ٶ���delete���ٶȵ�ͬʱ�Ľ��
			auto total = end - startNewSession;
			os << "������ʱ��" << total << _STD endl;
			auto plus = endNewSession - startNewSession;
			os << "ִ����new������ʱ���" << plus << _STD endl;			
			os << "�ƺ�ʱ��" << end - endNewSession << _STD endl;

			// �ƺ���������ִӿ�ͷ���𣬼��ϵ�����new���ֵõ�����ȷ����ʱ��
			auto avgTime = (total + plus) / (double)count;
			os << "������ʱ��" << avgTime << _STD endl;
			os << "ÿ��ɴ���" << 1 / avgTime << _STD endl;

			os << "����" << _STD endl;
			Logger::WriteMessage(os.str().c_str());
		}
	private:
		shared_ptr<FeedTheDog::Core> core;
		_STD ostringstream os;
		clock_t startNewSession;
		clock_t endNewSession;
		clock_t end;
		int count;
		void NewSessionTest(Worker* worker)
		{
			count = 150000;
			// �������Դ�������session���ٶȣ��������֮����������			
			startNewSession = clock();
			for (size_t i = 0; i < count; i++)
			{
				auto& tmpSession = worker->NewSession<_ASIO ip::tcp>("");
			}
			// ����new����ʱ�䣬newsession�����ʱ�Ĳ���post��������
			endNewSession = clock();
			core->Stop();
		}

	};
}