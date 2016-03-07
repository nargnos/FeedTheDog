#pragma once
#include "Trait.h"
namespace FeedTheDog
{
	struct CoreTrait
	{
		typedef Core TCore;
		typedef Worker TWorker;
		typedef IService TService;
		typedef MemoryPool TMemoryPool;
		typedef SessionPool TSessionPool;
	};

}  // namespace FeedTheDog
