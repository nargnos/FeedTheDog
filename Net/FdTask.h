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
	// �����post�������̰߳�ȫ��
	// ���������������߳�ִ�лص�
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

	// ����
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
		// FIX: ���epoll�Ǳ������⣬���������������
		FDTaskCtlAttorney::Add(loop, EPOLLIN | EPOLLONESHOT, task);
	}
	// �ᷢ�������õ�tls�����ֱ���ܵõ����������
	// ���ֻ��Ҫ������þ���posttask�ȽϿ�
	// void(Loop&)
	template<typename TCb>
	void PostFdTask(TCb&& cb)
	{
		assert(GlobalTlsPackage::Instance().TlsLoop != nullptr);
		auto task = new FdTask<TCb>(std::forward<TCb>(cb));
		// FIX: ���epoll�Ǳ������⣬���������������
		FDTaskCtlAttorney::Add(*GlobalTlsPackage::Instance().TlsLoop, EPOLLIN | EPOLLONESHOT, task);
	}
	// �����ѡ���е�post
	// void(Loop&)
	template<typename TCb>
	void PostFdTaskIdle(TCb&& cb)
	{
		auto task = new FdTask<TCb>(std::forward<TCb>(cb));
		// FIX: ���epoll�Ǳ������⣬���������������
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
			// ���ﲻ���ڶ��߳���
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
		// FIX: ���epoll�Ǳ������⣬���������������
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

