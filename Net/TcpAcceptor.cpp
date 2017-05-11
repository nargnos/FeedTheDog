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
namespace Detail
{
	TcpAcceptor::TcpAcceptor(const sockaddr_in& bind) :
		ios_(IoService::Instance()),
		isCanceled_(false),
		needReregister_(false)
	{
		assert(ios_);
		assert(ios_->WorkerCount() > 0);
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
		needReregister_ = ios_->WorkerCount() > 1;
		RegListen();
	}

	void TcpAcceptor::RegListen()
	{
		// ѡ��workerע��
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
		// ֻ�������߳�ʹ��
		FDTaskCtlAttorney::Del(loop, this);
	}


	void TcpAcceptor::DoAccept(Loop & loop, int fd)
	{
		assert(fd != -1);
		// ������ά��connect�����ڣ�ִ��rw������رղ���������
		auto sock = TcpAttachAttorney::Attach(loop, fd);
		// ��ʱ���������cb�ļ�϶�������ӹ�������
		if (onAccept_)
		{
			onAccept_(sock.get());
		}
	}

	void TcpAcceptor::DoEvent(Loop & loop, EpollOption op)
	{
		if (__glibc_unlikely(op.Flags.Err))
		{
			UnRegListen(loop);
			OnFailed();
			assert(false);
			return;
		}
		assert(op.Flags.In);
		do
		{
			auto fd = socket_.Accept();
			if (__glibc_likely(fd != -1))
			{
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
		} while (true);

		// NOTICE: ����: �ѻ�����������Ϳ��ǽ���Ȩ��
		// ��ֵԽ�󲨶�Խ�󣬵�ĳ��ֵԽСԽ�����ż����߻��ø�cpu��ƽ��	
		constexpr int limit = 8;
		if (!needReregister_ || loop.TaskCount() < limit)
		{
			return;
		}

		// ȡ���������æ���Ҹ���С��ע��
		// ������Ҫ׼ȷ
		// TODO: �л�������Ӧ���ܱ�������
		auto& perf = ios_->PerformanceSnapshot();
		// TODO: �㷨��̫�ԣ��е㲻ƽ��������������ٴ�������
		if (perf.IdleCount == 0 || (perf.BusyLoop == &loop && IsTooBusy(perf.BusyCount, perf.TaskCount)))
		{
			TRACEPOINT(LogPriority::Debug)("Switch! self: %d  max: %d  idle: %d",
				perf.BusyCount, perf.TaskCount, perf.IdleCount);
			// ��æ���л�����Ȩ
			// FIX: �����Ƿ���������Ƶ�����⣬��ξ����ּ��ٽ��Ӵ�����
			// TODO: �������loopִ��ǰ��������������Եõ��������߼��ٵ����ʣ������������
			UnRegListen(loop);
			RegListen(*perf.IdleLoop);
		}
	}

	bool TcpAcceptor::IsTooBusy(int selfCount, int sum)
	{
		// NOTICE: ����: æ�ж�
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

	void TcpAcceptor::SetOnFailed(FailedHandler&& h)
	{
		assert(!onFailed_);
		onFailed_ = std::move(h);
	}

	std::shared_ptr<TcpAcceptor> TcpAcceptor::Create(const sockaddr_in& bind)
	{
		std::shared_ptr<TcpAcceptor> result(new TcpAcceptor(bind));
		return result;
	}

}  // namespace Detail
