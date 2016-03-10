#pragma once
#include "Trait.h"
#include "CoreTrait.h"
#include "IService.h"
namespace FeedTheDog
{
	struct WorkerTrait
	{
		typedef IService TService;
		typedef SessionPool TSessionPool;
	};
}  // namespace FeedTheDog
