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
class TcpAttachAttorney;
class TcpProactorConnection;

// ���̰߳�ȫ��һ����
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
		// �����ж���shared_ptr�д���
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
		if (IsGood())
		{
			GetReadRecord().push(MakeRecord<TcpProactorConnection>(std::move(buffer), begin, std::forward<THandler>(cb)));
			// �п���socketһֱ����epoll
			// ��Ϊ���ʱ����ִ��cb���������Ԥ�����ܻῨ��ĳ�������ϣ�����Ҫ����task
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
		// TODO: ��ռλ����Ӧ��һЩ״̬Ҫ����
		return socket_.Shutdown(m);
	}

	// FIX: ��һ��С����Ҫ����
	template<typename THandler>
	void AsyncConnect(const sockaddr_in& addr, THandler&& cb)
	{
		// TODO: EINPROGRESS ��ֱ�ӷ�ep��ע������������ӻ�ʹ���߳���������ƽ�⣬Ӧ����ѡ�߳��ٴ���
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
				cb(this, Error::Success);
				return;
			}
		} while (true);

		if (err == EINPROGRESS)
		{
			GetWriteRecord().push(MakeRecord<TcpProactorConnection>(std::forward<THandler>(cb)));
			StoreObj();
			RegisterSocket();

			return;
		}
		cb(this, Error::Error);
	}

	// void(TcpConnection* self, Buffer&& buf, Error e)
	template<typename THandler>
	void AsyncReadSome(Buffer&& buffer, size_t begin, THandler&& cb)
	{
		assert(std::this_thread::get_id() == GetLoop().OwnerTid());
		assert(!buffer.IsEmpty());
		assert(!buffer.HasReadOnlyBlock());
		if (IsGood())
		{
			GetReadRecord().push(MakeRecord<TcpProactorConnection>(std::move(buffer), begin, std::forward<THandler>(cb), true));
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

	template<typename THandler>
	void AsyncWrite(Buffer&& buffer, size_t begin, THandler&& cb)
	{
		assert(!buffer.IsEmpty());
		assert(std::this_thread::get_id() == GetLoop().OwnerTid());
		if (IsGood())
		{
			GetWriteRecord().push(MakeRecord<TcpProactorConnection>(std::move(buffer), begin, std::forward<THandler>(cb)));
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

	// ��������ͬloop�������ӣ���ͬ�߳�Ͷ��
	std::shared_ptr<TcpProactorConnection> Clone() const
	{
		return std::shared_ptr<TcpProactorConnection>(new TcpProactorConnection(loop_));
	}

	// ѡ������Loop����
	// ����һ�������࣬����ֻ����connectһ��
	static TcpProactorConnectionProxy Create()
	{
		auto info = IoService::Instance()->PerformanceSnapshot();
		return TcpProactorConnectionProxy(std::shared_ptr<TcpProactorConnection>(new TcpProactorConnection(info.IdleLoop)));
	}
protected:
	// fd Ϊ������
	TcpProactorConnection(Loop& loop, int fd) :
		Base(loop, IoState::IoReady, IoState::IoReady),
		socket_(fd)
	{
		assert(fd != -1);
		constexpr int size = sizeof(TcpProactorConnection);
		SetNonBlocking();
	}
	// ��������Ҫconnect����ʹ��
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
			TRACEPOINT(LogPriority::Warning)("tcpsocket setnonblocking faild");
		}
	}

	ssize_t Read(TransferProgress& rec)
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
	ssize_t Write(TransferProgress& rec)
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

#endif // TCPSTATUSCONNECTION_H_
