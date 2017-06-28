#ifndef FDTASK_H_
#define FDTASK_H_
#include <atomic>
#include <cassert>
#include "IFDTask.h"
#include "EventFd.h"
#include "FDTaskCtlAttorney.h"
#include "Task.h"
#include "IoService.h"
#include "GetWorkersAttorney.h"
#include "GetLoopAttorney.h"
#include "TlsPackage.h"
namespace Detail
{
	// 这里的post操作是线程安全的
	// 用作可以让其它线程执行回调
	class Loop;
	template<typename TCb>
	class FdTaskBase :
		public TaskBase<TCb>,
		public IFDTask
	{
	public:
		explicit FdTaskBase(TCb&& cb) :
			TaskBase<TCb>(std::forward<TCb>(cb)),
			fd_(1, EventFdFlags::CloseOnExec)
		{
		}
	protected:
		virtual int FD() const override
		{
			return fd_.FD();
		}
	private:
		EventFd fd_;
	};

	// 单体
	template<typename TCb>
	class FdTask :
		public FdTaskBase<TCb>
	{
	public:
		using FdTaskBase<TCb>::FdTaskBase;

	private:
		virtual void DoEvent(Loop & loop, EpollOption op) override
		{
			this->Invoke(loop);
			delete this;
		}
	};

	// void(Loop&)
	template<typename TCb>
	void PostFdTask(Loop& loop, TCb&& cb)
	{
		auto task = new FdTask<TCb>(std::forward<TCb>(cb));
		// FIX: 如果epoll那边有问题，这里会有析构隐患
		FDTaskCtlAttorney::Add(loop, EPOLLIN | EPOLLONESHOT, task);
	}
	// 会发给自身，用的tls，如果直接能得到就用上面的
	// 如果只是要自身调用就用posttask比较快
	// void(Loop&)
	template<typename TCb>
	void PostFdTask(TCb&& cb)
	{
		assert(GlobalTlsPackage::Instance().TlsLoop != nullptr);
		auto task = new FdTask<TCb>(std::forward<TCb>(cb));
		// FIX: 如果epoll那边有问题，这里会有析构隐患
		FDTaskCtlAttorney::Add(*GlobalTlsPackage::Instance().TlsLoop, EPOLLIN | EPOLLONESHOT, task);
	}
	// 这个会选空闲的post
	// void(Loop&)
	template<typename TCb>
	void PostFdTaskIdle(TCb&& cb)
	{
		auto task = new FdTask<TCb>(std::forward<TCb>(cb));
		// FIX: 如果epoll那边有问题，这里会有析构隐患
		FDTaskCtlAttorney::Add(*IoService::Instance()->PerformanceSnapshot().IdleLoop,
			EPOLLIN | EPOLLONESHOT, task);
	}
	// aoe
	template<typename TCb>
	class FdTaskAoe :
		public FdTaskBase<TCb>
	{
	public:
		FdTaskAoe(TCb&& cb) :
			FdTaskBase<TCb>(std::forward<TCb>(cb)),
			count_(0)
		{

		}
		void AddCount()
		{
			// 这里不会在多线程用
			count_.fetch_add(1, std::memory_order_relaxed);
		}
		void DecCount()
		{
			assert(count_.load(std::memory_order_acquire) > 0);
			if (count_.fetch_sub(1, std::memory_order_release) == 0)
			{
				delete this;
			}
		}
	private:

		virtual void DoEvent(Loop & loop, EpollOption op) override
		{
			this->Invoke(loop);
			DecCount();
		}
		std::atomic<uint_fast8_t> count_;
	};
	// void(Loop&)
	template<typename TCb>
	void PostFdTaskAoe(TCb&& cb)
	{
		auto task = new FdTaskAoe<TCb>(std::forward<TCb>(cb));
		// FIX: 如果epoll那边有问题，这里会有析构隐患
		auto& workers = GetWorkersAttorney::Workers(*IoService::Instance());
		for (auto& i : workers)
		{
			auto& loop = GetLoopAttorney::GetLoop(*i);
			task->AddCount();
			FDTaskCtlAttorney::Add(loop, EPOLLIN | EPOLLONESHOT, task);
		}
	}


}  // namespace Detail
using Detail::PostFdTask;
using Detail::PostFdTaskIdle;
using Detail::PostFdTaskAoe;
#endif // FDTASK_H_

