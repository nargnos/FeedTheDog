#include "stdafx.h"
#include "Worker.h"
#include "Core.h"

namespace FeedTheDog
{
	Worker::Worker(TCore* core):
		sessionPool(core, ioService)
	{
		isRunning = false;
		static int wid = 0;
		id = wid++;
		owner = core;
		owner->GetTrace()->TracePoint(LogMsg::NewWorker, true, id);
	}

	Worker::~Worker()
	{
		owner->GetTrace()->TracePoint(LogMsg::FreeWorker, true, id);
	}

	Worker::TSessionPool* Worker::GetSessionPool()
	{
		return &sessionPool;
	}

	_ASIO io_service& Worker::GetIoService()
	{
		return ioService;
	}

	int Worker::GetID() const
	{
		return id;
	}

	

	void Worker::Start()
	{
		work = make_unique<_ASIO io_service::work>(ioService);

		ioService.reset();
		isRunning = true;
		owner->GetTrace()->TracePoint(LogMsg::StartWorker, true, id);
		ioService.run();
	}
	void Worker::Stop()
	{
		work.reset();
		sessionPool.CloseAll();
		
		ioService.stop();
		isRunning = false;
		owner->GetTrace()->TracePoint(LogMsg::StopWorker, true, id);
	}

	Worker::TCore* Worker::GetCore() const
	{
		return owner;
	}

}  // namespace FeedTheDog

