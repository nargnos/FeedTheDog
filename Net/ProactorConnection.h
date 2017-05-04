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
	// io�����е���, ���������ע��epoll�ȴ�io�ɴ���
	void OnCantIo(IoState& s)
	{
		assert(s.IsDoIo());
		s.SetIoReady(false);
		assert(!s.IsIoReady());

		/*RegisterSocket();
		StoreObj();*/
	}
	// �¼����ʱ����
	void OnNoBuff(IoState & s)
	{
		s.SetBuffReady(false);
	}
	// ����true��ʾ��ǰ��io
	// post�¼�ʱ����
	bool OnBuffReady(IoState& s)
	{
		s.SetBuffReady(true);
		return s.IsDoIo();
	}
	// ep��cb�е���
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
	// ����task����ע��
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


	// ����epoll�¼�
	virtual void DoEvent(Loop& loop, EpollOption op)  override
	{
		assert(IsGood());
		assert(std::addressof(loop) == std::addressof(GetLoop()));
		// �ַ�
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
		//	// ��������г���
		//	OnError();
		//	return;
		//}
		//if (!op.Flags.In && op.Flags.Hup)
		//{
		//	// ����hup��û��in, �����in�������Լ�����error
		//	// �Է��Ͽ�
		//	OnError();
		//	return;
		//}
		//if (op.Flags.In || op.Flags.RdHup)
		//{
		//	// close
		//	// ���رյ��������ж���û��
		//	// ����������

		//	// ����0��close
		//	OnIoReady(ReadState());
		//}

		//if (op.Flags.Out && !op.Flags.Hup)
		//{
		//	// д��������
		//	OnIoReady(WriteState());
		//}
		//OnRegisterTask();
	}
	// ����task�¼���ִֻ��IO��������ɡ��رջ�����¼�ʱ�͵ش���ע��
	// ����epoll��������ִ��cb��ʱ������״̬, ������ж�û������ã��Լ���������

	virtual bool DoEvent(Loop & loop) override
	{
		if (readState_.IsDoIo())
		{
			OnRead();
		}
		// ��һ�׶�д������ѭ������
		// read ���������write��ʹ���ﲻ�ᱻִ��
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
			// ����ִ��io -> ����task false
			return false;
		default:
			// ����close��error
			break;
		case 6:
			// 1 doio 1 bufready
			// �ȴ�io���ã�ע�ᣬͬʱ����ִ��io -> ����task false
			RegisterSocket();
			StoreObj();
			return false;
		case 2:
		case 4:
			// 1 ioready 1 bufready || 2 bufready
			// �ȴ�io���ã�ע�� -> ע��task true
			RegisterSocket();
			StoreObj();
			break;
		case 1:
			// 2 io ready
			// δpost�ص���ע������ -> ע��task true
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
		// �������has buf��reg,store
		// �������doio ->false������ȫtrue
		// �������io rdy����ʾ��ʱδ�ύ�ص���detach unreg->true
		// ����������cb���ѱ�ִ�У�ֱ��true

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
	// ����洢���ã�������������
	// TODO: ��������Ҫ�Ը����������
	void StoreObj()
	{
		if (store_)
		{
			return;
		}
		// �������ü���
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
			// �����ʱ���������ڣ�Ҫ������Լ��������ﲻ����
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

