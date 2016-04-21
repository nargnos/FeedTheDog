#pragma once
#include "Define.h"
#include "WorkerPolicy.h"
namespace FeedTheDog
{
	struct CorePolicy
	{
		typedef Worker<WorkerPolicy> TWorker;
		template<typename TManager>
		struct TService
		{			
			typedef IService<TManager> TServiceType;
		};
		typedef Config TConfig;
		typedef TraceSource TTraceSource;
		typedef WorkerPool<TWorker> TWorkerPool;
	};

}  // namespace FeedTheDog
