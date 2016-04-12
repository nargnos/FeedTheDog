#pragma once
#include "Define.h"
#include "WorkerPolicy.h"
namespace FeedTheDog
{
	struct CorePolicy
	{
		template<typename TOwner>
		struct TCore
		{
			typedef Worker<TOwner,WorkerPolicy> TWorkerType;
			typedef IService<TOwner> TService;
		};
		typedef Config TConfig;
		typedef TraceSource TTraceSource;
	};

}  // namespace FeedTheDog
