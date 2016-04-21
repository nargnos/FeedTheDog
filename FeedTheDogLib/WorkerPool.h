#pragma once
#include "Owner.h"
namespace FeedTheDog
{
	template<typename TWorker>
	class WorkerPool :
		public _BOOST noncopyable
	{
	public:
		typedef _STD vector<unique_ptr<TWorker>> TWorkerVectorType;
		WorkerPool() :
			workerIndex(0)
		{
			unsigned int core = thread::hardware_concurrency()*2;
			threadCount = (core == 0) ? 1 : core;

			for (unsigned int i = 0; i < threadCount; i++)
			{
				workers.push_back(make_unique<TWorker>());
			}
			isStopped = true;
		}

		~WorkerPool()
		{
		}
		void Stop()
		{
			for each (auto& var in workers)
			{
				var->Stop();
			}
			isStopped = true;
		}
		void Start()
		{
			isStopped = false;
			_STD vector<shared_ptr<thread>> threads;
			for (size_t i = 1; i < threadCount; i++)
			{
				threads.push_back(make_shared<thread>(_BOOST bind(&TWorker::Start, workers[i].get())));
			}
			workers[0]->Start();
			// join�ȴ������߳̽���
			for each (auto& var in threads)
			{
				var->join();
			}
		}
		int GetWorkerCount() const
		{
			return threadCount;
		}
		// ȡ����Worker
		TWorker* SelectIdleWorker() const
		{
			if (isStopped)
			{
				// ��coreδ��ʱ���ڷ�ɢ��ʱ��ӵķ���
				return SelectWorker();
			}
			else
			{
				auto it = workers.begin();
				TWorker* result = (it++)->get();
				unsigned int count = result->GetSessionCount();
				auto end = workers.end();
				
				while (count!=0 && it < end)
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
		TWorker* SelectWorker() const
		{
			return workers[GetWorkerIndex()].get();
		}

	private:
		int GetWorkerIndex() const
		{
			int index = workerIndex;
			int nextIndex = 0;
			do
			{
				nextIndex = (index + 1) % threadCount;
			} while (!workerIndex.compare_exchange_weak(index, nextIndex,_STD memory_order_release));
			return index;
		}
		TWorkerVectorType workers;
		mutable _STD atomic<int> workerIndex;
		unsigned int threadCount;
		bool isStopped;
	};
}  // namespace FeedTheDog
