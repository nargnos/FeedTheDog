#include "IoService.h"
#include <algorithm>
#include "Worker.h"
#include "RunAttorney.h"
#include "Loop.h"
#include "GetLoopAttorney.h"
namespace Detail
{
	// 结构还是不够好，Buffer的处理可改进

	// NOTICE: 参数: 线程数与核心比例
	const float CoreScale = 1.;

	class StopWorkerAttorney
	{
	public:
		static void Stop(Worker& worker)
		{
			worker.Stop();
		}
	private:
		StopWorkerAttorney() = delete;
		~StopWorkerAttorney() = delete;
	};


	IoService::IoService()
	{
		auto cores = std::thread::hardware_concurrency();
		if (cores == 0)
		{
			cores = 1;
		}


		workerSize_ = static_cast<size_t>(cores * CoreScale);
		if (workerSize_ == 0)
		{
			workerSize_ = 1;
		}
		size_t i = 0;
		// 优先把工作线程关联cpu0
		for (; i < workerSize_; i++)
		{
			workers_.emplace_back(new Worker(static_cast<unsigned int>(i % cores)));
		}
	}

	const std::vector<std::unique_ptr<Worker>>& IoService::Workers()const
	{
		return workers_;
	}

	IoService::~IoService() = default;

	void IoService::Shutdown()
	{
		for (auto& i : workers_)
		{
			StopWorkerAttorney::Stop(*i);
		}
	}

	std::shared_ptr<IoService> IoService::Instance()
	{
		static std::shared_ptr<IoService> result(new IoService());
		return result;
	}

	void IoService::Join()
	{
		for (auto& i : workers_)
		{
			i->Join();
		}
	}

	size_t IoService::WorkerCount() const
	{
		return workerSize_;
	}

	const PerfInfo& IoService::PerformanceSnapshot()
	{
		static thread_local PerfInfo result;
		// 并不需要准确
		result.TaskCount = 0;
		auto it = workers_.begin();
		result.IdleLoop = &GetLoopAttorney::GetLoop(**it);
		result.BusyLoop = result.IdleLoop;
		result.IdleCount = result.IdleLoop->TaskCount();
		result.BusyCount = result.IdleCount;

		++it;
		auto end = workers_.end();
		for (; it < end; ++it)
		{
			auto& tmpLoop = GetLoopAttorney::GetLoop(**it);
			auto tmpCount = tmpLoop.TaskCount();
			result.TaskCount += tmpCount;

			if (tmpCount < result.IdleCount)
			{
				result.IdleCount = tmpCount;
				result.IdleLoop = &tmpLoop;
			}
			else if (tmpCount > result.BusyCount)
			{
				result.BusyCount = tmpCount;
				result.BusyLoop = &tmpLoop;
			}
		}
		return result;
	}


}  // namespace Detail