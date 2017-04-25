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

// TODO: 测试新的结构
// 可以封装起状态，不用enum
//enum class TestS
//{
//	A,
//	B,
//	C
//};
//enum class TestCon
//{
//	X,Y,Z
//};
//class TestC
//{
//public:
//
//	typedef bool(*FFF)(TestC*);
//	TestC()
//	{
//	}
//
//	~TestC()
//	{
//	}
//	// 用重载或者直接分多个函数，这样不行；这里在提交io操作时使用
//	void OnCondition(TestCon st)
//	{
//		status_ = NextStatus(status_,st);
//		f_ = Func(status_);
//	}
//
//	bool Do()
//	{
//		return f_(this);
//		// 由loop执行，执行时this会切换自身状态
//		// 返回bool表示是否要移出tasklist
// // 或者f_返回status，赋值更新函数，这里判断是否结束，f_不应该管理this的状态但是好像会执行到注册的回调从而状态被修改
//	}
// // 非查表实现就需要虚函数（更直观），每次执行都要查一次虚表，这里如果状态不换就能省一些开销
//	static constexpr TestS m[16][16]{ TestS::A,TestS::B,TestS::C,.... };
//	// 这个在编译期就可确定
//	static constexpr TestS NextStatus(TestS status, TestCon s)
//	{		
// // 可能需要分层，发生在io不能一次完成时的reg，如果跟其它状态并在一起就要判断，判断吧，分层有开销
//		return m[static_cast<int>(status)][static_cast<int>(s)];
//	}
//	// 这个也是
//	static constexpr FFF Func(TestS s)
//	{
//		// 返回对应的函数指针
//		// 
//		return nullptr;
//	}
//
//private:
//	FFF f_;
//	TestS status_;
//};
////////////////////////////////////////////



enum class Status :char
{
	// 读写完成
	Complete,
	// 继续读写
	Again,
	// 读写返回EAGAIN
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
	// FIX: 这里可能有问题
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
	// TODO: 这里是否可以替换成状态机，保存一个处理当前状态的函数（返回值是下个状态）指针，
	// 当发生io时注册io task，loop中调用状态指针，省掉重复判断
	// 这样当几个状态同时出现的时候（需要处理好调用到相关函数），taskList对于每个连接都只存一个task(可直接存连接，没有io任务就移出)，也不会频繁push pop，相对有一些优势
	// 当前状态enum要保存，查表获得新的状态函数
	// 缺点是状态划分过小会影响执行，编译器也不能对函数指针优化
	void ReadReady()
	{
		assert(!IsClosed() && !HasError());
		canRead_ = true;
		// 如果有注册完成回调，就向loop推送io task
		if (readTask_ && Self()->IsReadyToRead())
		{
			RegisterIOTask(std::move(readTask_));
		}
	}
	void WriteReady()
	{
		assert(!IsClosed() && !HasError());
		canWrite_ = true;
		if (writeTask_ && Self()->IsReadyToWrite())
		{
			RegisterIOTask(std::move(writeTask_));
		}
	}
	void OnClose()
	{
		if (!isClosed_)
		{
			isClosed_ = true;
			canWrite_ = false;
			canRead_ = false;
			Unregister();
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
			Unregister();
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
		// 不处理不会用到的pri
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
	void RegisterIOTask(std::unique_ptr<ITask>&& task)
	{
		loop_.RegisterTask(std::move(task));
	}
	// FIX: 冗余
	bool ReadTask(std::unique_ptr<ITask>&& task)
	{
		assert(task);
		if (!IsActive())
		{
			// 丢弃task
			return true;
		}
		assert(!HasError() && !IsClosed());
		assert(Self()->IsReadyToRead());
		auto s = Self()->DoRead();
		switch (s)
		{
		case Status::Again:
			// 重新提交task			
			return false;
		case Status::Pause:
			// 当EAGAIN时
			canRead_ = false;
			break;
		case Status::Complete:
			Self()->OnReadSuccess();
			break;
		case Status::Close:
			OnClose();
			return true;
		case Status::Error:
			OnError();
			return true;
		default:
			__builtin_unreachable();
			break;
		}
		// 保存，等待下次使用
		readTask_ = std::move(task);
		return true;
	}
	bool WriteTask(std::unique_ptr<ITask>&& task)
	{
		assert(task);
		if (!IsActive())
		{
			// 丢弃task
			return true;
		}
		assert(!HasError() && !IsClosed());
		assert(Self()->IsReadyToWrite());
		auto s = Self()->DoWrite();
		switch (s)
		{
		case Status::Again:
			// 重新提交task
			return false;
		case Status::Pause:
			// 当EAGAIN时
			canWrite_ = false;
			break;
		case Status::Complete:
			Self()->OnWriteSuccess();
			break;
		case Status::Close:
			OnClose();
			return true;
		case Status::Error:
			OnError();
			return true;
		default:
			__builtin_unreachable();
			break;
		}
		// 保存，等待下次使用

		writeTask_ = std::move(task);
		return true;
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
		// 分发
		if (op.Flags.Err)
		{
			OnError();
			return;
		}
		if (!op.Flags.In && op.Flags.Hup)
		{
			// 发生hup且没有in, 如果有in读完后会自己进入error
			canWrite_ = false;
			OnError();
			return;
		}
		if (op.Flags.In || op.Flags.RdHup)
		{
			// close
			// 读关闭但缓冲区有东西没读
			// 正常读请求

			// 读取操作入队，读到0就close
			ReadReady();
		}

		if (op.Flags.Out && !op.Flags.Hup)
		{
			// 写操作入队
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
	// 当发生in的时候 ptr不空就push进loop，ptr空就跳过
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

