// FtdLibTest.cpp : �������̨Ӧ�ó������ڵ㡣
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

// �������߳�ͬʱ��ȡsession����
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
			// GetNewSessionTime�����ÿ�ζ�ѡ�����߳�ӵ�еĳش���session�������ͻ���ʣ�InsertStorage�Ƕ�ռΪ21%��yieldռ��14%��
			// �ڶ��̵߳����������Ҫѡ��ǰ�̰߳󶨵�worker
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

	// ���Դ�������session���ٶȣ��������֮����������
	cout << "========" << endl << "���̲߳���" << endl;
	cout << "count=" << count << endl;

	cout << "new/delete��ʱ�� " << max << "s" << endl;
	cout << "ÿ���new/delete " << (size_t)(count / max) << "��" << endl;


	// ���ݽ��newsession�����������ܵ�Ӱ��Ӧ���Ǻ�΢С�˰�
	core->Stop();
}
// ���̻߳�ȡsession����
void NewSessionTest(FeedTheDog::ServiceManager*core)
{
	//10000;//
	const size_t count = 6000000;
	auto time = GetNewSessionTime(count, [&core]()->const WorkerPool::WorkerPtr&
	{
		return core->GetWorkerPool()->GetWorker<NextWorker>();
	});
	// ���Դ�������session���ٶȣ��������֮����������
	cout << "========" << endl << "���̲߳���" << endl;
	cout << "count=" << count << endl;
	cout << "new/delete��ʱ�� " << time << "s" << endl;
	// ֮ǰ��λ����
	cout << "ÿ���new/delete " << (size_t)(count / time) << "��" << endl;


	MutilThreadNewSessionTest(core);
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
void AsyncTaskTest(FeedTheDog::ServiceManager&core)
{
	auto future = core.GetWorkerPool()->Async([](int val1, int val2)
	{
		return val1 << val2;
	}, 1, 2);
	std::cout << "����future ";
	std::cout << (future.get() == (1 << 2) ? "�ɹ�" : "ʧ��") << std::endl;
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
	cout << "pool����(Session)" << endl;
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
		cout << "�ο���: " << t << "s" << endl;
	}
	cout << "--------------" << endl;

	// ������ObjectPool��ԭʼ�����̫��
	/*
	pool����(Session) lock
	ObjectPool -> UniquePtr -> pool: 1.085s
	ObjectPool -> Pointer -> object_pool: 1.047s
	ObjectPool -> Pointer -> pool: 0.947s
	object_pool: 0.907s
	pool: 0.678s
	new/delete: 2.963s
	�ο���: 0.723s
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
	cout << "ѡ��Worker�ٶ� " << time.elapsed() << 's' << endl;
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


	_STD cout << "����";
	system("pause");
	return 0;
}

