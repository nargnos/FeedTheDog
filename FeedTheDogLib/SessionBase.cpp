#include "stdafx.h"
#include "SessionBase.h"
#include "Core.h"
#include "SessionPool.h"
namespace FeedTheDog
{
	SessionBase::SessionBase(TCore* ptr, _ASIO io_service& io) :
		ios(io)
	{
		core = ptr;
		isClosed = false;
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
	SessionBase::TCore* SessionBase::GetCore() const
	{
		return core;
	}

}  // namespace FeedTheDog


