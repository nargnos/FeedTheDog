#pragma once
#include "Config.h"
#include "Trait.h"
#include "CoreTrait.h"
#include "IService.h"

namespace FeedTheDog
{
	class Core :
		private _BOOST noncopyable,
		public _STD enable_shared_from_this<Core>
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
	};

}  // namespace FeedTheDog
