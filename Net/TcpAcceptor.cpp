#include "TcpAcceptor.h"
#include <array>
#include "Loop.h"
#include "IoService.h"
#include "Worker.h"
#include "FDTaskCtlAttorney.h"
#include "Task.h"
#include "GetLoopAttorney.h"
#include "TcpAttachAttorney.h"
#include "GetWorkersAttorney.h"
TcpAcceptor::TcpAcceptor(const std::shared_ptr<IoService>& ios,
	const sockaddr_in& bind) :
	ios_(ios),
	isCanceled_(false),
	needReregister_(false)
{
	assert(ios);
	assert(ios->WorkerCount() > 0);
	if (!socket_)
	{
		TRACEERRNOEXITSTR(LogPriority::Emerg, "Create Socket Failed");
	}
	if (!socket_.SetNonBlocking())
	{
		TRACEPOINT(LogPriority::Emerg)("Set NonBlocking Failed");
		_EXIT;
	}
	if (!socket_.SetReusePort(true))
	{
		TRACEPOINT(LogPriority::Notice)("Set Reuse Port Failed");
	}
	if (!socket_.Bind(bind))
	{
		TRACEERRNOEXITSTR(LogPriority::Emerg, "Bind Socket Failed");
	}
	if (!socket_.Listen())
	{
		TRACEERRNOEXITSTR(LogPriority::Emerg, "Listen Socket Failed");
	}
	needReregister_ = ios->WorkerCount() > 1;
	RegListen();
}

void TcpAcceptor::RegListen()
{
	// 选择worker注册
	auto& workers = GetWorkersAttorney::Workers(*ios_);
	auto& worker = *workers.front();
	auto& newLoop = GetLoopAttorney::GetLoop(worker);
	RegListen(newLoop);

}
void TcpAcceptor::RegListen(Loop& loop)
{
	FDTaskCtlAttorney::Add(loop, EPOLLIN, this);
}
void TcpAcceptor::UnRegListen(Loop& loop)
{
	// 只在自身线程使用
	FDTaskCtlAttorney::Del(loop, this);
}


void TcpAcceptor::DoAccept(Loop & loop, int fd)
{
	assert(fd != -1);
	// 不处理（维持connect生存期，执行rw）将会关闭并丢弃连接
	auto sock = TcpAttachAttorney::Attach(loop, fd);
	// 此时如果在设置cb的间隙就有连接过来则丢弃
	if (onAccept_)
	{
		onAccept_(sock.get());
	}
}

void TcpAcceptor::DoEvent(Loop & loop, EpollOption op)
{
	if (op.Flags.Err)
	{
		UnRegListen(loop);
		OnFailed();
		assert(false);
		return;
	}
	assert(op.Flags.In);
	size_t i = 0;
	do
	{
		auto fd = socket_.Accept();
		if (__glibc_likely(fd != -1))
		{
			++i;
			DoAccept(loop, fd);
		}
		else
		{
			auto err = errno;
			if (__glibc_likely(err == EAGAIN || err == EWOULDBLOCK || err == EINTR))
			{
				break;
			}
			else
			{
				TRACEERRNOEXITSTR(LogPriority::Emerg, "Accept Failed");
			}
		}
	} while (i < SOMAXCONN);
	if (!needReregister_)
	{
		return;
	}
	// 取总数，如果忙就找个最小的注册
	// 并不需要准确

	auto perf = ios_->PerformanceSnapshot();

	if (&perf.BusyLoop == &loop && IsTooBusy(perf.BusyCount, perf.TaskCount))
	{
		// 过忙，切换接收权
		// FIX: 这里是否会产生交接频繁问题，如何均衡又减少交接次数？
		// TODO: 如果跟踪loop执行前后的任务数，可以得到增长或者减少的速率，这个可能有用
		UnRegListen(loop);
		RegListen(perf.IdleLoop);
	}
}

bool TcpAcceptor::IsTooBusy(int selfCount, int sum)
{
	// NOTICE: 参数: 忙判定
	constexpr int rsh = 3;
	return selfCount >= (sum - (sum >> rsh));
}


int TcpAcceptor::FD() const
{
	return socket_.FD();
}

void TcpAcceptor::OnFailed()
{
	if (onFailed_)
	{
		onFailed_();
	}
	TRACEPOINT_LINE(LogPriority::Debug);
	TRACEERRNOEXIT(LogPriority::Emerg);
}


// 有可能在其它线程关闭
void TcpAcceptor::Cancel()
{
	auto f = false;
	if (isCanceled_.compare_exchange_strong(f, true, std::memory_order_release))
	{
		// TODO: 这里有些问题
		//UnRegListen();
		socket_.Close();
	}
}

TcpAcceptor::~TcpAcceptor()
{
	Cancel();
}

void TcpAcceptor::SetOnAccept(AcceptHandler && h)
{
	assert(!onAccept_);
	onAccept_ = std::move(h);
}

void TcpAcceptor::SetOnFailed(FailedHandler&& h)
{
	assert(!onFailed_);
	onFailed_ = std::move(h);
}

std::shared_ptr<TcpAcceptor> TcpAcceptor::Create(const std::shared_ptr<IoService>& ios,
	const sockaddr_in& bind)
{
	std::shared_ptr<TcpAcceptor> result(new TcpAcceptor(ios, bind));
	return result;
}

