#include "stdafx.h"
#include "WorkerPool.h"
#include "Worker.h"
namespace FeedTheDog
{
	class WorkerFriendProxy
	{
	public:
		friend class WorkerPool;
	private:
		static void Start(Worker* worker)
		{
			worker->Start();
		}
		static void Stop(Worker* worker)
		{
			worker->Stop();
		}
		WorkerFriendProxy() = delete;

		~WorkerFriendProxy() = delete;
	};

	int GetCoreCount()
	{
		static int core = thread::hardware_concurrency();
		return core == 0 ? 1 : core;
	}
	int CalcThreadPoolSize()
	{
		static int halfCores = thread::hardware_concurrency() >> 1;
		return halfCores > 1 ? halfCores : 2;
	}

	WorkerPool::WorkerPool() :
		core_(GetCoreCount()),
		isStopped_(true),
		ioService_(CalcThreadPoolSize() >> 1), // 线程池一半线程用来处理这个
		threadPoolSize_(CalcThreadPoolSize()),
		threadPool_(threadPoolSize_)
	{
		threadCount_ = (core_ == 0) ? 1 : core_;
		threadCount_ = static_cast<int>(threadCount_ * ThreadCountScale);

		for (int i = 0; i < threadCount_; i++)
		{
			auto worker = new (_BOOST alignment::aligned_alloc(ALIGNSIZE, sizeof(TWorker))) TWorker();;
			if (worker == nullptr)
			{
				throw;
			}
			workers_.push_back(WorkerPtr(worker));

			// 实现方式为多io多线程
			auto tmpWorkerThread = make_shared<thread>(
				[worker]()
			{
				WorkerFriendProxy::Start(worker);
			});

			// 设置核心关联
#ifdef _WINDOWS_
			if (core_ > 1)
			{
				auto mask = 1 << (i % core_);
				auto result = SetThreadAffinityMask((HANDLE)tmpWorkerThread->native_handle(), mask);
				assert(result);
			}
#else
			// TODO: Linux的线程关联核心
#endif // _WINDOWS_
			threads_.push_back(tmpWorkerThread);
			// 添加线程-Worker对应表
			threadToWorker_.push_back({ tmpWorkerThread->get_id(),i });
		}
		// 单独线程用来处理一些accept什么的

		auto count = threadPoolSize_ >> 1;
		assert(threadPoolSize_ > 0);
		for (int i = 0; i < count; i++)
		{
			threadPool_.submit([&]()
			{
				_ASIO io_service::work work(ioService_);
				ioService_.run();
			});
		}

		isStopped_ = false;
	}


	WorkerPool::~WorkerPool()
	{

	}

	void WorkerPool::Stop()
	{
		isStopped_ = true;
		ioService_.stop();
		threadPool_.close();
		for each (auto& var in workers_)
		{
			WorkerFriendProxy::Stop(var.get());
		}
	}
	void WorkerPool::Join()
	{
		// join等待所有线程结束
		for each (auto& var in threads_)
		{
			var->join();
		}
		threadPool_.join();
	}

	int WorkerPool::GetWorkerCount() const
	{
		return threadCount_;
	}

	const WorkerPool::WorkerPtr& WorkerPool::SelectWorker() const
	{
		assert(!isStopped_);
		return workers_[GetWorkerIndex()];
	}

	const WorkerPool::WorkerPtr& WorkerPool::SelectCurrentThreadWorker() const
	{
		assert(!isStopped_);
		//auto index = ;
		assert(CurrentWorkerID() != -1);
		static thread_local auto& result = workers_[CurrentWorkerID()];
		return result;
		// return index == -1 ? SelectWorker() : workers_[index];
	}

	int WorkerPool::CurrentWorkerID() const
	{
		static thread_local int result = ThreadIdToWorkerId(get_id());
		return result;
	}
	_ASIO io_service & WorkerPool::GetIoService()
	{
		return ioService_;
	}
	int WorkerPool::ThreadIdToWorkerId(thread::id & tid) const
	{
		// 数组过小直接遍历
		for each (auto& var in threadToWorker_)
		{
			if (var.first == tid)
			{
				return var.second;
			}
		}
		return -1;
	}
	int WorkerPool::GetWorkerIndex() const
	{
		static thread_local int index = 0;
		index = (index + 1) % threadCount_;
		return index;
	}
}  // namespace FeedTheDog
