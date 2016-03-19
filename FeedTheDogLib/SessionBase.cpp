#include "stdafx.h"
#include "SessionBase.h"
#include "SessionPool.h"
#include "Worker.h"
namespace FeedTheDog
{
	SessionBase::SessionBase(TWorker* worker) :
		ios(worker->GetIoService()),
		worker_(worker)
	{
		core = _STD move(worker->GetCore()->shared_from_this());
		isErased = false;
	}
	SessionBase::~SessionBase()
	{
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
	_BOOST system::error_code & SessionBase::GetErrorCode()
	{
		return errorCode;
	}
}  // namespace FeedTheDog


