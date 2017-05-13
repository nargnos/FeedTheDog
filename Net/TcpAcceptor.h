#ifndef TCPACCEPTOR_H_
#define TCPACCEPTOR_H_
#include <memory>
#include <atomic>
#include <functional>
#include <array>
#include "SocketCpp.h"
#include "Logger.h"
#include "TcpProactorConnection.h"
#include "TcpAttachAttorney.h"
namespace Detail
{
	class Worker;
	class IoService;
	class ITcpAcceptor 
	{
	public:
		ITcpAcceptor() = default;
		virtual ~ITcpAcceptor() = default;
		virtual void Cancel() = 0;
		virtual void Join() const= 0;
	private:

	};


	class TcpAcceptorBase :
		protected IFDTask,
		public ITcpAcceptor,
		public Noncopyable
	{
	public:
		explicit TcpAcceptorBase(const sockaddr_in& bind);
		virtual ~TcpAcceptorBase();

		virtual int FD() const;
		// 有可能在其它线程关闭
		virtual void Cancel() override;
		virtual void Join() const override;
	protected:
		void RegListen();
		void RegListen(Loop & loop);
		void UnRegListen(Loop& loop);

		void Balance(Loop& loop);
		bool IsTooBusy(size_t selfCount, size_t sum);

		std::shared_ptr<IoService> ios_;
		TcpSocket socket_;
		std::atomic_bool isCanceled_;
		bool needReregister_;

	};
	template<typename TAcceptHandler, typename TFailedHandler>
	class TcpAccetpor :
		public TcpAcceptorBase
	{
	public:
		TcpAccetpor(const sockaddr_in& bind, TAcceptHandler&& ah, TFailedHandler&& fh) :
			TcpAcceptorBase(bind),
			acceptHandler_(std::forward<TAcceptHandler>(ah)),
			failedHandler_(std::forward<TFailedHandler>(fh))
		{
		}

	private:
		void DoAccept(Loop & loop, int fd)
		{
			assert(fd != -1);
			// 不处理（维持connect生存期，执行rw）将会关闭并丢弃连接
			auto sock = TcpAttachAttorney::Attach(loop, fd);
			acceptHandler_(*sock);
		}
		// 会在多个线程被调用，但一次只有一个线程能访问
		virtual void DoEvent(Loop& loop, EpollOption op)
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

			Balance(loop);
		}
		void OnFailed()
		{
			failedHandler_();
			TRACEPOINT_LINE(LogPriority::Debug);
			TRACEERRNOEXIT(LogPriority::Emerg);
		}

		TAcceptHandler acceptHandler_;
		TFailedHandler failedHandler_;
	};
	// void(TcpConnection&)
	// void()
	template<typename TAcceptHandler, typename TFailedHandler>
	std::shared_ptr<ITcpAcceptor> CreateTcpAccepter(const sockaddr_in& bind,
		TAcceptHandler&& acceptCallback, TFailedHandler&& failedCallback)
	{
		return std::make_shared<TcpAccetpor<TAcceptHandler, TFailedHandler>>(
			bind, std::forward<TAcceptHandler>(acceptCallback), std::forward<TFailedHandler>(failedCallback));
	}
	// void(TcpConnection&)
	template<typename TAcceptHandler>
	std::shared_ptr<ITcpAcceptor> CreateTcpAccepter(const sockaddr_in& bind,
		TAcceptHandler&& acceptCallback)
	{
		return CreateTcpAccepter(bind, std::forward<TAcceptHandler>(acceptCallback), []() {});
	}
}  // namespace Detail
using Detail::CreateTcpAccepter;
#endif // TCPACCEPTOR_H_

