#include "stdafx.h"
#include "SessionBase.h"
#include "SessionPool.h"
#include "Worker.h"
namespace FeedTheDog
{
	SessionBase::SessionBase(TWorker* worker, _ASIO io_service& io) :
		ios(io),
		timer(io),
		worker_(worker)
	{
		core = worker->GetCore()->shared_from_this();
		isErased = false;
	}
	void SessionBase::Close()
	{
		timer.cancel(ignored_ec);
	}
	_ASIO deadline_timer & SessionBase::GetTimer()
	{
		return timer;
	}
	SessionBase::~SessionBase()
	{
		Close();
	}
	SessionBase::TBufferType& SessionBase::GetBuffer()
	{
		return buffer;
	}
	_ASIO io_service& SessionBase::GetIoService()
	{
		return ios;
	}
	shared_ptr<SessionBase::TCore>& SessionBase::GetCore()
	{
		return core;
	}
	SessionBase::TWorker* SessionBase::GetWorker()
	{
		return worker_;
	}
}  // namespace FeedTheDog


