#pragma once
#include "Trait.h"
namespace FeedTheDog
{
	struct CoreTrait
	{
		typedef Core TCore;
		typedef Worker TWorker;
		typedef IService TService;
		//typedef SessionPool TSessionPool;
		template<typename TProtocol>
		struct TSessionPool
		{
			typedef SessionPool<TProtocol> type;
		};
	};

}  // namespace FeedTheDog
