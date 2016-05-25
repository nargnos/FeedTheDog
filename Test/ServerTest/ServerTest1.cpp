#include "stdafx.h"
#include "ObjectPool.h"
#include "Worker.h"
#include "WorkerPool.h"
#include "ServiceManager.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace ServerTest
{
	class LibTest;
}
namespace FeedTheDog
{
	class WorkerPoolFriendProxy
	{
	public:
		friend class ServerTest::LibTest;
	private:
		static void Start(WorkerPool* pool)
		{
			pool->Join();
		}
		static void Stop(WorkerPool* pool)
		{
			pool->Stop();
		}
		WorkerPoolFriendProxy() = delete;

		~WorkerPoolFriendProxy() = delete;
	};
}  // namespace FeedTheDog

namespace ServerTest
{
	class TestObject
	{
	public:
		TestObject()
		{
			static int count = 0;
			i_ = count++;
			Output("ctor");
		}

		~TestObject()
		{
			Output("dtor");
		}
		TestObject(const TestObject& v)
		{
			i_ = v.i_;
			Output("copy");
		}
		TestObject(const TestObject&& v)
		{
			i_ = v.i_;
			Output("move");
		}
		TestObject& operator=(const TestObject& v)
		{
			i_ = v.i_;
			Output("copy=");
			return *this;
		}
		TestObject& operator=(const TestObject&& v)
		{
			i_ = v.i_;
			Output("move=");
			return *this;
		}
	private:
		void Output(const char* str)
		{
			std::ostringstream out;
			out << str << " -> " << i_;
			Logger::WriteMessage(out.str().c_str());
		}
		int i_;
	};

	template<typename TPool>
	void TestObjectPool(TPool& pool)
	{
		auto unique = pool.Construct<Unique>();
		{
			auto ptr = pool.Construct<Shared>();
		}

		auto ptr = pool.Construct<Pointer>();
		ptr.Delete();

	/*	ptr = pool.Construct<None>();
		auto dlt = pool.GetDeletor();
		dlt(ptr);

		ptr = pool.Construct<None>();
		pool.GetDeletor().Scope(ptr);*/
	}

	using namespace FeedTheDog;
	TEST_CLASS(LibTest)
	{
	public:
		TEST_METHOD(ObjectPoolTest)
		{
			// Object pool
			{
				ObjectPool<TestObject> pool;
				TestObjectPool(pool);
				// 未析构对象
				pool.Construct<Pointer>();
				auto ptr=pool.Construct<Pointer>();
				bool b1 = nullptr == ptr;
				bool b2 = ptr == nullptr;
				bool b3 = nullptr != ptr;
				bool b4 = ptr != nullptr;
				bool b5 = (bool)ptr;
				auto copy = ptr;
				bool b6 = ptr == copy;
				bool b7 = copy != ptr;
				Assert::IsTrue(b1 == false);
				Assert::IsTrue(b2 == false);
				Assert::IsTrue(b3 == true);
				Assert::IsTrue(b4 == true);
				Assert::IsTrue(b5 == true);
				Assert::IsTrue(b6 == true);
				Assert::IsTrue(b7 == false);
			}
			Logger::WriteMessage("=========");
			{
				ObjectPool<TestObject, _BOOST pool> pool;
				TestObjectPool(pool);
			}
			Logger::WriteMessage("=========");
			{
				ObjectPool<char[10], _BOOST pool> pool;
				TestObjectPool(pool);
			}
		}
		TEST_METHOD(WorkerTest)
		{
			Worker obj;
			// 创建socket测试
			obj.NewSocket<Tcp, Shared>();
			obj.NewSocket<Tcp, Unique>();
			obj.NewSocket<Udp, Shared>();
			obj.NewSocket<Udp, Unique>();

			auto tcpPtr = obj.NewSocket<Tcp, Pointer>();
			tcpPtr.Delete();

			auto udpPtr = obj.NewSocket<Udp, Pointer>();
			udpPtr.Delete();

			// 创建buffer测试
			obj.NewBuffer<Vector, Shared>();
			obj.NewBuffer<Vector, Unique>();

			obj.NewBuffer<Array, Shared>();
			obj.NewBuffer<Array, Unique>();

			obj.NewBuffer<Shared>(512);
			obj.NewBuffer<Unique>(512);

			obj.NewBuffer();
			obj.NewBuffer(128);

			auto arraybufferPtr = obj.NewBuffer<Array, Pointer>();
			arraybufferPtr.Delete();

			auto vectorbufferPtr = obj.NewBuffer<Vector, Pointer>();
			vectorbufferPtr.Delete();

			auto tcpSession = obj.NewSession<Tcp>();


		}
		TEST_METHOD(WorkerPoolTest)
		{
			WorkerPool wpool;
			thread t(std::bind(&WorkerPoolFriendProxy::Start, &wpool));
			std::this_thread::sleep_for(std::chrono::seconds(1));
			{
				// 生成future测试
				int num = 123;
				auto future = wpool.Async([](int x, int y)
				{
					char str[] = "0.";
					for (size_t i = 0; i < 5; i++)
					{
						str[0] += 1;
						Logger::WriteMessage(str);
						std::this_thread::sleep_for(std::chrono::milliseconds(200));
					}
					return y * x;
				}, num, 2);
				// 推送task测试
				wpool.PostTask([&]()
				{
					// 做一些多余操作
					auto buff = wpool.GetWorker<WorkerSelector::NextWorker>()->NewBuffer<Vector, Unique>();
					std::string str("Post Task");
					buff->assign(str.begin(), str.end());

					std::string output(buff->begin(), buff->end());

					Logger::WriteMessage(output.c_str());
				});
				// 取当前worker测试
				WorkerPool::TWorker::ArrayPool::TObjectPtr<Shared> buffer;
				std::promise<char> p;
				auto bufferchar = p.get_future();
				wpool.GetWorker<WorkerSelector::NextWorker>()->GetIoService().post([&]()
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(200));
					char c = 'A';
					buffer = wpool.GetWorker<WorkerSelector::CurrentWorker>()->NewBuffer<Array, Shared>();
					(*buffer)[0] = c;
					p.set_value(c);
				});

				Assert::IsTrue(bufferchar.get() == (*buffer)[0]);

				auto result = future.get();
				Assert::IsTrue(num * 2 == result);

			}
			std::this_thread::sleep_for(std::chrono::seconds(3));
			WorkerPoolFriendProxy::Stop(&wpool);
			t.join();
		}
		TEST_METHOD(ServiceManagerTest)
		{
			ServiceManager mng;
			thread t(bind(&ServiceManager::Start, &mng));
			std::this_thread::sleep_for(std::chrono::seconds(1));
			auto x = mng.GetWorkerPool()->GetWorkerCount();
			mng.Stop();
		}
	};

	TEST_CLASS(ServerTest1)
	{

	public:
		ServerTest1()
		{
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

		}

		TEST_METHOD(CoreTest)
		{
		}


	};


}
