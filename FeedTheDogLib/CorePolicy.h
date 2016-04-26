#pragma once
#include "Define.h"
namespace FeedTheDog
{
	struct CorePolicy
	{
		typedef Worker TWorker;
		template<typename TManager>
		struct TService
		{			
			typedef IService<TManager> TServiceType;
		};
		typedef Config TConfig;
		typedef TraceSource TTraceSource;
		typedef WorkerPool TWorkerPool;
	};

}  // namespace FeedTheDog
