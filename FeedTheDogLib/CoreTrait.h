#pragma once
#include "Trait.h"
namespace FeedTheDog
{
	class Worker;
	struct MemoryPoolStrategy;
	struct SessionStorageStrategy;
	struct CoreTrait
	{
		typedef Core TCore;
		typedef Worker TWorker;
		typedef IService TService;
		template<typename TProtocol>
		struct TSessionPool
		{
			typedef SessionPool<TProtocol,Worker,SessionPoolTrait,MemoryPoolStrategy, SessionStorageStrategy> type;
		};
	};

}  // namespace FeedTheDog
