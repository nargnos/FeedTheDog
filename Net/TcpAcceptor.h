#ifndef TCPACCEPTOR_H_
#define TCPACCEPTOR_H_
#include <memory>
#include <atomic>
#include <functional>
#include <array>
#include "SocketCpp.h"
#include "Logger.h"
#include "TcpProactorConnection.h"
namespace Detail
{
	class Worker;
	class IoService;
	class TcpAcceptor :
		public std::enable_shared_from_this<TcpAcceptor>,
		public IFDTask,
		public Noncopyable
	{
	public:
		using ConnectionPtr = TcpConnection*;
		using AcceptHandler = std::function<void(ConnectionPtr)>;
		using FailedHandler = std::function<void()>;
		virtual int FD() const;
		void Cancel();
		// 函数声明为 void(const std::shared_ptr<TcpConnection>&)
		void SetOnAccept(AcceptHandler&& handler);
		// 默认出错就退出, 设置后会取消默认操作
		void SetOnFailed(FailedHandler&& handler);

		virtual ~TcpAcceptor();
		static std::shared_ptr<TcpAcceptor> Create(const sockaddr_in& bind);
	private:
		explicit TcpAcceptor(const sockaddr_in& bind);
		void RegListen();
		void RegListen(Loop & loop);
		void UnRegListen(Loop& loop);
		void DoAccept(Loop & loop, int fd);
		// 会在多个线程被调用，但一次只有一个线程能访问
		virtual void DoEvent(Loop& loop, EpollOption op);
		bool IsTooBusy(int selfCount, int sum);
		void OnFailed();

		FailedHandler onFailed_;
		std::shared_ptr<IoService> ios_;
		TcpSocket socket_;
		AcceptHandler onAccept_;
		std::atomic_bool isCanceled_;
		bool needReregister_;
	};
}  // namespace Detail
using Detail::TcpAcceptor;
#endif // !TCPACCEPTOR_H_

