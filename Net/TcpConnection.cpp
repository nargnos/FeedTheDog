#include "TcpConnection.h"
#include <thread>
#include "Logger.h"
#include "Loop.h"
#include "Task.h"


TcpConnection::TcpConnection(Loop& loop, int fd) :
	Connection<TcpConnection>(loop),
	socket_(fd)
{
	assert(fd != -1);
	
	if (!socket_.SetNonBlocking())
	{
		TRACEPOINT(LogPriority::Warning)("tcpsocket setnonblocking faild");
	}
	
}


void TcpConnection::DispatchReadIoStatus(Status s)
{
	switch (s)
	{
	case Status::Complete:
		OnReadSuccess();
		break;
	case Status::Again:
	case Status::Pause:
		Register();
		break;
	case Status::Close:
		OnClose();
		break;
	case Status::Error:
		OnError();
		break;
	default:
		__builtin_unreachable();
		break;
	}
}
void TcpConnection::DispatchWriteIoStatus(Status s)
{
	switch (s)
	{
	case Status::Complete:
		OnWriteSuccess();
		break;
	case Status::Again:
	case Status::Pause:
		Register();
		break;
	case Status::Close:
		OnClose();
		break;
	case Status::Error:
		OnError();
		break;
	default:
		__builtin_unreachable();
		break;
	}
}
TcpSocket & TcpConnection::Socket()
{
	return socket_;
}

const TcpSocket & TcpConnection::Socket() const
{
	return socket_;
}

bool TcpConnection::IsReadyToRead() const
{
	return !readCompleteHandlers_.empty();
}

bool TcpConnection::IsReadyToWrite() const
{
	return !writeCompleteHandlers_.empty();
}

void TcpConnection::OnReadFaild(Error e)
{
	while (!readCompleteHandlers_.empty())
	{
		auto& item = *readCompleteHandlers_.front();
		item.Callback()(this, std::move(item.GetBuffer()), e);
		readCompleteHandlers_.pop();
	}
}

void TcpConnection::OnWriteFaild(Error e)
{
	while (!writeCompleteHandlers_.empty())
	{
		auto& item = *writeCompleteHandlers_.front();
		item.Callback()(this, std::move(item.GetBuffer()), e);
		writeCompleteHandlers_.pop();
	}
}

void TcpConnection::PostReadCompleteHandler(Error error)
{
	PostCompleteHandler(readCompleteHandlers_, error);
}

void TcpConnection::PostCompleteHandler(std::queue<RecPtr>& queue, Error error)
{
	assert(!queue.empty());
	auto& item = queue.front();
	item->SetError(error);
	loop_.RegisterTask(std::move(item));
	queue.pop();
}

void TcpConnection::OnReadSuccess()
{
	PostReadCompleteHandler(Error::Success);
}

void TcpConnection::OnWriteSuccess()
{
	PostWriteCompleteHandler(Error::Success);
}

void TcpConnection::InitTask()
{
	if (!isInited_)
	{
		isInited_ = true;
		auto self = shared_from_this();
		// task维持周期，在error或close时清掉
		// 此时task可能还在loop里，执行完task不继续就析构掉
		readTask_ = MakeTask([self](Loop& loop, std::unique_ptr<ITask>&& task)
		{
			assert(std::addressof(loop) == std::addressof(self->GetLoop()));
			return self->ReadTask(std::move(task));
		});

		writeTask_ = MakeTask([self](Loop& loop, std::unique_ptr<ITask>&& task)
		{			
			assert(std::addressof(loop) == std::addressof(self->GetLoop()));
			return self->WriteTask(std::move(task));
		});
	}
}

void TcpConnection::PostWriteCompleteHandler(Error error)
{
	PostCompleteHandler(writeCompleteHandlers_, error);
}


TcpConnection::~TcpConnection() = default;

Status TcpConnection::DoRead()
{
	assert(IsReadyToRead());
	assert(!IsClosed() && !HasError());
	if (!canRead_)
	{
		return Status::Pause;
	}
	auto& rec = *readCompleteHandlers_.front();
	auto& iov = rec.IOVec();
	auto& bgn = std::get<0>(iov);
	auto& end = std::get<1>(iov);
	auto dt = std::distance(bgn, end);
	assert(dt > 0);
	ssize_t trans = 0;
	if (__glibc_likely(dt == 1))
	{
		trans = socket_.Read(bgn->iov_base, bgn->iov_len);
	}
	else
	{
		trans = socket_.Readv(&*bgn, static_cast<int>(dt));
	}
	if (__glibc_likely(trans > 0))
	{
		if (rec.IsTransSome())
		{
			rec.GetBuffer().Resize(rec.Trans() + trans);
			return Status::Complete;
		}
		rec.AddTrans(trans);
		return rec.IsComplete() ? Status::Complete : Status::Again;
	}
	else if (trans == -1)
	{
		auto err = errno;
		if (err == EAGAIN || err == EWOULDBLOCK || err == EINTR)
		{
			return Status::Pause;
		}
		TRACEPOINT(LogPriority::Debug)("%d fd:%s", socket_.FD(), LOGSTR_ERRNO);
	}
	else if (trans == 0)
	{
		return Status::Close;
	}

	return Status::Error;
}
Status TcpConnection::DoWrite()
{
	assert(IsReadyToWrite());
	assert(!IsClosed() && !HasError());
	if (!canWrite_)
	{
		return Status::Pause;
	}
	auto& rec = *writeCompleteHandlers_.front();
	auto& iov = rec.IOVec();
	auto& bgn = std::get<0>(iov);
	auto& end = std::get<1>(iov);
	auto dt = std::distance(bgn, end);
	assert(dt > 0);
	ssize_t trans = 0;
	if (__glibc_likely(dt == 1))
	{
		trans = socket_.Write(bgn->iov_base, bgn->iov_len);
	}
	else
	{

		trans = socket_.Writev(&*bgn, static_cast<int>(dt));
	}
	if (__glibc_likely(trans > 0))
	{
		rec.AddTrans(trans);
		return rec.IsComplete() ? Status::Complete : Status::Again;
	}
	else
	{
		auto err = errno;
		if (err == EAGAIN || err == EWOULDBLOCK || err == EINTR)
		{
			return Status::Pause;
		}
	}
	return Status::Error;
}
int TcpConnection::FD() const
{
	return socket_.FD();
}

std::shared_ptr<TcpConnection> TcpConnection::Create(Loop & loop, int fd)
{
	return std::shared_ptr<TcpConnection>(new TcpConnection(loop, fd));
}


void TcpConnection::AsyncRead(Buffer&& buffer, size_t begin, CompleteHandler && h)
{
	assert(std::this_thread::get_id() == loop_.OwnerTid());
	if (!IsClosed())
	{
		InitTask();
		readCompleteHandlers_.emplace(new Rec(shared_from_this(), std::move(buffer), std::move(h), begin));
		auto s = DoRead();
		DispatchReadIoStatus(s);
	}
	else
	{
		h(this, std::move(buffer), Error::Error);
	}
}

void TcpConnection::AsyncReadSome(Buffer&& buffer, size_t begin, CompleteHandler && h)
{
	assert(std::this_thread::get_id() == loop_.OwnerTid());
	if (!IsClosed())
	{
		InitTask();
		readCompleteHandlers_.emplace(new Rec(shared_from_this(), std::move(buffer), std::move(h), begin, TransSome()));
		auto s = DoRead();
		DispatchReadIoStatus(s);
	}
	else
	{
		h(this, std::move(buffer), Error::Error);
	}
}

void TcpConnection::AsyncWrite(Buffer&& buffer, size_t begin, CompleteHandler && h)
{
	assert(std::this_thread::get_id() == loop_.OwnerTid());
	if (!IsClosed())
	{
		InitTask();
		writeCompleteHandlers_.emplace(new Rec(shared_from_this(), std::move(buffer), std::move(h), begin));
		auto s = DoWrite();
		DispatchWriteIoStatus(s);
	}
	else
	{
		h(this, std::move(buffer), Error::Error);
	}
}
