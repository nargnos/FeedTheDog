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

class TcpProactorConnection :
	public ProactorConnection<TcpProactorConnection, TcpRecord<TcpProactorConnection>>,
	public std::enable_shared_from_this<TcpProactorConnection>
{
public:
	using Base = ProactorConnection<TcpProactorConnection, TcpRecord<TcpProactorConnection>>;
	friend ProactorConnection<TcpProactorConnection, TcpRecord<TcpProactorConnection>>;
	using THandler = TcpRecord<TcpProactorConnection>::THandler;
	virtual int FD() const override
	{
		return socket_.FD();
	}

	void AsyncRead(Buffer&& buffer, size_t begin, THandler&& cb)
	{
		assert(std::this_thread::get_id() == GetLoop().OwnerTid());
		assert(!buffer.HasReadOnlyBlock());
		if (IsGood())
		{
			GetReadRecord().emplace(std::move(buffer), begin, std::move(cb));
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

	void AsyncReadSome(Buffer&& buffer, size_t begin, THandler&& cb)
	{
		assert(std::this_thread::get_id() == GetLoop().OwnerTid());
		assert(!buffer.HasReadOnlyBlock());
		if (IsGood())
		{
			GetReadRecord().emplace(std::move(buffer), begin, std::move(cb), TransSome());
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

	void AsyncWrite(Buffer&& buffer, size_t begin, THandler&& cb)
	{
		assert(std::this_thread::get_id() == GetLoop().OwnerTid());
		if (IsGood())
		{
			GetWriteRecord().emplace(std::move(buffer), begin, std::move(cb));
			if (OnBuffReady(WriteState()))
			{
				OnRegTask();
			}
		}
		else
		{
			cb(this, std::move(buffer), Error::Error);
		}
	}
	static std::shared_ptr<TcpProactorConnection> Attach(Loop& loop, int fd)
	{
		return std::shared_ptr<TcpProactorConnection>(new TcpProactorConnection(loop, fd));
	}
protected:
	TcpProactorConnection(Loop& loop, int fd) :
		Base(loop),
		socket_(fd)
	{
		assert(fd != -1);
		constexpr int size = sizeof(TcpProactorConnection);
		SetNonBlocking();
	}
	explicit TcpProactorConnection(Loop & loop) :
		Base(loop)
	{
		if (!socket_)
		{
			TRACEERRNOEXIT(LogPriority::Emerg);
		}
		SetNonBlocking();
	}
	void SetNonBlocking()
	{
		if (!socket_.SetNonBlocking())
		{
			TRACEPOINT(LogPriority::Warning)("tcpsocket setnonblocking faild");
		}
	}

	ssize_t Read(BufferRecord& rec)
	{
		auto bgn = rec.IOVecBgn();
		auto end = rec.IOVecEnd();
		auto dt = std::distance(bgn, end);
		assert(dt > 0);
		ssize_t ret = 0;
		if (__glibc_likely(dt == 1))
		{
			ret = socket_.Read(bgn->iov_base, bgn->iov_len);
		}
		else
		{
			ret = socket_.Readv(&*bgn, static_cast<int>(dt));
		}
		return ret;
	}
	ssize_t Write(BufferRecord& rec)
	{
		auto bgn = rec.IOVecBgn();
		auto end = rec.IOVecEnd();
		auto dt = std::distance(bgn, end);
		assert(dt > 0);
		ssize_t ret = 0;
		if (__glibc_likely(dt == 1))
		{
			ret = socket_.Write(bgn->iov_base, bgn->iov_len);
		}
		else
		{
			ret = socket_.Writev(&*bgn, static_cast<int>(dt));
		}
		return ret;
	}

private:
	TcpSocket socket_;

};
using TcpConnection = TcpProactorConnection;
#endif // TCPSTATUSCONNECTION_H_

