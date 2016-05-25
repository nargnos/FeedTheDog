#include "stdafx.h"
#include "WorkerBase.h"


namespace FeedTheDog
{
	WorkerBase::WorkerBase():
		ioService_(1)
	{
		isRunning_ = false;
		static int wid = 0;
		id_ = ++wid;
	}
	WorkerBase::~WorkerBase()
	{
		assert(!isRunning_);
	}
	int WorkerBase::GetID() const
	{
		return id_;
	}

	void WorkerBase::Start()
	{
		_ASIO io_service::work work(ioService_);		
		isRunning_ = true;
		ioService_.run();
	}

	void WorkerBase::Stop()
	{
		ioService_.stop();		
		isRunning_ = false;
	}
	_ASIO io_service & WorkerBase::GetIoService()
	{
		return ioService_;
	}

}  // namespace FeedTheDog
