#include "stdafx.h"
#include "WorkerBase.h"


namespace FeedTheDog
{
	WorkerBase::WorkerBase():
		ioService_(1)
	{
		counter_ = 0;
		isRunning_ = false;
		static int wid = 0;
		id_ = ++wid;
	}
	WorkerBase::~WorkerBase()
	{
		if (isRunning_)
		{
			Stop();
		}
	}
	int WorkerBase::GetID() const
	{
		return id_;
	}

	void WorkerBase::Start()
	{
		ioService_.reset();
		work_ = make_unique<_ASIO io_service::work>(ioService_);
		isRunning_ = true;
		ioService_.run();
	}

	void WorkerBase::Stop()
	{
		isRunning_ = false;
		work_.reset();
		ioService_.stop();
	}
	_ASIO io_service & WorkerBase::GetIoService()
	{
		return ioService_;
	}
	const _ASIO io_service & WorkerBase::GetIoService() const
	{
		return ioService_;
	}
	unsigned int WorkerBase::GetSessionCount() const
	{
		return counter_.load(_STD memory_order_relaxed);
	}
}  // namespace FeedTheDog
