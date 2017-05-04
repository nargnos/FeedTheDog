#include "IoService.h"
#include "Worker.h"
#include "RunAttorney.h"
#include "Loop.h"
// FIX: ����ֻ��2��*2���̣߳�vbox cpu0�ٶ�Ϊʲô��cpu1���ٶȿ�5��...
// vbox��cpu0������1900+, ��cpu1����������Ҫ��ʵ��������
// ��Ϊ���������ٶȲ�ͬ�����������д����������Ϊ����accept�̣߳�������ʱ���óɵ���

// �ṹ���ǲ����ã�Buffer�Ĵ���ɸĽ�

// NOTICE: ����
const float CoreScale = 0.5;
// ���԰�����������������Ҳ�޷�����������a+b�Ĺ�ϵ����(a+b)/2�Ĺ�ϵ


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
	// ���Ȱѹ����̹߳���cpu0
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
