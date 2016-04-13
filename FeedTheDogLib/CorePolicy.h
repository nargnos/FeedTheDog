#pragma once
#include "Define.h"
#include "WorkerPolicy.h"
namespace FeedTheDog
{
	struct CorePolicy
	{
		template<typename TManager>
		struct TCore
		{
			typedef Worker<TManager, WorkerPolicy> TWorkerType;
			typedef IService<TManager> TService;
			typedef _STD vector<unique_ptr<TWorkerType>> TWorkerVectorType;

			inline static TWorkerVectorType CreateWorkerVector(TManager* manager, unsigned int count)
			{
				TWorkerVectorType workers;
				for (unsigned int i = 0; i < count; i++)
				{
					workers.push_back(make_unique<TWorkerType>(manager));
				}
				return workers;
			}
			inline static void StopWorkers(TWorkerVectorType& workers)
			{
				for each (auto& var in workers)
				{
					var->Stop();
				}
			}
			inline static TWorkerType* SelectIdleWorker(TWorkerVectorType& workers)
			{
				auto result = min_element(workers.begin(), workers.end(),
					[](TWorkerVectorType::value_type& l, TWorkerVectorType::value_type& r)
				{
					return l->GetSessionCount() < r->GetSessionCount();
				});
				return result->get();
			}

			inline static void StartWorkers(TWorkerVectorType& workers)
			{
				_STD vector<shared_ptr<_BOOST thread>> threads;
				auto threadCount = workers.size();
				assert(threadCount > 0);
				if (threadCount > 1)
				{
					for (size_t i = 1; i < threadCount; i++)
					{
						auto tmpThread = make_shared<_BOOST thread>(_BOOST bind(&TWorkerType::Start, workers[i].get()));

						threads.push_back(tmpThread);
					}
				}

				// 自身线程也算在内
				workers[0]->Start();
				// join等待所有线程结束
				for each (auto& var in threads)
				{
					var->join();
				}
			}

		};
		typedef Config TConfig;
		typedef TraceSource TTraceSource;
	};

}  // namespace FeedTheDog
