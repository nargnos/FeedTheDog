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
	// FIX: 有点乱，再整一整
	// TODO: 重构
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
		// io过程中调用, 出现这个必注册epoll等待io可处理
		void OnCantIo(IoState& s);
		// 事件完成时调用
		void OnNoBuff(IoState& s);
		// 返回true表示当前可io
		// post事件时调用
		bool OnBuffReady(IoState& s);
		// ep的cb中调用
		bool OnIoReady(IoState& s);

		void OnFailState() const;

		void RegisterSocket();
		void UnregisterSocket();

		Loop& loop_;
		IoState readState_;
		IoState writeState_;
		// FIX: 在connect的时候会用到，可能会与其它线程共用这个变量
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
		// 设置task并且注册
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

		// 自身存储引用，保护不被析构
		// TODO: 有隐患，要对各种情况测试
		// ERROR: 当loop被终止时会执行不到析构
		// 有无不使用容器能使它维持的方法
		void StoreObj()
		{
			if (store_)
			{
				return;
			}
			// 提升引用计数
			store_ = Self()->shared_from_this();
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
			// 牺牲掉可读性
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

		// 返回true表示要清理队首
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
				// 队列已在onclose中清理
				assert(GetReadRecord().empty());
				return false;
			}
			OnError();
			assert(ReadState().HasError());
			// 同上
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
				// 如果此时有数据在内，要处理就自己读，这里不处理
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

