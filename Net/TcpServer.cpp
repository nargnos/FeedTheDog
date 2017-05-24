#include "TcpServer.h"
#include <array>
#include "Loop.h"
#include "IoService.h"
#include "Worker.h"
#include "FDTaskCtlAttorney.h"
#include "Task.h"
#include "GetLoopAttorney.h"
#include "GetWorkersAttorney.h"
namespace Detail
{
	int TcpServerBase::FD() const
	{
		return socket_.FD();
	}

	// 有可能在其它线程关闭

	void TcpServerBase::Cancel()
	{
		auto f = false;
		if (isCanceled_.compare_exchange_strong(f, true, std::memory_order_release))
		{
			// TODO: 这里有些问题
			//UnRegListen();
			socket_.Close();
		}
	}
	TcpServerBase::~TcpServerBase()
	{
		Cancel();
	}
	TcpServerBase::TcpServerBase(const sockaddr_in & bind) :
		ios_(IoService::Instance()),
		isCanceled_(false),
		needReregister_(false)
	{
		assert(ios_);
		assert(ios_->WorkerCount() > 0);
		if (!socket_)
		{
			TRACEERRNOEXITSTR(LogPriority::Emerg, "Create Socket Failed");
			return;
		}
		if (!socket_.SetNonBlocking())
		{
			TRACEERRNOEXITSTR(LogPriority::Emerg, "Set NonBlocking Failed");
			return;
		}
		if (!socket_.SetReusePort(true))
		{
			TRACEPOINT(LogPriority::Notice)("Set Reuse Port Failed");
			return;
		}
		if (!socket_.Bind(bind))
		{
			TRACEERRNOEXITSTR(LogPriority::Emerg, "Bind Socket Failed");
			return;
		}
		if (!socket_.Listen())
		{
			TRACEERRNOEXITSTR(LogPriority::Emerg, "Listen Socket Failed");
			return;
		}
		needReregister_ = ios_->WorkerCount() > 1;
		RegListen();
	}
	void TcpServerBase::RegListen()
	{
		// 选择worker注册
		auto& worker = GetWorkersAttorney::FirstWorker(*ios_);
		auto& newLoop = GetLoopAttorney::GetLoop(*worker);
		RegListen(newLoop);
	}
	void TcpServerBase::RegListen(Loop & loop)
	{
		FDTaskCtlAttorney::Add(loop, EPOLLIN, this);
	}
	void TcpServerBase::UnRegListen(Loop & loop)
	{
		// 只在自身线程使用
		FDTaskCtlAttorney::Del(loop, this);
	}
	void TcpServerBase::Balance(Loop & loop)
	{
		// NOTICE: 参数: 堆积数超过这个就考虑交出权利
		// 数值越大波动越大，到某个值越小越慢，门槛过高会让各cpu不平均	
		constexpr int limit = 8;
		if (!needReregister_ || loop.TaskCount() < limit)
		{
			return;
		}
		// 取总数，如果忙就找个最小的注册
		// 并不需要准确
		// TODO: 切换的消耗应该能被抵消掉
		auto& perf = ios_->PerformanceSnapshot();
		// TODO: 算法不太对，有点不平均；等完成所有再处理这里
		if (perf.IdleCount == 0 || (perf.BusyLoop == &loop && IsTooBusy(perf.BusyCount, perf.TaskCount)))
		{
			TRACEPOINT(LogPriority::Debug)("Switch! self: %d  max: %d  idle: %d",
				perf.BusyCount, perf.TaskCount, perf.IdleCount);
			// 过忙，切换接收权
			// FIX: 这里是否会产生交接频繁问题，如何均衡又减少交接次数？
			// TODO: 如果跟踪loop执行前后的任务数，可以得到增长或者减少的速率，这个可能有用
			UnRegListen(loop);
			RegListen(*perf.IdleLoop);
		}
	}
	bool TcpServerBase::IsTooBusy(size_t selfCount, size_t sum)
	{
		// NOTICE: 参数: 忙判定
		constexpr int rsh = 3;
		return selfCount >= (sum - (sum >> rsh));
	}
	void TcpServerBase::Join() const
	{
		ios_->Join();
	}
}  // namespace Detail
