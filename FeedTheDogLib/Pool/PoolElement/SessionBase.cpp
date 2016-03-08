#include "SessionBase.h"
#include "..\..\Core\Core.h"
#include "..\SessionPool.h"
namespace FeedTheDog
{
	SessionBase::SessionBase(typename CoreTrait::TSessionPool* ptr)
	{
		pool = ptr->shared_from_this();
	}
	SessionBase::~SessionBase()
	{
	}
	SessionBase::TBufferType& SessionBase::GetBuffer()
	{
		return buffer;
	}

	shared_ptr<SessionBase::TSessionPool>& SessionBase::GetIdleSessionPool()
	{
		return pool;
	}

	SessionBase::TCore * SessionBase::GetCore() const
	{
		return pool->GetWorker()->GetCore();
	}
	
}  // namespace FeedTheDog


