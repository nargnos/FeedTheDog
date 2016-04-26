#include "stdafx.h"
#include "WorkerBase.h"


namespace FeedTheDog
{
	WorkerBase::WorkerBase()
	{
		isRunning = false;
		static int wid = 0;
		id = wid++;
	}
	WorkerBase::~WorkerBase()
	{
	}
	int WorkerBase::GetID() const
	{
		return id;
	}

	void WorkerBase::Start()
	{
		ioService.reset();
		work = make_unique<_ASIO io_service::work>(ioService);
		isRunning = true;
		ioService.run();
	}

	void WorkerBase::Stop()
	{
		isRunning = false;
		CloseAllSessions();
		work.reset();
		ioService.stop();
	}
	_ASIO io_service & WorkerBase::GetIoService()
	{
		return ioService;
	}
}  // namespace FeedTheDog
