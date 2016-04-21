#pragma once

#include "ServicePolicy.h"
namespace FeedTheDog
{
	template<typename TServicePolicy>
	class ServiceBaseImpl :
		public IService<typename TServicePolicy::TServiceManager>
	{
	public:
		typedef typename TServicePolicy::TSessionManager TSessionManager;
		typedef typename TServiceManager::TWorker TWorker;
		typedef typename TServiceManager::TTraceSource TTraceSource;
		typedef typename TServiceManager::TWorkerPool TWorkerPool;
		typedef typename TServiceManager::TTraceSource::TLevel TLevel;
		typedef typename TWorker::TTcp TTcp;
		typedef typename TWorker::TUdp TUdp;
		typedef typename TWorker::TTcpSessionPool TTcpSessionPool;
		typedef typename TWorker::TTcpSession TTcpSession;
		typedef typename TWorker::TUdpSessionPool TUdpSessionPool;
		typedef typename TWorker::TUdpSession TUdpSession;

		template<typename TProtocol>
		struct TSession
		{
			typedef typename TWorker::template TSession<TProtocol>::TSessionType TSessionType;
		};

		ServiceBaseImpl(const char* name)
		{
			isInitialized = false;
			name_ = name;
			isStopped = false;
		}
		virtual const char* Name() const override
		{
			return name_;
		}
		virtual bool Init(TServiceManager* managerPtr) override
		{
			assert(!isInitialized);
			manager = managerPtr;
			isStopped = false;
			isInitialized = true;
			return InitService();
		}
		inline TWorker* SelectIdleWorker()
		{
			assert(isInitialized);
			return manager->GetWorkerPool().SelectIdleWorker();
		}
		inline const unique_ptr<TTraceSource>& GetTrace() const
		{
			return manager->GetTrace();
		}
		inline TWorker* SelectWorker()
		{
			assert(isInitialized);
			return manager->GetWorkerPool().SelectWorker();
		}

		template<typename TProtocol>
		inline shared_ptr<typename TSession<TProtocol>::TSessionType> NewSession()
		{
			return SelectIdleWorker()->GetSessionPool<TProtocol>()->NewSession();
		}
		template<typename TProtocol>
		inline typename TWorker::template TSessionPool<TProtocol>::TSessionPoolType::TResolver& GetResolver()
		{
			return SelectIdleWorker()->GetSessionPool<TProtocol>()->GetResolver();
		}

		// TODO: 如何使用插件
		virtual bool AddAddon(shared_ptr<IAddon>& addon)
		{
			if (addons.count(addon->Name()) > 0)
			{
				return false;
			}
			addons.insert({ addon->Name(), addon });
			return true;
		}
		// TODO: 运行时删除需要多做一些处理
		virtual void RemoveAddon(const char* name)
		{
			addons.unsafe_erase(name);
		}


		virtual ~ServiceBaseImpl()
		{
		}
	protected:
		virtual bool InitService() = 0;
		concurrent_unordered_map<string, shared_ptr<IAddon>> addons;
		TSessionManager sessionManager;
		TServiceManager* manager;
		const char* name_;
		bool isStopped;
		bool isInitialized;
	};
	typedef ServiceBaseImpl<ServicePolicy> ServiceBase;
}  // namespace FeedTheDog
