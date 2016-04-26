#pragma once
#include "ServiceManager.h"
#include "SessionManager.h"
namespace FeedTheDog
{
	class WorkerFriendProxy;

	class ServiceBaseImpl
	{
	public:
		friend class ServiceBase;
		typedef ServiceManager TServiceManager;
		typedef SessionManager TSessionManager;

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
		}
		const char* Name() const
		{
			return name_;
		}
		void Init(TServiceManager* managerPtr)
		{
			assert(!isInitialized);
			manager = managerPtr;
			isInitialized = true;
		}

		// TODO: 如何使用插件
		bool AddAddon(shared_ptr<IAddon>& addon)
		{
			if (addons.count(addon->Name()) > 0)
			{
				return false;
			}
			addons.insert({ addon->Name(), addon });
			return true;
		}
		// TODO: 运行时删除需要多做一些处理
		void RemoveAddon(const char* name)
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
			return manager->GetWorkerPool()->SelectIdleWorker();
		}
		inline const unique_ptr<TTraceSource>& GetTrace() const
		{
			return manager->GetTrace();
		}
		inline TWorker* SelectWorker()
		{
			assert(isInitialized);
			return manager->GetWorkerPool()->SelectWorker();
		}

		template<typename TProtocol, bool hasBuffer = true>
		inline shared_ptr<typename TSession<TProtocol, hasBuffer>::TSessionType> NewSession()
		{
			return GetSessionPool<TProtocol>()->NewSession<hasBuffer>();
		}

		template<typename TProtocol>
		inline typename TSessionPool<TProtocol>::TSessionPoolType::TResolver& GetResolver()
		{
			return GetSessionPool<TProtocol>()->GetResolver();
		}

		concurrent_unordered_map<string, shared_ptr<IAddon>> addons;
		TServiceManager* manager;
		const char* name_;
		bool isInitialized;
	private:
		template<typename TProtocol>
		const unique_ptr<typename TSessionPool<TProtocol>::TSessionPoolType>& GetSessionPool()
		{
			return WorkerFriendProxy::GetSessionPool<TProtocol>(SelectIdleWorker());
		}
	};

	class WorkerFriendProxy
	{
	public:
		friend ServiceBaseImpl;
	private:
		template<typename TProtocol, typename TWorker>
		static const unique_ptr<typename TWorker::template TSessionPool<TProtocol>::TSessionPoolType>&
			GetSessionPool(TWorker* worker)
		{
			return worker->GetSessionPool<TProtocol>();
		}
		WorkerFriendProxy() = delete;

		~WorkerFriendProxy() = delete;

	};
}  // namespace FeedTheDog
