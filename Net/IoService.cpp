#include "IoService.h"
#include <algorithm>
#include "Worker.h"
#include "RunAttorney.h"
#include "Loop.h"
#include "GetLoopAttorney.h"
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
	assert(workerSize_ > 0);
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

void IoService::Wait()
{
	for (auto& i : workers_)
	{
		i->Wait();
	}
}

size_t IoService::WorkerCount() const
{
	return workerSize_;
}

PerfInfo IoService::PerformanceSnapshot()
{
	// 并不需要准确
	int sum = 0;
	auto it = workers_.begin();
	Loop* idle = &GetLoopAttorney::GetLoop(**it);
	Loop* busy = idle;
	int minCount = idle->TaskCount();
	int maxCount = minCount;

	++it;
	auto end = workers_.end();
	for (; it < end; ++it)
	{
		auto& tmpLoop = GetLoopAttorney::GetLoop(**it);
		auto tmpCount = tmpLoop.TaskCount();
		sum += tmpCount;

		if (tmpCount < minCount)
		{
			minCount = tmpCount;
			idle = &tmpLoop;
		}
		else if (tmpCount > maxCount)
		{
			maxCount = tmpCount;
			busy = &tmpLoop;
		}
	}
	return PerfInfo(sum, *idle, *busy, minCount, maxCount);
}

PerfInfo::PerfInfo(size_t count, Loop & idle, Loop & busy, size_t idleCount, size_t busyCount) :
	IdleLoop(idle),
	BusyLoop(busy),
	TaskCount(count),
	IdleCount(idleCount),
	BusyCount(busyCount)
{

}
