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
		friend TcpAttachAttorney;
		friend GetSocketAttorney;
		TcpProactorConnection(Loop& loop, int fd);

		~TcpProactorConnection() = default;

		template<typename THandler>
		void AsyncReadSome(THandler&& cb, size_t limit = 0)
		{
			assert(!HasError());
			PostReadOp(MakeReadSomeOp(*this, std::forward<THandler>(cb), limit));
			CheckAndRegTask(EPOLLIN);

		}
		template<typename THandler>
		void AsyncRead(size_t size, THandler&& cb)
		{
			assert(!HasError());
			PostReadOp(MakeReadOp(*this, size, std::forward<THandler>(cb)));
			CheckAndRegTask(EPOLLIN);

		}

		template<typename PrefixType, typename TCallback, typename TPrefixHandler>
		void AsyncReadMessage(TCallback&& cb, TPrefixHandler&& ph)
		{
			assert(!HasError());
			PostReadOp(MakeReadMessageOp<PrefixType>(
				*this, std::forward<TCallback>(cb), std::forward<TPrefixHandler>(ph)));
			CheckAndRegTask(EPOLLIN);
		}

		template<typename THandler>
		void AsyncWrite(Buffer&& buff, size_t begin, THandler&& cb)
		{
			assert(!HasError());
			PostWriteOp(MakeWriteOp(*this, std::move(buff), begin, std::forward<THandler>(cb)));			
			CheckAndRegTask(EPOLLOUT);
		}
		virtual int FD() const override;
	private:
		TcpSocket& Socket();
		static std::shared_ptr<TcpProactorConnection> Attach(Loop & loop, int fd);
		void SetNonBlocking();
		TcpSocket socket_;

	};


}  // namespace Detail

using  TcpConnection = Detail::TcpProactorConnection;

#endif // TCPSTATUSCONNECTION_H_
