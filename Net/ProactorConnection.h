#ifndef STATUSCONNECTION_H_
#define STATUSCONNECTION_H_
#include <cassert>
#include <memory>
#include <queue>
#include <atomic>
#include <sys/epoll.h>
#include "ITask.h"
#include "IFDTask.h"
#include "IoState.h"
#include "Noncopyable.h"
#include "FDTaskCtlAttorney.h"
#include "BufferRecord.h"
#include "Loop.h"
#include "Logger.h"
#include "RegisterTaskAttorney.h"
namespace Detail
{
	// FIX: �е��ң�����һ��
	// TODO: �ع�
	class ProactorConnectionBase :
		public IFDTask,
		public Noncopyable
	{
	public:
		ProactorConnectionBase(Loop& loop, IoState::IoStatus readStat, IoState::IoStatus writeStat);

		bool HasError() const;
		bool IsClosed() const;
		bool IsGood() const;
		Loop& GetLoop() const;
	protected:

		IoState& ReadState();
		IoState& WriteState();
		// io�����е���, ���������ע��epoll�ȴ�io�ɴ���
		void OnCantIo(IoState& s);
		// �¼����ʱ����
		void OnNoBuff(IoState& s);
		// ����true��ʾ��ǰ��io
		// post�¼�ʱ����
		bool OnBuffReady(IoState& s);
		// ep��cb�е���
		bool OnIoReady(IoState& s);

		void OnFailState() const;

		void RegisterSocket();
		void UnregisterSocket();

		Loop& loop_;
		IoState readState_;
		IoState writeState_;
		// FIX: ��connect��ʱ����õ������ܻ��������̹߳����������
		std::atomic_bool isSocketRegistered_;
		bool isTaskRegistered_;
	};


	template<typename TChild>
	class ProactorConnection :
		public ProactorConnectionBase,
		public ITask
	{
	public:
		using TRecord = ProgressRecord<TChild>;
		explicit ProactorConnection(
			Loop& loop, IoState::IoStatus readStat, IoState::IoStatus writeStat) :
			ProactorConnectionBase(loop, readStat, writeStat),
			store_(nullptr)
		{

		}

	protected:
		using TRecordPtr = std::unique_ptr<TRecord>;
		using TQueue = std::queue<TRecordPtr>;

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
			RegisterTaskAttorney::RegisterTask(loop_, Self()->shared_from_this());
		}
		void SetError()
		{
			readState_.SetError();
			writeState_.SetError();
		}

		// ����洢���ã�������������
		// TODO: ��������Ҫ�Ը����������
		// ERROR: ��loop����ֹʱ��ִ�в�������
		// ���޲�ʹ��������ʹ��ά�ֵķ���
		void StoreObj()
		{
			if (store_)
			{
				return;
			}
			// �������ü���
			store_ = Self()->shared_from_this();
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
			// �������ɶ���
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
			if (__glibc_unlikely(!IsGood()))
			{
				return;
			}
			SetError();
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

		void OnClose()
		{
			if (__glibc_unlikely(!IsGood()))
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
			auto& recQueue = GetReadRecord();
			assert(!recQueue.empty());
			auto& f = *recQueue.front();
			if (__glibc_likely(f.Do(*Self(), &ProactorConnection::DoRead)))
			{
				PopRecord(recQueue, ReadState());
			}

		}
		void OnWrite()
		{
			auto& recQueue = GetWriteRecord();
			assert(!recQueue.empty());
			auto& f = *recQueue.front();
			if (__glibc_likely(f.Do(*Self(), &ProactorConnection::DoWrite)))
			{
				PopRecord(recQueue, WriteState());
			}
		}

		void PopRecord(TQueue& queue, IoState& s)
		{
			queue.pop();
			if (__glibc_likely(queue.empty()))
			{
				OnNoBuff(s);
				assert(!s.IsBuffReady());
				return;
			}
		}

		// ����true��ʾҪ�������
		bool DoRead(TRecord& rec, TransferProgress& progress)
		{
			assert(ReadState().IsDoIo());
			auto& bufRec = progress;
			auto trans = Self()->Read(bufRec);

			switch (trans)
			{
			default:
				// > 0
				if (bufRec.IsTransSome())
				{
					bufRec.GetBuffer().Resize(bufRec.Trans() + trans);
					RunCompleteHandler(rec, Error::Success);
					return true;
				}

				bufRec.AddTrans(trans);
				if (!bufRec.IsComplete())
				{
					return false;
				}
				RunCompleteHandler(rec, Error::Success);
				return true;
			case -1:
			{
				auto err = errno;
				if (__glibc_likely(IsContinue(err)))
				{
					OnCantIo(ReadState());
					assert(!ReadState().IsIoReady());
					return false;
				}
				TRACEPOINT(LogPriority::Debug)("%d fd:%s", FD(), LOGSTR_ERRNO);
			}
			break;
			case 0:
				OnClose();
				assert(ReadState().IsClose());
				// ��������onclose������
				assert(GetReadRecord().empty());
				return false;
			}
			OnError();
			assert(ReadState().HasError());
			// ͬ��
			assert(GetReadRecord().empty());
			return false;
		}

		bool DoWrite(TRecord& rec, TransferProgress& progress)
		{
			assert(WriteState().IsDoIo());
			auto& bufRec = progress;
			auto trans = Self()->Write(bufRec);
			if (__glibc_likely(trans > 0))
			{
				bufRec.AddTrans(trans);
				if (!bufRec.IsComplete())
				{
					return false;
				}
				RunCompleteHandler(rec, Error::Success);
				return true;
			}
			else
			{
				auto err = errno;
				if (__glibc_likely(IsContinue(err)))
				{
					OnCantIo(WriteState());
					assert(!WriteState().IsIoReady());
					assert(GetWriteRecord().empty());
					return false;
				}
			}
			OnError();
			assert(WriteState().HasError());
			assert(GetWriteRecord().empty());
			return false;
		}

		bool IsContinue(int err)
		{
			return err == EAGAIN || err == EWOULDBLOCK || err == EINTR;
		}

		void RunCompleteHandler(TRecord& rec, Error e)
		{
			rec.Complete(*Self(), e);
		}

		void OnFail(TQueue& queue, Error e)
		{
			while (!queue.empty())
			{
				auto& rec = *queue.front();
				// �����ʱ���������ڣ�Ҫ������Լ��������ﲻ����
				RunCompleteHandler(rec, e);
				queue.pop();
			}
		}

		TQueue readRecord_;
		TQueue writeRecord_;
		std::shared_ptr<ProactorConnectionBase> store_;

	};

}  // namespace Detail
#endif // STATUSCONNECTION_H_

