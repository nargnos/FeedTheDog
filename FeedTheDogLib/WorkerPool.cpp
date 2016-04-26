#include "stdafx.h"
#include "WorkerPool.h"
#include "Worker.h"
namespace FeedTheDog
{
	WorkerPool::WorkerPool() :
		workerIndex(0)
	{
		unsigned int core = thread::hardware_concurrency() * 2;
		threadCount = (core == 0) ? 1 : core;

		for (unsigned int i = 0; i < threadCount; i++)
		{
			workers.push_back(make_unique<TWorker>());
		}
		isStopped = true;
	}

	WorkerPool::~WorkerPool()
	{
	}

	void WorkerPool::Stop()
	{
		for each (auto& var in workers)
		{
			var->Stop();
		}
		isStopped = true;
	}

	void WorkerPool::Start()
	{
		isStopped = false;
		_STD vector<shared_ptr<thread>> threads;
		for (size_t i = 1; i < threadCount; i++)
		{
			threads.push_back(make_shared<thread>(
				[worker = workers[i].get()]()
			{
				worker->Start();
			}));
		}
		workers[0]->Start();
		// join等待所有线程结束
		for each (auto& var in threads)
		{
			var->join();
		}
	}

	int WorkerPool::GetWorkerCount() const
	{
		return threadCount;
	}

	WorkerPool::TWorker * WorkerPool::SelectIdleWorker() const
	{
		if (isStopped)
		{
			// 当core未开时用于分散此时添加的服务
			return SelectWorker();
		}
		else
		{
			auto it = workers.begin();
			TWorker* result = (it++)->get();
			unsigned int count = result->GetSessionCount();
			auto end = workers.end();

			while (count != 0 && it < end)
			{
				auto tmp = it->get();
				auto tmpCount = tmp->GetSessionCount();
				if (count > tmpCount)
				{
					count = tmpCount;
					result = tmp;
				}
				++it;
			}
			return result;
		}
	}
	WorkerPool::TWorker * WorkerPool::SelectWorker() const
	{
		return workers[GetWorkerIndex()].get();
	}
	int WorkerPool::GetWorkerIndex() const
	{
		int index = workerIndex;
		int nextIndex = 0;
		do
		{
			nextIndex = (index + 1) % threadCount;
		} while (!workerIndex.compare_exchange_weak(index, nextIndex, _STD memory_order_release));
		return index;
	}
}  // namespace FeedTheDog
