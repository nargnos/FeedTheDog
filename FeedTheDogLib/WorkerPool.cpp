#include "stdafx.h"
#include "WorkerPool.h"
#include "Worker.h"
namespace FeedTheDog
{
	WorkerPool::WorkerPool() :
		workerIndex_(0)
	{
		core_ = thread::hardware_concurrency();
		core_ = (core_ == 0) ? 1 : core_;
		threadCount_ = core_;

		for (unsigned int i = 0; i < threadCount_; i++)
		{
			auto tmpPtr = _BOOST alignment::aligned_alloc(ALIGNSIZE, sizeof(TWorker));
			if (tmpPtr == nullptr)
			{
				throw;
			}
			new (tmpPtr) TWorker();
			workers_.push_back(WorkerPtr(reinterpret_cast<TWorker*>(tmpPtr)));
		}
		isStopped_ = true;
		workersBegin_ = workers_.begin();
		workersEnd_ = workers_.end();
	}

	WorkerPool::~WorkerPool()
	{
	}

	void WorkerPool::Stop()
	{
		for each (auto& var in workers_)
		{
			var->Stop();
		}
		isStopped_ = true;
	}

	void WorkerPool::Start()
	{
		isStopped_ = false;
		_STD vector<shared_ptr<thread>> threads;
		for (size_t i = 0; i < threadCount_; i++)
		{
			auto tmp = make_shared<thread>(
				[worker = workers_[i].get()]()
			{
				worker->Start();
			});
#ifdef _WINDOWS_
			if (core_ > 1)
			{
				auto result = SetThreadAffinityMask((HANDLE)tmp->native_handle(), 1 << (i%core_));
				assert(result);
			}
#else
			// TODO: Linux的线程关联核心
#endif // _WINDOWS_
			threads.push_back(tmp);
		}
		// join等待所有线程结束
		for each (auto& var in threads)
		{
			var->join();
		}
	}

	int WorkerPool::GetWorkerCount() const
	{
		return threadCount_;
	}

	WorkerPool::TWorker * WorkerPool::SelectIdleWorker() const
	{
		TWorker* result = nullptr;
		if (isStopped_)
		{
			// 当core未开时用于分散此时添加的服务
			result = SelectWorker();
		}
		else
		{
			// 大于0是常态，不需要用0比较
			auto it = workersBegin_;
			result = (it++)->get();

			unsigned int count = result->GetSessionCount();

			while (it < workersEnd_)
			{
				auto tmp = it->get();
				auto tmpCount = tmp->GetSessionCount();
				if (count > tmpCount)
				{
					result = tmp;
					count = tmpCount;
				}
				++it;
			}


		}
		assert(result != nullptr);
		return result;
	}
	WorkerPool::TWorker * WorkerPool::SelectWorker() const
	{
		return workers_[GetWorkerIndex()].get();
	}
	int WorkerPool::GetWorkerIndex() const
	{
		int index = workerIndex_;
		int nextIndex = 0;
		do
		{
			nextIndex = (index + 1) % threadCount_;
		} while (!workerIndex_.compare_exchange_weak(index, nextIndex, _STD memory_order_release));
		return index;
	}
}  // namespace FeedTheDog
