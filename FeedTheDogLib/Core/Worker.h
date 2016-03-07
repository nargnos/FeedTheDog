#pragma once
#include "..\Common.h"
#include "..\Config.h"
#include "..\Trait\WorkerTrait.h"
#include "..\Trait\CoreTrait.h"
namespace FeedTheDog
{
	class Worker
	{
	public:
		typedef Worker TWorker;
		typedef typename WorkerTrait::TService TService;
		typedef typename CoreTrait::TCore TCore;
		Worker(TCore* core);
		virtual ~Worker();

		int Count() const;
		void Start();
		void Stop();
	private:
		int threadCount;
		TCore* core;
		
	};
}  // namespace FeedTheDog
