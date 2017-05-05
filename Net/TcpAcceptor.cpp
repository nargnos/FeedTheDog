#include "TcpAcceptor.h"
#include <array>
#include "Loop.h"
#include "IoService.h"
#include "Worker.h"
#include "FDTaskCtlAttorney.h"
#include "Task.h"
#include "GetLoopAttorney.h"
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
	needReregister_ = ios->WorkerCount() > 1;
	RegListen();
}

void TcpAcceptor::RegListen()
{
	// ѡ��workerע��
	auto& newLoop = GetLoopAttorney::GetLoop(*GetWorkersAttorney::Workers(*ios_).front());
	RegListen(newLoop);

}
void TcpAcceptor::RegListen(Loop& loop)
{
	FDTaskCtlAttorney::Add(loop, EPOLLIN, this);
}
void TcpAcceptor::UnRegListen(Loop& loop)
{
	// ֻ�������߳�ʹ��
	FDTaskCtlAttorney::Del(loop, this);
}


void TcpAcceptor::DoAccept(Loop & loop, int fd)
{
	assert(fd != -1);
	// ������ά��connect�����ڣ�ִ��rw������رղ���������
	auto sock = TcpConnection::Attach(loop, fd);
	// ��ʱ���������cb�ļ�϶�������ӹ�������
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
		OnFaild();
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
				TRACEERRNOEXITSTR(LogPriority::Emerg, "Accept Faild");
			}
		}
	} while (i < SOMAXCONN);
	if (!needReregister_)
	{
		return;
	}
	// ȡ���������æ���Ҹ���С��ע��
	// ������Ҫ׼ȷ
	auto selfCount = loop.TaskCount();
	int sum = 0;
	int minCount = INT32_MAX;
	Loop* minLoop = nullptr;
	auto& workers = GetWorkers();
	for (auto& item : workers)
	{
		auto& tmpLoop = GetLoopAttorney::GetLoop(*item);
		auto tmpCount = tmpLoop.TaskCount();
		sum += tmpCount;
		if (tmpCount < minCount)
		{
			minCount = tmpCount;
			minLoop = &tmpLoop;
		}
	}
	if (IsBusy(selfCount, sum))
	{
		// æ���л�����Ȩ
		// FIX: �����Ƿ���������Ƶ�����⣬��ξ����ּ��ٽ��Ӵ�����
		
		UnRegListen(loop);
		RegListen(*minLoop);
	}
}

const std::vector<std::unique_ptr<Worker>> & TcpAcceptor::GetWorkers()
{
	return GetWorkersAttorney::Workers(*ios_);
}

bool TcpAcceptor::IsBusy(int selfCount, int sum)
{
	// NOTICE: ����: æ�ж�
	constexpr int rsh = 3;
	return selfCount >= (sum - (sum >> rsh));
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


// �п����������̹߳ر�
void TcpAcceptor::Cancel()
{
	auto f = false;
	if (isCanceled_.compare_exchange_strong(f, true, std::memory_order_release))
	{
		// TODO: ������Щ����
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

void TcpAcceptor::SetOnFaild(FaildHandler&& h)
{
	assert(!onFaild_);
	onFaild_ = std::move(h);
}

std::shared_ptr<TcpAcceptor> TcpAcceptor::Create(const std::shared_ptr<IoService>& ios,
	const sockaddr_in& bind)
{
	std::shared_ptr<TcpAcceptor> result(new TcpAcceptor(ios, bind));
	return result;
}

