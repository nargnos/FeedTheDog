#include "stdafx.h"
#include "SessionBase.h"
#include "SessionPool.h"
#include "Worker.h"
namespace FeedTheDog
{
	SessionBase::SessionBase(TWorker* worker) :
		worker_(worker),
		core(worker->GetCore())
	{
	}
	SessionBase::~SessionBase()
	{
	}
	SessionBase::TBufferType& SessionBase::GetBuffer()
	{
		return buffer;
	}
	SessionBase::TReturnCore SessionBase::GetCore()
	{
		return core;
	}
	SessionBase::TWorker* SessionBase::GetWorker()
	{
		return worker_;
	}
}  // namespace FeedTheDog


