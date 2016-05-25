// FtdLibTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <ServiceManager.h>
using namespace FeedTheDog;

using std::cout;
using std::endl;


//namespace FeedTheDog
//{
//	class WorkerFriendProxy
//	{
//	public:
//		template<typename TProtocol, typename TWorker, typename TSession = TWorker::TSession<TProtocol, false>>
//		static const unique_ptr<typename TWorker::template TSessionPool<TSession>>&
//			GetSessionPool(TWorker* worker)
//		{
//			return worker->GetSessionPool<TSession>();
//		}
//	private:
//		WorkerFriendProxy() = delete;
//
//		~WorkerFriendProxy() = delete;
//
//	};
//}  // namespace FeedTheDog

template<typename TGetWorker>
double GetNewSessionTime(size_t count, TGetWorker&& getWorker)
{
	//std::list<Worker::TcpSocketPool::TResult<Unique>> list;
	boost::timer timer;

	for (size_t i = 0; i < count; i++)
	{
		getWorker()->NewSession<Tcp>();
		//getWorker()->NewSocket<Tcp, Unique>();
		//list.emplace_back();
	}
	/*while (!list.empty())
	{
		list.pop_back();
	}*/
	return timer.elapsed();
}

// 核心数线程同时获取session速率
void MutilThreadNewSessionTest(FeedTheDog::ServiceManager*core)
{
	// 100000;//
	size_t count = 6000000;
	auto workerCount = core->GetWorkerPool()->GetWorkerCount();

	size_t avgCount = count / workerCount;
	count = avgCount*workerCount;

	std::vector<std::future<double>> futureVector;

	for (size_t i = 0; i < workerCount; i++)
	{
		shared_ptr<std::promise<double>> tmpPromise = make_shared<std::promise<double>>();
		futureVector.push_back(tmpPromise->get_future());

		core->GetWorkerPool()->GetWorker<NextWorker>()->GetIoService().post([&, p = std::move(tmpPromise)]() mutable
		{
			// GetNewSessionTime中如果每次都选其它线程拥有的池创建session会增大冲突概率（InsertStorage非独占为21%，yield占了14%）
			// 在多线程的这种情况需要选择当前线程绑定的worker
			p->set_value(GetNewSessionTime(avgCount, [&core]()->const WorkerPool::WorkerPtr&
			{
				return core->GetWorkerPool()->GetWorker<CurrentWorker>();
			}));
		});
	}

	unique_ptr<double[]> results = make_unique<double[]>(workerCount);
	for (size_t i = 0; i < workerCount; i++)
	{
		results[i] = futureVector[i].get();
	}
	auto max = *std::max_element(results.get(), results.get() + workerCount);

	// 测试创建析构session的速度，这里分配之后立马析构
	cout << "========" << endl << "多线程测试" << endl;
	cout << "count=" << count << endl;

	cout << "new/delete总时间 " << max << "s" << endl;
	cout << "每秒可new/delete " << (size_t)(count / max) << "次" << endl;


	// 根据结果newsession对于总体性能的影响应该是很微小了吧
	core->Stop();
}
// 单线程获取session速率
void NewSessionTest(FeedTheDog::ServiceManager*core)
{
	//10000;//
	const size_t count = 6000000;
	auto time = GetNewSessionTime(count, [&core]()->const WorkerPool::WorkerPtr&
	{
		return core->GetWorkerPool()->GetWorker<NextWorker>();
	});
	// 测试创建析构session的速度，这里分配之后立马析构
	cout << "========" << endl << "单线程测试" << endl;
	cout << "count=" << count << endl;
	cout << "new/delete总时间 " << time << "s" << endl;
	// 之前单位有误
	cout << "每秒可new/delete " << (size_t)(count / time) << "次" << endl;


	MutilThreadNewSessionTest(core);
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
void AsyncTaskTest(FeedTheDog::ServiceManager&core)
{
	auto future = core.GetWorkerPool()->Async([](int val1, int val2)
	{
		return val1 << val2;
	}, 1, 2);
	std::cout << "测试future ";
	std::cout << (future.get() == (1 << 2) ? "成功" : "失败") << std::endl;
}

template<typename TPool>
double GetPoolSpeed(TPool& pool, size_t count)
{
	_ASIO io_service ios;
	boost::timer time;
	for (size_t i = 0; i < count; i++)
	{
		pool.Construct<Pointer>(ios).Delete();
	}
	return time.elapsed();
}

void ObjectPoolSpeedTest()
{
	cout << "pool测试(Session)" << endl;
	size_t count = 6000000;// 100000000;
	double t;

//#define NOLOCK
#ifdef NOLOCK
#define LOCK ,_BOOST details::pool::null_mutex
#else
#define LOCK
#endif // NOLOCK
	{
		_ASIO io_service ios;
		ObjectPool<Session<Tcp>, _BOOST pool LOCK> pool;
		boost::timer time;
		for (size_t i = 0; i < count; i++)
		{
			pool.Construct<Unique>(ios);
		}
		t = time.elapsed();
		cout << "ObjectPool -> UniquePtr -> pool: " << t << "s" << endl;
	}

	{
		ObjectPool<Session<Tcp>, _BOOST object_pool LOCK> pool;
		t = GetPoolSpeed(pool, count);
		cout << "ObjectPool -> Pointer -> object_pool: " << t << "s" << endl;

	}

	{
		ObjectPool<Session<Tcp>, _BOOST pool LOCK> pool;
		t = GetPoolSpeed(pool, count);
		cout << "ObjectPool -> Pointer -> pool: " << t << "s" << endl;
		
	}

	{
		_ASIO io_service ios;
		_BOOST object_pool<Session<Tcp>> pool;
		boost::timer time;
		for (size_t i = 0; i < count; i++)
		{
			pool.destroy(pool.construct<_ASIO io_service&>(ios));
		}
		t = time.elapsed();
		cout << "object_pool: " << t << "s" << endl;
	}
	{

		_ASIO io_service ios;
		_BOOST pool<> pool(sizeof(Session<Tcp>));
		boost::timer time;
		for (size_t i = 0; i < count; i++)
		{
			auto tmp = new (pool.malloc()) Session<Tcp>(ios);
			tmp->~Session<Tcp>();
			pool.free(tmp);
		}
		t = time.elapsed();
		cout << "pool: " << t << "s" << endl;
	}
	{
		_ASIO io_service ios;
		boost::timer time;
		for (size_t i = 0; i < count; i++)
		{
			auto tmp = new Session<Tcp>(ios);
			delete tmp;
		}
		t = time.elapsed();
		cout << "new/delete: " << t << "s" << endl;
	}
	{
		_ASIO io_service ios;
		char buff[sizeof(Session<Tcp>)];
		boost::timer time;
		for (size_t i = 0; i < count; i++)
		{
			auto tmp=new (buff) Session<Tcp>(ios);
			tmp->~Session<Tcp>();
		}
		t = time.elapsed();
		cout << "参考组: " << t << "s" << endl;
	}
	cout << "--------------" << endl;

	// 不能让ObjectPool比原始池相差太多
	/*
	pool测试(Session) lock
	ObjectPool -> UniquePtr -> pool: 1.085s
	ObjectPool -> Pointer -> object_pool: 1.047s
	ObjectPool -> Pointer -> pool: 0.947s
	object_pool: 0.907s
	pool: 0.678s
	new/delete: 2.963s
	参考组: 0.723s
	*/
	
}
void TestSelectWorkerSpeed(WorkerPool& pool)
{
	int result = 0;
	boost::timer time;
	for (size_t i = 0; i < 1000000000; i++)
	{
		pool.CurrentWorkerID();
	}
	cout << "选择Worker速度 " << time.elapsed() << 's' << endl;
}
int main()
{
	ObjectPoolSpeedTest();
	//return 0;
	{
		FeedTheDog::ServiceManager core;
		AsyncTaskTest(core);
		// TestSelectWorkerSpeed(*core.GetWorkerPool());
		core.GetWorkerPool()->PostTask(_BOOST bind(&NewSessionTest, &core));

		std::thread t([&core]()
		{
			core.Start();
		});
		t.join();
	}

	//TestLog(os, core.GetTrace());
	//Logger::WriteMessage(os.str().c_str());


	_STD cout << "结束";
	system("pause");
	return 0;
}

