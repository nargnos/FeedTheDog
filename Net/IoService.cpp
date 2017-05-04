#include "IoService.h"
#include "Worker.h"
#include "RunAttorney.h"
#include "Loop.h"
// FIX: 电脑只有2核*2超线程；vbox cpu0速度为什么比cpu1的速度快5倍...
// vbox用cpu0可以上1900+, 带cpu1巨慢；还是要在实体机测才行
// 因为这两个核速度不同，测试数据有大幅波动（因为交换accept线程），先暂时设置成单核

// 结构还是不够好，Buffer的处理可改进

// NOTICE: 参数
const float CoreScale = 0.5;
// 尝试按快慢比例分配任务也无法提升，不是a+b的关系而是(a+b)/2的关系


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


IoService::IoService() :
	selectID_(0)
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

const std::unique_ptr<Worker>& IoService::SelectWorker()
{
	do
	{
		std::int_fast16_t old = selectID_.load(std::memory_order_acquire);
		std::int_fast16_t newVal = (old + 1) % workerSize_;
		if (selectID_.compare_exchange_weak(old, newVal, std::memory_order_release))
		{
			return workers_[old];
		}
	} while (true);
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
