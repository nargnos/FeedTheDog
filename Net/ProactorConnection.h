#ifndef STATUSCONNECTION_H_
#define STATUSCONNECTION_H_
#include <cassert>
#include <memory>
#include <queue>
#include <sys/epoll.h>
#include "ITask.h"
#include "IFDTask.h"
#include "IoState.h"
#include "Noncopyable.h"
#include "FDTaskCtlAttorney.h"
#include "BufferRecord.h"
#include "Loop.h"
#include "Logger.h"
#include "ObjStore.h"



class ProactorConnectionBase :
	public IFDTask,
	public Noncopyable
{
public:
	explicit ProactorConnectionBase(Loop& loop);

	bool HasError() const;
	bool IsClosed() const;
	bool IsGood() const;
	Loop& GetLoop() const;
protected:

	IoState& ReadState();
	IoState& WriteState();
	// io过程中调用, 出现这个必注册epoll等待io可处理
	void OnCantIo(IoState& s)
	{
		assert(s.IsDoIo());
		s.SetIoReady(false);
		assert(!s.IsIoReady());

		/*RegisterSocket();
		StoreObj();*/
	}
	// 事件完成时调用
	void OnNoBuff(IoState & s)
	{
		s.SetBuffReady(false);
	}
	// 返回true表示当前可io
	// post事件时调用
	bool OnBuffReady(IoState& s)
	{
		s.SetBuffReady(true);
		return s.IsDoIo();
	}
	// ep的cb中调用
	bool OnIoReady(IoState& s)
	{
		s.SetIoReady(true);
		return s.IsDoIo();
	}


	void OnFailState() const;
	void RegisterSocket();
	void UnregisterSocket();

	Loop& loop_;
	IoState readState_;
	IoState writeState_;
	bool isTaskRegistered_;
	bool isSocketRegistered_;
};


template<typename TChild, typename TRecord>
class ProactorConnection :
	public ProactorConnectionBase,
	public ITask
{
public:
	explicit ProactorConnection(Loop& loop) :
		ProactorConnectionBase(loop),
		store_(nullptr)
	{

	}

protected:
	using TQueue = std::queue<TRecord>;

	TQueue& GetReadRecord()
	{
		return readRecord_;
	}
	TQueue& GetWriteRecord()
	{
		return writeRecord_;
	}

	const TChild* Self() const
	{
		return static_cast<const TChild*>(this);
	}
	TChild* Self()
	{
		return static_cast<TChild*>(this);
	}
	// 设置task并且注册
	void OnRegTask()
	{
		if (isTaskRegistered_)
		{
			return;
		}
		isTaskRegistered_ = true;
		loop_.RegisterTask(Self()->shared_from_this());
	}

private:


	// 处理epoll事件
	virtual void DoEvent(Loop& loop, EpollOption op)  override
	{
		assert(IsGood());
		assert(std::addressof(loop) == std::addressof(GetLoop()));
		// 分发
		if ((op & EPOLLERR) || ((op & (EPOLLIN | EPOLLHUP))) == EPOLLHUP)
		{
			OnError();
			return;
		}
		bool ret = false;
		if (op & (EPOLLIN | EPOLLRDHUP))
		{
			ret = OnIoReady(ReadState());
		}

		if ((op & (EPOLLOUT | EPOLLHUP)) == EPOLLOUT)
		{
			ret |= OnIoReady(WriteState());
		}
		if (ret)
		{
			OnRegTask();
		}

		//if (op.Flags.Err)
		//{
		//	// 处理过程中出错
		//	OnError();
		//	return;
		//}
		//if (!op.Flags.In && op.Flags.Hup)
		//{
		//	// 发生hup且没有in, 如果有in读完后会自己进入error
		//	// 对方断开
		//	OnError();
		//	return;
		//}
		//if (op.Flags.In || op.Flags.RdHup)
		//{
		//	// close
		//	// 读关闭但缓冲区有东西没读
		//	// 正常读请求

		//	// 读到0就close
		//	OnIoReady(ReadState());
		//}

		//if (op.Flags.Out && !op.Flags.Hup)
		//{
		//	// 写操作就绪
		//	OnIoReady(WriteState());
		//}
		//OnRegisterTask();
	}
	// 处理task事件，只执行IO，发生完成、关闭或错误事件时就地处理并注销
	// 不进epoll的连接在执行cb的时候设置状态, 如果空闲而没相关引用，自己就析构掉

	virtual bool DoEvent(Loop & loop) override
	{
		if (readState_.IsDoIo())
		{
			OnRead();
		}
		// 做一套读写，减少循环次数
		// read 出错会设置write，使这里不会被执行
		if (writeState_.IsDoIo())
		{
			OnWrite();
		}
		auto val = readState_.Value() * writeState_.Value();
		assert(val != 0);

		switch (val)
		{
		case 3:
		case 9:
			// 2 doio || 1 doio 1 ioready
			// 继续执行io -> 保持task false
			return false;
		default:
			// 出错，close或error
			break;
		case 6:
			// 1 doio 1 bufready
			// 等待io可用，注册，同时继续执行io -> 保持task false
			RegisterSocket();
			StoreObj();
			return false;
		case 2:
		case 4:
			// 1 ioready 1 bufready || 2 bufready
			// 等待io可用，注册 -> 注销task true
			RegisterSocket();
			StoreObj();
			break;
		case 1:
			// 2 io ready
			// 未post回调，注销所有 -> 注销task true
			UnregisterSocket();
			DestoryObj();
			break;
		case 0:
			OnFailState();
			__builtin_unreachable();
			break;
		}

		isTaskRegistered_ = false;
		return true;
		// 如果存在has buf就reg,store
		// 如果存在doio ->false，否则全true
		// 如果都是io rdy，表示此时未提交回调。detach unreg->true
		// 如果错误，相关cb都已被执行，直接true

	}


	void OnError()
	{
		if (!IsGood())
		{
			return;
		}
		readState_.SetError();
		writeState_.SetError();
		assert(readState_.HasError());
		assert(writeState_.HasError());
		OnFail(GetReadRecord(), Error::Error);
		OnFail(GetWriteRecord(), Error::Error);
		UnregisterSocket();
		DestoryObj();
	}

	void DestoryObj()
	{
		if (!store_)
		{
			return;
		}
		store_ = nullptr;
	}
	// 自身存储引用，保护不被析构
	// TODO: 有隐患，要对各种情况测试
	void StoreObj()
	{
		if (store_)
		{
			return;
		}
		// 提升引用计数
		store_ = Self()->shared_from_this();
	}


	void OnClose()
	{
		if (!IsGood())
		{
			return;
		}
		readState_.SetClose();
		writeState_.SetClose();
		assert(readState_.IsClose());
		assert(writeState_.IsClose());
		OnFail(GetReadRecord(), Error::Close);
		OnFail(GetWriteRecord(), Error::Close);
		UnregisterSocket();
		DestoryObj();
	}
	void OnRead()
	{
		assert(ReadState().IsDoIo());
		auto& recQueue = GetReadRecord();
		auto& rec = recQueue.front();
		auto& bufRec = rec;
		auto trans = Self()->Read(bufRec);

		switch (trans)
		{
		default:
			// > 0
			if (bufRec.IsTransSome())
			{
				bufRec.GetBuffer().Resize(bufRec.Trans() + trans);
				OnComplete(recQueue, rec, ReadState());
				return;
			}

			bufRec.AddTrans(trans);
			if (!bufRec.IsComplete())
			{
				return;
			}
			OnComplete(recQueue, rec, ReadState());
			return;
		case -1:
		{
			auto err = errno;
			if (err == EAGAIN || err == EWOULDBLOCK || err == EINTR)
			{
				OnCantIo(ReadState());
				assert(!ReadState().IsIoReady());
				return;
			}
			TRACEPOINT(LogPriority::Debug)("%d fd:%s", FD(), LOGSTR_ERRNO);
		}
		break;
		case 0:
			OnClose();
			assert(ReadState().IsClose());
			return;
		}
		OnError();
		assert(ReadState().HasError());
	}

	void OnWrite()
	{
		assert(WriteState().IsDoIo());
		auto& recQueue = GetWriteRecord();
		auto& rec = recQueue.front();
		auto& bufRec = rec;
		auto trans = Self()->Write(bufRec);
		if (__glibc_likely(trans > 0))
		{
			bufRec.AddTrans(trans);
			if (!bufRec.IsComplete())
			{
				return;
			}
			OnComplete(recQueue, rec, WriteState());
			return;
		}
		else
		{
			auto err = errno;
			if (err == EAGAIN || err == EWOULDBLOCK || err == EINTR)
			{
				OnCantIo(WriteState());
				assert(!WriteState().IsIoReady());
				return;
			}
		}
		OnError();
		assert(WriteState().HasError());
	}

	void OnComplete(TQueue& recQueue, TRecord & rec, IoState& s)
	{
		assert(s.IsDoIo());
		RunCompleteHandler(rec, Error::Success);
		recQueue.pop();
		if (recQueue.empty())
		{
			OnNoBuff(s);
			assert(!s.IsBuffReady());
			return;
		}
		assert(s.IsDoIo());
	}


	void RunCompleteHandler(TRecord& rec, Error e)
	{
		rec.Call(Self(), std::move(rec.GetBuffer()), e);
	}

	void OnFail(TQueue& queue, Error e)
	{
		while (!queue.empty())
		{
			auto& rec = queue.front();
			// 如果此时有数据在内，要处理就自己读，这里不处理
			//auto tr = rec.BR.Trans();
			//if (tr != 0)
			//{
			//	auto& buff = rec.BR.GetBuffer();
			//	buff.Resize(tr);
			//}
			RunCompleteHandler(rec, e);
			queue.pop();
		}
	}
	std::queue<TRecord> readRecord_;
	std::queue<TRecord> writeRecord_;
	std::shared_ptr<ProactorConnectionBase> store_;

};

#endif // STATUSCONNECTION_H_

