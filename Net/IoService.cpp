#include "IoService.h"
#include "Worker.h"
#include "RunProxy.h"
#include "Loop.h"
const float CoreScale = 1.;

class StopWorkerProxy
{
public:
	static void Stop(Worker& worker)
	{
		worker.Stop();
	}
private:
	StopWorkerProxy() = delete;
	~StopWorkerProxy() = delete;
};


IoService::IoService() :
	loop_(std::make_unique<Loop>()),
	selectID_(0)
{
	master_ = std::make_unique<std::thread>([this]() {
		RunProxy::Start(*loop_);
	});
	auto cores = std::thread::hardware_concurrency();
	if (cores == 0)
	{
		cores = 1;
	}

	// 线程数,减掉前面创建的线程
	workerSize_ = static_cast<size_t>(cores * CoreScale);
	workerSize_ = workerSize_ <= 1 ? 1 : workerSize_ - 1;
	size_t i = 0;
	// 优先把工作线程关联cpu0
	for (; i < workerSize_; i++)
	{
		workers_.emplace_back(new Worker(static_cast<unsigned int>(i % cores)));
	}
	SetAffinity(master_->native_handle(), i % cores);
	// 主线程cpu关联
	SetAffinity(pthread_self(), i % cores);
}

IoService::~IoService() = default;

void IoService::Shutdown()
{
	RunProxy::Stop(*loop_);

	for (auto& i : workers_)
	{
		StopWorkerProxy::Stop(*i);
	}
}

std::shared_ptr<IoService> IoService::Instance()
{
	static std::shared_ptr<IoService> result(new IoService());
	return result;
}

const std::unique_ptr<Worker>& IoService::SelectWorker() const
{
	auto& result = workers_[selectID_];
	selectID_ = (selectID_ + 1) % workerSize_;
	return result;
}

void IoService::Wait()
{
	master_->join();
	for (auto& i : workers_)
	{
		i->Wait();
	}
}

size_t IoService::WorkerCount() const
{
	return workerSize_;
}
