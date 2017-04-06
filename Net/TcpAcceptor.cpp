#include "TcpAcceptor.h"
#include <boost/lockfree/queue.hpp>
#include "Loop.h"
#include "IoService.h"
#include "Worker.h"
#include "AcceptorRegister.h"
#include "TcpConnection.h"
#include "FDTaskCtlProxy.h"
#include "Task.h"
TcpAcceptor::TcpAcceptor(const std::shared_ptr<IoService>& ios,
	const sockaddr_in& bind) :
	ios_(ios),
	isCanceled_(false)
{
	assert(ios);
	if (!socket_)
	{
		TRACEERRNOEXITSTR(LogPriority::Emerg, "Create Socket Faild");
	}
	if (!socket_.SetNonBlocking())
	{
		TRACEPOINT(LogPriority::Emerg)("Set NonBlocking Faild");
		_EXIT;
	}
	if (!socket_.SetReusePort(true))
	{
		TRACEPOINT(LogPriority::Notice)("Set Reuse Port Faild");
	}
	if (!socket_.Bind(bind))
	{
		TRACEERRNOEXITSTR(LogPriority::Emerg, "Bind Socket Faild");
	}
	if (!socket_.Listen())
	{
		TRACEERRNOEXITSTR(LogPriority::Emerg, "Listen Socket Faild");
	}
	AcceptorRegister::Register(*ios, *this);
}

void TcpAcceptor::DoAccept(Loop & loop, int fd)
{
	assert(fd != -1);
	// 不处理（维持connect生存期，执行rw）将会关闭并丢弃连接
	auto sock = TcpConnection::Create(loop, fd);
	assert(onAccept_);
	onAccept_(sock.get());
}
// 这里如果出错先析构会导致doaccept引用错误
void TcpAcceptor::DoEvent(Loop & loop, EpollOption op)
{
	if (op.Flags.Err)
	{
		OnFaild();
		assert(false);
		return;
	}
	assert(op.Flags.In);

	auto& ios = *ios_;


	size_t index = 0;
	// 接够个数发送完再接
	do
	{
		auto& item = accepts_[index];
		item = socket_.Accept();
		if (item != -1)
		{
			++index;
		}
		else
		{
			auto err = errno;
			if (err == EAGAIN || err == EWOULDBLOCK || err == EINTR)
			{
				break;
			}
			else
			{
				TRACEERRNOEXITSTR(LogPriority::Emerg, "Accept Faild");
			}
		}
	}while (index < accepts_.size());
	if (index == 0)
	{
		return;
	}
	// 此处index为个数
	auto& count = index;
	auto workerCount = ios.WorkerCount();
	auto avg = count <= workerCount ? 1 : count / workerCount;
	auto cur = accepts_.begin();
	auto end = cur + count;
	while (true)
	{
		ios.SelectWorker()->PostTcpFd(cur, avg, this);

		cur += avg;
		auto dt = std::distance(cur, end);
		if (dt <= 0)
		{
			break;
		}
		else if (dt < avg)
		{
			avg = dt;
		}
	}
}

int TcpAcceptor::FD() const
{
	return socket_.FD();
}

void TcpAcceptor::OnFaild()
{
	if (onFaild_)
	{
		onFaild_();
	}
	TRACEPOINT_LINE(LogPriority::Debug);
	TRACEERRNOEXIT(LogPriority::Emerg);
}
// 有可能在其它线程关闭
void TcpAcceptor::Cancel()
{
	if (!isCanceled_.test_and_set(std::memory_order_acquire))
	{
		AcceptorRegister::UnRegister(*ios_, *this);
		socket_.Close();
		Detach();
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

void TcpAcceptor::SetOnFaild(FaildHandler&& h)
{
	assert(!onFaild_);
	onFaild_ = std::move(h);
}

std::shared_ptr<TcpAcceptor> TcpAcceptor::Create(const std::shared_ptr<IoService>& ios,
	const sockaddr_in& bind)
{
	std::shared_ptr<TcpAcceptor> result(new TcpAcceptor(ios, bind));
	result->SetPos(Store::Attach(result));
	return result;
}

