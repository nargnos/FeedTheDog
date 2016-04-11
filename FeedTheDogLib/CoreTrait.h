#pragma once
#include "Trait.h"
#include "WorkerTrait.h"
namespace FeedTheDog
{
	struct CoreTrait
	{
		template<typename TOwner>
		struct TCore
		{
			typedef Worker<TOwner,WorkerTrait> TWorkerType;
			typedef IService<TOwner> TService;
		};
		typedef Config TConfig;
		typedef TraceSource<Config::TEnum> TTraceSource;
	};

}  // namespace FeedTheDog
