#include "stdafx.h"
#include "WorkerBase.h"
#include "Core.h"
namespace FeedTheDog
{
	WorkerBase::~WorkerBase()
	{
		owner->GetTrace()->TracePoint(LogMsg::FreeWorker, true, id);
	}
	int WorkerBase::GetID() const
	{
		return id;
	}

	WorkerBase::TCore * WorkerBase::GetCore() const
	{
		return owner;
	}
	void WorkerBase::Start()
	{
		ioService.reset();
		work = make_unique<_ASIO io_service::work>(ioService);
		isRunning = true;
		owner->GetTrace()->TracePoint(LogMsg::StartWorker, true, id);
		ioService.run();
	}

	void WorkerBase::Stop()
	{
		isRunning = false;
		CloseAllSessions();
		work.reset();
		//ioService.stop();

		owner->GetTrace()->TracePoint(LogMsg::StopWorker, true, id);
	}
	_ASIO io_service & WorkerBase::GetIoService()
	{
		return ioService;
	}
	WorkerBase::WorkerBase(TCore * core)
	{
		isRunning = false;
		static int wid = 0;
		id = wid++;
		owner = core;
		owner->GetTrace()->TracePoint(LogMsg::NewWorker, true, id);
	}
}  // namespace FeedTheDog
