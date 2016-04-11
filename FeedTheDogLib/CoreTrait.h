#pragma once
#include "Trait.h"
namespace FeedTheDog
{
	struct CoreTrait
	{
		template<typename TOwner>
		struct TCore
		{
			typedef Worker<TOwner> TWorkerType;
			typedef IService<TOwner> TService;
		};
	};

}  // namespace FeedTheDog
