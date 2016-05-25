#include "stdafx.h"
#include "ServiceBaseImpl.h"

namespace FeedTheDog
{
	ServiceBaseImpl::ServiceBaseImpl(TWorkerPool& workerPool) :
		workerPool_(workerPool)
	{
	}
	ServiceBaseImpl::~ServiceBaseImpl()
	{
	}
}  // namespace FeedTheDog
