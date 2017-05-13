#include "ProactorConnection.h"
namespace Detail
{

	bool ProactorConnectionBase::IsClosed() const
	{
		return readState_.IsClose();
	}

	ProactorConnectionBase::ProactorConnectionBase(Loop & loop, IoState::IoStatus readStat, IoState::IoStatus writeStat) :
		loop_(loop),
		readState_(readStat),
		writeState_(writeStat),
		isSocketRegistered_(false),
		isTaskRegistered_(false)
	{
	}

	bool ProactorConnectionBase::HasError() const
	{
		return readState_.HasError();
	}

	bool ProactorConnectionBase::IsGood() const
	{
		// 错误状态会同时设置，所以这里只用判断一个
		return readState_.IsGood();
		//return readState_.IsGood() && writeState_.IsGood();
	}

	Loop & ProactorConnectionBase::GetLoop() const
	{
		return loop_;
	}


	bool ProactorConnectionBase::OnBuffReady(IoState & s)
	{
		s.SetBuffReady(true);
		return s.IsDoIo();
	}

	bool ProactorConnectionBase::OnIoReady(IoState & s)
	{
		s.SetIoReady(true);
		return s.IsDoIo();
	}

	void ProactorConnectionBase::OnFailState() const
	{
		assert(false);
		throw nullptr;
	}

	IoState & ProactorConnectionBase::ReadState()
	{
		return readState_;
	}

	IoState & ProactorConnectionBase::WriteState()
	{
		return writeState_;
	}

	// io过程中调用, 出现这个必注册epoll等待io可处理

	void ProactorConnectionBase::OnCantIo(IoState & s)
	{
		assert(s.IsDoIo());
		s.SetIoReady(false);
		assert(!s.IsIoReady());
	}

	// 事件完成时调用

	void ProactorConnectionBase::OnNoBuff(IoState & s)
	{
		s.SetBuffReady(false);
	}

	void ProactorConnectionBase::UnregisterSocket()
	{
		if (!isSocketRegistered_.load(std::memory_order_acquire))
		{
			return;
		}
		FDTaskCtlAttorney::Del(loop_, this);
		isSocketRegistered_.store(false,std::memory_order_release);
	}

	void ProactorConnectionBase::RegisterSocket()
	{
		// FIX: 当选择空闲线程投递connect的一些环境条件，如果条件考虑不全或被破坏，这里和unreg会有问题
		// add后unreg才有机会执行
		// reg同时被多个线程（2个，当前未完全退出）进入也是在add后
		// add后reg不会在本线程再次被调用，之后的reg和unreg都是在同一线程被调用
		// TODO: 队列那是否也要
		if (isSocketRegistered_.load(std::memory_order_acquire))
		{
			return;
		}
		isSocketRegistered_.store(true, std::memory_order_release);
		// 不处理不会用到的pri
		FDTaskCtlAttorney::Add(loop_, EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP, this);
	}
}  // namespace Detail


