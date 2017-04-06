#include "IoService.h"
#include "Worker.h"
#include "RunProxy.h"
#include "Loop.h"
// FIX: ����ֻ��2�ˣ�����û���ԣ�asio���߳� vbox cpu0 1200 cpu1 300+��cpu1��ô����ô��
// ������1core��1thread ������1900+, ����������������������������acc��conn�׺��ԣ�������Ҫ��ʵ��������
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

	// �߳���,����ǰ�洴�����߳�
	workerSize_ = static_cast<size_t>(cores * CoreScale);
	workerSize_ = workerSize_ <= 1 ? 1 : workerSize_ - 1;
	size_t i = 0;
	// ���Ȱѹ����̹߳���cpu0
	for (; i < workerSize_; i++)
	{
		workers_.emplace_back(new Worker(static_cast<unsigned int>(i % cores)));
	}
	SetAffinity(master_->native_handle(), i % cores);
	// ���߳�cpu����
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
