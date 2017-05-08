#include "ProactorConnection.h"

bool ProactorConnectionBase::IsClosed() const
{
	return readState_.IsClose();
}

ProactorConnectionBase::ProactorConnectionBase(Loop & loop, IoState::IoStatus readStat, IoState::IoStatus writeStat) :
	loop_(loop),
	readState_(readStat),
	writeState_(writeStat),
	isTaskRegistered_(false),
	isSocketRegistered_(false)
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
	if (!isSocketRegistered_)
	{
		return;
	}
	isSocketRegistered_ = false;
	FDTaskCtlAttorney::Del(loop_, this);
}

void ProactorConnectionBase::RegisterSocket()
{
	if (isSocketRegistered_)
	{
		return;
	}
	isSocketRegistered_ = true;
	// 不处理不会用到的pri
	FDTaskCtlAttorney::Add(loop_, EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP, this);
}


