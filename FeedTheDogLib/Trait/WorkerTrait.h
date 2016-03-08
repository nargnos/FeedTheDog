#pragma once
#include "Trait.h"
#include "CoreTrait.h"
#include "..\Interface\IService.h"
namespace FeedTheDog
{
	struct WorkerTrait
	{
		typedef IService TService;
		typedef SessionPool TSessionPool;
	};
}  // namespace FeedTheDog
