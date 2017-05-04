#include "ProactorConnection.h"

ProactorConnectionBase::ProactorConnectionBase(Loop & loop) :
	loop_(loop),
	isTaskRegistered_(false),
	isSocketRegistered_(false)
{
}

bool ProactorConnectionBase::IsClosed() const
{
	return readState_.IsClose();
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


