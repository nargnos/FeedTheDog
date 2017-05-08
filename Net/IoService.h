#ifndef IOSERVICE_H_
#define IOSERVICE_H_
#include <memory>
#include <vector>
#include <atomic>
#include <thread>
#include "Noncopyable.h"
class Worker;
class Loop;
class GetWorkersAttorney;
struct PerfInfo
{
	PerfInfo(size_t count, Loop& idle, Loop& busy, size_t idleCount, size_t busyCount);

	// 最闲的线程的loop结构
	Loop& IdleLoop;
	// 最忙的线程的loop结构
	Loop& BusyLoop;
	// 整个程序的 活动任务数
	size_t TaskCount;
	size_t IdleCount;
	size_t BusyCount;
};
class IoService :
	public Noncopyable
{
public:
	friend GetWorkersAttorney;
	~IoService();
	// 终止后不能重新开始
	void Shutdown();
	static std::shared_ptr<IoService> Instance();
	void Wait();
	size_t WorkerCount() const;
	// FIX: 返回了一些用不到的信息，看之后如果还没用可以考虑去掉
	PerfInfo PerformanceSnapshot();
private:
	IoService();
	const std::vector<std::unique_ptr<Worker>>& Workers() const;
	std::vector<std::unique_ptr<Worker>> workers_;
	size_t workerSize_;
};


#endif // IOSERVICE_H_

