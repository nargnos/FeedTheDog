#pragma once
#include "..\Common.h"
#include "..\Config.h"
#include "..\Trait\Trait.h"
#include "..\Trait\CoreTrait.h"
#include "..\Interface\IService.h"
namespace FeedTheDog
{
	class Core :
		private _BOOST noncopyable
	{
	public:
		typedef typename Core TCore;
		typedef typename CoreTrait::TWorker TWorker;
		typedef typename CoreTrait::TService TService;
		typedef typename CoreTrait::TSessionPool TSessionPool;
		Core();
		virtual ~Core();
		// ȡ����Worker�Ķ����
		TSessionPool* GetIdleSessionPool();
		TService* GetService(int id);
		bool AddService(const shared_ptr<TService>&);
		void DeleteService(const shared_ptr<TService>&);
		void Start();
		void Stop();
		int GetWorkerCount() const;
		// ȡ����Worker
		TWorker* SelectIdleWorker();
	private:
		bool isStop;
		int threadCount;
		Config config;
		_STD vector<shared_ptr<TWorker>> workers;
		_BOOST mutex mutex;
		concurrent_unordered_map<int, shared_ptr<TService>> services;
	};

}  // namespace FeedTheDog
