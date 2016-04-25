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
		typedef typename TTcpSessionPool::TSession TTcpSession;
		typedef typename TTcpSessionPool::TSession_NoBuffer TTcpSession_NoBuffer;
		typedef typename TWorker::TUdpSessionPool TUdpSessionPool;
		typedef typename TUdpSessionPool::TSession TUdpSession;
		typedef typename TUdpSessionPool::TSession_NoBuffer TUdpSession_NoBuffer;

		template<typename TProtocol>
		struct TSessionPool
		{
			typedef typename TWorker::template TSessionPool<TProtocol>::TSessionPoolType TSessionPoolType;
		};
		template<typename TProtocol, bool hasBuffer>
		struct TSession
		{
			typedef typename TSessionPool<TProtocol>::TSessionPoolType::template THasBuffer<hasBuffer>::TSessionType TSessionType;
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

		template<typename TProtocol, bool hasBuffer = true>
		inline shared_ptr<typename TSession<TProtocol, hasBuffer>::TSessionType> NewSession()
		{
			return SelectIdleWorker()->GetSessionPool<TProtocol>()->NewSession<hasBuffer>();
		}

		template<typename TProtocol>
		inline typename TWorker::template TSessionPool<TProtocol>::TSessionPoolType::TResolver& GetResolver()
		{
			return SelectIdleWorker()->GetSessionPool<TProtocol>()->GetResolver();
		}
		virtual bool InitService() = 0;
		concurrent_unordered_map<string, shared_ptr<IAddon>> addons;
		TSessionManager sessionManager;
		TServiceManager* manager;
		const char* name_;
		bool isStopped;
		bool isInitialized;
	};
	typedef ServiceBaseImpl<ServicePolicy> ServiceBase;
	typedef ServiceBase::TTcpSession TcpSession;
	typedef ServiceBase::TUdpSession UdpSession;
	typedef ServiceBase::TTcpSession_NoBuffer TcpSessionNoBuffer;
	typedef ServiceBase::TUdpSession_NoBuffer UdpSessionNoBuffer;

}  // namespace FeedTheDog
