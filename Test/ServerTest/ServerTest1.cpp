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
			// 并不能得出正确结果，只作为参考
			// 时间中少了分配内存的部分，测得的结果是new的速度与delete的速度等同时的结果
			auto total = end - startNewSession;
			os << "运行总时间" << total << _STD endl;
			auto plus = endNewSession - startNewSession;
			os << "执行完new操作的时间点" << plus << _STD endl;			
			os << "善后时间" << end - endNewSession << _STD endl;

			// 善后和析构部分从开头记起，加上单独的new部分得到较正确的总时间
			auto avgTime = (total + plus) / (double)count;
			os << "单操作时间" << avgTime << _STD endl;
			os << "每秒可处理" << 1 / avgTime << _STD endl;

			os << "结束" << _STD endl;
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
			// 单纯测试创建析构session的速度，这里分配之后立马析构			
			startNewSession = clock();
			for (size_t i = 0; i < count; i++)
			{
				auto& tmpSession = worker->NewSession<_ASIO ip::tcp>("");
			}
			// 不是new结束时间，newsession把最耗时的部分post进队列了
			endNewSession = clock();
			core->Stop();
		}

	};
}