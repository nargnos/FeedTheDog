#ifndef ICONNECTION_H_
#define ICONNECTION_H_
#include <memory>
#include <queue>
#include <cassert>
#include <sys/epoll.h>
#include "FDTaskCtlProxy.h"
#include "Loop.h"
#include "Task.h"
#include "IFDTask.h"
#include "ITask.h"
#include "Noncopyable.h"
#include "TransRec.h"

//virtual bool IsReadyToRead() const = 0;
//virtual bool IsReadyToWrite() const = 0;
//virtual void OnReadSuccess() = 0;
//virtual void OnWriteSuccess() = 0;
//virtual void OnReadFaild(Error e) = 0;
//virtual void OnWriteFaild(Error e) = 0;
//virtual Status DoRead() = 0;
//virtual Status DoWrite() = 0;


enum class Status :char
{
	// ��д���
	Complete,
	// ������д
	Again,
	// ��д����EAGAIN
	Pause,
	Close,
	Error
};
template<typename TChild>
class Connection :
	public Noncopyable,
	public IFDTask
{
public:
	explicit Connection(Loop & loop) :
		loop_(loop),
		isRegisted_(false),
		canRead_(false),
		canWrite_(false),
		isClosed_(false),
		hasError_(false),
		isInited_(false)
	{

	}
	void Close()
	{
		OnClose();
	}
	bool IsClosed()const { return isClosed_; }
	bool HasError()const { return hasError_; }
	Loop& GetLoop()const
	{
		return loop_;
	}
protected:
	bool CanRead()const { return canRead_; }
	bool IsActive()const { return isRegisted_; }
	bool CanWrite()const { return canWrite_; }
	void ReadReady()
	{
		assert(!IsClosed() && !HasError());
		canRead_ = true;
		// �����ע����ɻص�������loop����io task
		if (readTask_ && Self()->IsReadyToRead())
		{
			QueueIOTask(std::move(readTask_));
		}
	}
	void WriteReady()
	{
		assert(!IsClosed() && !HasError());
		canWrite_ = true;
		if (writeTask_ && Self()->IsReadyToWrite())
		{
			QueueIOTask(std::move(writeTask_));
		}
	}
	void OnClose()
	{
		if (!isClosed_)
		{
			isClosed_ = true;
			canWrite_ = false;
			canRead_ = false;
			//Unregister();
			NotifyFaild(Error::Close);
			DestoryTask();
		}
	}
	void OnError()
	{
		if (!hasError_ && !isClosed_)
		{
			hasError_ = true;
			isClosed_ = true;
			canWrite_ = false;
			canRead_ = false;
			//Unregister();
			NotifyFaild(Error::Error);
			DestoryTask();
		}
	}
	void NotifyFaild(Error e)
	{
		Self()->OnReadFaild(e);
		Self()->OnWriteFaild(e);
	}
	void Register()
	{
		if (isRegisted_)
		{
			return;
		}
		isRegisted_ = true;
		// ���������õ���pri
		FDTaskCtlProxy::Add(loop_, EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP, this);
	}
	void Unregister()
	{
		if (!isRegisted_)
		{
			return;
		}
		isRegisted_ = false;
		FDTaskCtlProxy::Del(loop_, this);
	}
	void QueueIOTask(std::unique_ptr<ITask>&& task)
	{
		loop_.QueueTask(std::move(task));
	}
	// FIX: ����
	void ReadTask(std::unique_ptr<ITask>&& task)
	{
		assert(task);
		if (!IsActive())
		{
			// ����task
			return;
		}
		assert(!HasError() && !IsClosed());
		assert(Self()->IsReadyToRead());
		auto s = Self()->DoRead();
		switch (s)
		{
		case Status::Again:
			// �����ύtask
			QueueIOTask(std::move(task));
			return;
		case Status::Pause:
			// ��EAGAINʱ
			canRead_ = false;
			break;
		case Status::Complete:
			Self()->OnReadSuccess();
			break;
		case Status::Close:
			OnClose();
			return;
		case Status::Error:
			OnError();
			return;
		default:
			__builtin_unreachable();
			break;
		}
		// ���棬�ȴ��´�ʹ��
		readTask_ = std::move(task);
	}
	void WriteTask(std::unique_ptr<ITask>&& task)
	{
		assert(task);
		if (!IsActive())
		{
			// ����task
			return;
		}
		assert(!HasError() && !IsClosed());
		assert(Self()->IsReadyToWrite());
		auto s = Self()->DoWrite();
		switch (s)
		{
		case Status::Again:
			// �����ύtask
			QueueIOTask(std::move(task));
			return;
		case Status::Pause:
			// ��EAGAINʱ
			canWrite_ = false;
			break;
		case Status::Complete:
			Self()->OnWriteSuccess();
			break;
		case Status::Close:
			OnClose();
			return;
		case Status::Error:
			OnError();
			return;
		default:
			__builtin_unreachable();
			break;
		}
		// ���棬�ȴ��´�ʹ��

		writeTask_ = std::move(task);
	}
	void DestoryTask()
	{
		if (isInited_)
		{
			readTask_ = nullptr;
			writeTask_ = nullptr;
		}
	}
	virtual void DoEvent(Loop& loop, EpollOption op)  override
	{
		assert(!hasError_);
		assert(!isClosed_);
		assert(std::addressof(loop) == std::addressof(GetLoop()));
		// �ַ�
		if (op.Flags.Err)
		{
			OnError();
			return;
		}
		if (!op.Flags.In && op.Flags.Hup)
		{
			// ����hup��û��in, �����in�������Լ�����error
			canWrite_ = false;
			OnError();
			return;
		}
		if (op.Flags.In || op.Flags.RdHup)
		{
			// close
			// ���رյ��������ж���û��
			// ����������

			// ��ȡ������ӣ�����0��close
			ReadReady();
		}

		if (op.Flags.Out && !op.Flags.Hup)
		{
			// д�������
			WriteReady();
		}
	}

	const TChild* Self() const
	{
		return static_cast<const TChild*>(this);
	}
	TChild* Self()
	{
		return static_cast<TChild*>(this);
	}
	// ������in��ʱ�� ptr���վ�push��loop��ptr�վ�����
	std::unique_ptr<ITask> readTask_;
	std::unique_ptr<ITask> writeTask_;
	Loop& loop_;
	bool isRegisted_;
	bool canRead_;
	bool canWrite_;
	bool isClosed_;
	bool hasError_;
	bool isInited_;
};


#endif // ICONNECTION_H_

