#ifndef IOSERVICE_H_
#define IOSERVICE_H_
#include <memory>
#include <vector>
#include <atomic>
#include <thread>
#include "Noncopyable.h"
namespace Detail
{
	class Worker;
	class Loop;
	class GetWorkersAttorney;
	struct PerfInfo
	{
		// 最闲的线程的loop结构
		Loop* __restrict IdleLoop;
		// 最忙的线程的loop结构
		Loop* __restrict BusyLoop;
		size_t IdleCount;
		size_t BusyCount;
		// 整个程序的 活动任务数
		size_t TaskCount;
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
		void Join();
		size_t WorkerCount() const;
		// FIX: 返回了一些用不到的信息，看之后如果还没用可以考虑去掉
		// 并不保证结果正确
		const PerfInfo& PerformanceSnapshot();
	private:
		IoService();
		const std::vector<std::unique_ptr<Worker>>& Workers() const;
		std::vector<std::unique_ptr<Worker>> workers_;
		size_t workerSize_;
	};


}  // namespace Detail
using Detail::IoService;
#endif // IOSERVICE_H_

