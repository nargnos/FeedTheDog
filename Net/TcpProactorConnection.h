#ifndef TCPSTATUSCONNECTION_H_
#define TCPSTATUSCONNECTION_H_
#include <queue>
#include <memory>
#include <functional>
#include "TcpOperator.h"
#include "ProactorConnection.h"
#include "Logger.h"
#include "SocketCpp.h"
#include "Buffer.h"
#include "Task.h"
#include "IoService.h"
namespace Detail
{
	class TcpAttachAttorney;
	class GetSocketAttorney;

	class TcpProactorConnection :
		public ProactorConnection
	{
	public:
		// 事先检查回调类型，否则错误提示就跑另一边去了
#define CheckCallback(type) CheckHandler(void(TcpProactorConnection&, Buffer&&, Error), type)

		friend TcpAttachAttorney;
		friend GetSocketAttorney;
		friend ConnectionAttorney;
		TcpProactorConnection(Loop& loop, int fd);

		~TcpProactorConnection() = default;

		template<typename THandler>
		void AsyncReadSome(THandler&& cb, size_t limit = 0)
		{
			CheckTid();
			CheckFd();
			CheckCallback(cb);
			// 发生错误但数据未读完，允许继续读，
			// 未读完前Error判断不会失败，但自身标记为错误，这时写操作会失败
			// assert(!HasError());
			PostReadOp(MakeReadSomeOp(*this, std::forward<THandler>(cb), limit));
			CheckAndRegTask(EPOLLIN);
		}
		template<typename THandler>
		void AsyncRead(size_t size, THandler&& cb)
		{
			CheckTid();
			CheckFd();
			CheckCallback(cb);

			// assert(!HasError());
			PostReadOp(MakeReadOp(*this, size, std::forward<THandler>(cb)));
			CheckAndRegTask(EPOLLIN);

		}

		template<typename PrefixType, typename TCallback, typename TPrefixHandler>
		void AsyncReadMessage(TCallback&& cb, TPrefixHandler&& ph)
		{
			CheckTid();
			CheckFd();
			CheckCallback(cb);

			// assert(!HasError());
			PostReadOp(MakeReadMessageOp<PrefixType>(
				*this, std::forward<TCallback>(cb), std::forward<TPrefixHandler>(ph)));
			CheckAndRegTask(EPOLLIN);
		}

		template<typename THandler>
		void AsyncWrite(Buffer&& buff, size_t begin, THandler&& cb)
		{
			CheckTid();
			CheckFd();
			CheckCallback(cb);
			// assert(!HasError());
			PostWriteOp(MakeWriteOp(*this, std::move(buff), begin, std::forward<THandler>(cb)));			
			CheckAndRegTask(EPOLLOUT);
		}
		bool SetKeepAlive(bool opt) const;
		bool SetNoDelay(bool opt) const;
	private:

		TcpSocket& Socket();
		static std::shared_ptr<TcpProactorConnection> Attach(Loop & loop, int fd);
		TcpSocket socket_;
	};


}  // namespace Detail

using  TcpConnection = Detail::TcpProactorConnection;

#endif // TCPSTATUSCONNECTION_H_
