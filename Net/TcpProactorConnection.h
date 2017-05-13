#ifndef TCPSTATUSCONNECTION_H_
#define TCPSTATUSCONNECTION_H_
#include <queue>
#include <memory>
#include <functional>
#include "ProactorConnection.h"
#include "Logger.h"
#include "SocketCpp.h"
#include "Buffer.h"
#include "BufferRecord.h"
#include "Task.h"
#include "IoService.h"
namespace Detail
{
	class TcpAttachAttorney;
	class TcpProactorConnection;

	// 非线程安全，一次性
	class TcpProactorConnectionProxy :
		public Noncopyable
	{
	public:
		explicit TcpProactorConnectionProxy(std::shared_ptr<TcpProactorConnection>&& des) :
			des_(std::move(des))
		{
		}
		TcpProactorConnectionProxy(TcpProactorConnectionProxy&& obj) :
			des_(std::move(obj.des_))
		{
			// 自身判断在shared_ptr中处理
		}
		~TcpProactorConnectionProxy() = default;
		template<typename THandler>
		void AsyncConnect(const sockaddr_in& addr, THandler&& cb);
	private:
		std::shared_ptr<TcpProactorConnection> des_;
	};

	class TcpProactorConnection :
		public ProactorConnection<TcpProactorConnection>,
		public std::enable_shared_from_this<TcpProactorConnection>
	{
	public:
		using Base = ProactorConnection<TcpProactorConnection>;
		friend ProactorConnection<TcpProactorConnection>;
		friend TcpAttachAttorney;

		virtual int FD() const override
		{
			return socket_.FD();
		}
		template<typename THandler>
		void AsyncRead(Buffer&& buffer, size_t begin, THandler&& cb)
		{
			assert(std::this_thread::get_id() == GetLoop().OwnerTid());
			assert(!buffer.IsEmpty());
			assert(!buffer.HasReadOnlyBlock());
			if (__glibc_likely(IsGood()))
			{
				GetReadRecord().push(
					MakeRecord<TcpProactorConnection>(std::move(buffer), begin, std::forward<THandler>(cb)));
				// 有可能socket一直不进epoll
				// 因为完成时立即执行cb，如果这里预读可能会卡在某个连接上，所以要先入task
				if (OnBuffReady(ReadState()))
				{
					OnRegTask();
				}
			}
			else
			{
				cb(this, std::move(buffer), Error::Error);
			}
		}
		bool Shutdown(ShutdownMode m)
		{
			assert(false);
			// TODO: 先占位，相应的一些状态要设置
			return socket_.Shutdown(m);
		}

		// FIX: 有一点小错，需要处理
		template<typename THandler>
		void AsyncConnect(const sockaddr_in& addr, THandler&& cb)
		{
			// TODO: EINPROGRESS 后直接放ep，注意如果单纯连接会使各线程连接数不平衡，应该先选线程再处理
			std::decay_t<decltype(errno)> err = 0;
			do
			{
				if (!socket_.Connect(addr))
				{
					err = errno;
					if (err == EINTR)
					{
						continue;
					}
					break;
				}
				else
				{
					cb(*this, Error::Success);
					return;
				}
			} while (true);
			// TODO: 重试和超时
			if (err == EINPROGRESS)
			{
				auto rec = MakeRecord<TcpProactorConnection>(std::forward<THandler>(cb));

				GetWriteRecord().push(std::move(rec));
				StoreObj();
				RegisterSocket();

				return;
			}
			cb(*this, Error::Error);
		}

		// void(TcpConnection& self, Buffer&& buf, Error e)
		template<typename THandler>
		void AsyncReadSome(Buffer&& buffer, size_t begin, THandler&& cb)
		{
			assert(std::this_thread::get_id() == GetLoop().OwnerTid());
			assert(!buffer.IsEmpty());
			assert(!buffer.HasReadOnlyBlock());
			if (__glibc_likely(IsGood()))
			{
				GetReadRecord().push(
					MakeRecord<TcpProactorConnection>(std::move(buffer), begin, std::forward<THandler>(cb), true));
				if (OnBuffReady(ReadState()))
				{
					OnRegTask();
				}
			}
			else
			{
				cb(*this, std::move(buffer), Error::Error);
			}
		}

		template<typename THandler>
		void AsyncWrite(Buffer&& buffer, size_t begin, THandler&& cb)
		{
			assert(!buffer.IsEmpty());
			assert(std::this_thread::get_id() == GetLoop().OwnerTid());
			if (__glibc_likely(IsGood()))
			{
				GetWriteRecord().push(
					MakeRecord<TcpProactorConnection>(std::move(buffer), begin, std::forward<THandler>(cb)));
				if (OnBuffReady(WriteState()))
				{
					OnRegTask();
				}
			}
			else
			{
				cb(*this, std::move(buffer), Error::Error);
			}
		}

		// 创建具有同loop的新连接，在同线程投递
		std::shared_ptr<TcpProactorConnection> Clone() const
		{
			return std::shared_ptr<TcpProactorConnection>(new TcpProactorConnection(loop_));
		}

		// 选择最闲Loop创建
		// 创建一个代理类，限制只能用connect一次
		static TcpProactorConnectionProxy Create()
		{
			auto& info = IoService::Instance()->PerformanceSnapshot();
			return TcpProactorConnectionProxy(
				std::shared_ptr<TcpProactorConnection>(new TcpProactorConnection(*info.IdleLoop)));
		}
	protected:
		// fd 为已连接
		TcpProactorConnection(Loop& loop, int fd) :
			Base(loop, IoState::IoReady, IoState::IoReady),
			socket_(fd)
		{
			assert(fd != -1);
			// constexpr int size = sizeof(TcpProactorConnection);
			SetNonBlocking();
		}
		// 创建，需要connect才能使用
		explicit TcpProactorConnection(Loop & loop) :
			Base(loop, IoState::IoReady, IoState::BufReady)
		{
			if (!socket_)
			{
				TRACEERRNOEXIT(LogPriority::Emerg);
			}
			SetNonBlocking();
		}
		static std::shared_ptr<TcpProactorConnection> Attach(Loop& loop, int fd)
		{
			return std::shared_ptr<TcpProactorConnection>(new TcpProactorConnection(loop, fd));
		}
		void SetNonBlocking()
		{
			if (!socket_.SetNonBlocking())
			{
				TRACEPOINT(LogPriority::Warning)("tcpsocket setnonblocking failed");
			}
		}

		ssize_t Read(TransferProgress& rec)
		{
			auto bgn = rec.IOVecBgn();
			auto end = rec.IOVecEnd();
			auto dt = std::distance(bgn, end);
			assert(dt > 0);
			if (__glibc_likely(dt == 1))
			{
				return socket_.Read(bgn->iov_base, bgn->iov_len);
			}
			return socket_.Readv(&*bgn, static_cast<int>(dt));
		}
		ssize_t Write(TransferProgress& rec)
		{
			auto bgn = rec.IOVecBgn();
			auto end = rec.IOVecEnd();
			auto dt = std::distance(bgn, end);
			assert(dt > 0);
			if (__glibc_likely(dt == 1))
			{
				return socket_.Write(bgn->iov_base, bgn->iov_len);
			}
			return socket_.Writev(&*bgn, static_cast<int>(dt));
		}

	private:
		TcpSocket socket_;
	};
	using TcpConnection = TcpProactorConnection;


	template<typename THandler>
	void TcpProactorConnectionProxy::AsyncConnect(const sockaddr_in & addr, THandler && cb)
	{
		assert(des_);
		if (!des_)
		{
			return;
		}

		des_->AsyncConnect(addr, std::forward<THandler>(cb));
		des_ = nullptr;
	}

}  // namespace Detail

using Detail::TcpConnection;

#endif // TCPSTATUSCONNECTION_H_
