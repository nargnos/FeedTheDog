#pragma once
#include "Config.h"
#include "Trait.h"
#include "CoreTrait.h"
#include "IService.h"

namespace FeedTheDog
{
	// Service Manager & ThreadPool
	class Core :
		private _BOOST noncopyable
	{
	public:
		typedef typename Core TCore;
		typedef typename CoreTrait::TWorker TWorker;
		typedef typename CoreTrait::TService TService;
		//typedef typename CoreTrait::TSessionPool TSessionPool;
		Core();
		virtual ~Core();
		
		TService* GetService(const char*);
		bool AddService(const shared_ptr<TService>&);
		void DeleteService(const shared_ptr<TService>&);
		void Start();
		void Stop();
		int GetWorkerCount() const;
		// ȡ����Worker
		TWorker* SelectIdleWorker();
		shared_ptr<TraceSource<Config::TEnum>>& GetTrace();

	private:
		bool isStop;
		// ��coreû����ʱ�õ�
		int tmpWorkerIndex;
		Config config;
		_STD vector<shared_ptr<TWorker>> workers;
		_BOOST mutex mutex;
		concurrent_unordered_map<const char*, shared_ptr<TService>> services;
		int threadCount;
	};

}  // namespace FeedTheDog
