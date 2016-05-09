#pragma once
#include "ServiceManager.h"
namespace FeedTheDog
{
	class WorkerFriendProxy;

	struct ServiceDefines
	{
		using TServiceManager = ServiceManager;

		using TWorker = typename TServiceManager::TWorker;
		using TTraceSource = typename TServiceManager::TTraceSource;
		using TWorkerPool = typename TServiceManager::TWorkerPool;
		using TLevel = typename TServiceManager::TTraceSource::TLevel;

		using Tcp = typename TWorker::Tcp;
		using Udp = typename TWorker::Udp;

		using TcpSessionPool = typename TWorker::TcpSessionPool;
		using TcpSession = typename TWorker::TcpSession;

		using UdpSessionPool = typename TWorker::UdpSessionPool;
		using UdpSession = typename TWorker::UdpSession;

		using TcpSessionPoolNoBuffer = typename TWorker::TcpSessionPoolNoBuffer;
		using TcpSessionNoBuffer = typename TWorker::TcpSessionNoBuffer;

		using UdpSessionPoolNoBuffer = typename TWorker::UdpSessionPoolNoBuffer;
		using UdpSessionNoBuffer = typename TWorker::UdpSessionNoBuffer;

		template<typename TSession>
		using TSessionPool = typename TWorker::TSessionPool<TSession>;

		template<typename TProtocol, bool hasBuffer>
		using TSession = typename TWorker::TSession<TProtocol, hasBuffer>;

		template<typename TProtocol>
		using TResolver = typename TWorker::TResolver<TProtocol>;
	};

	class ServiceBaseImpl :
		private ServiceDefines
	{
	public:
		friend class ServiceBase;

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
		inline TWorker* FASTCALL SelectIdleWorker()
		{
			assert(isInitialized);
			return manager->GetWorkerPool()->SelectIdleWorker();
		}
		inline const unique_ptr<TTraceSource>& FASTCALL GetTrace() const
		{
			return manager->GetTrace();
		}
		inline TWorker* FASTCALL SelectWorker()
		{
			assert(isInitialized);
			return manager->GetWorkerPool()->SelectWorker();
		}

		template<typename TProtocol, bool hasBuffer>
		inline shared_ptr<TSession<TProtocol, hasBuffer>> FASTCALL NewSession()
		{
			return GetSessionPool<TProtocol, hasBuffer>()->NewSession();
		}

		template<typename TProtocol>
		inline const _STD unique_ptr<TResolver<TProtocol>>& FASTCALL GetResolver()
		{
			return SelectIdleWorker()->GetResolver<TProtocol>();
		}

		concurrent_unordered_map<string, shared_ptr<IAddon>> addons;
		TServiceManager* manager;
		const char* name_;
		bool isInitialized;
	private:
		template<typename TProtocol, bool hasBuffer>
		const unique_ptr<TSessionPool< TSession<TProtocol, hasBuffer>>>& FASTCALL GetSessionPool()
		{
			return WorkerFriendProxy::GetSessionPool<TSession<TProtocol, hasBuffer>>(SelectIdleWorker());
		}
	};

	class WorkerFriendProxy
	{
	public:
		friend ServiceBaseImpl;
	private:
		template<typename TSession, typename TWorker>
		static const unique_ptr<typename TWorker::template TSessionPool<TSession>>&
			FASTCALL GetSessionPool(TWorker* worker)
		{
			return worker->GetSessionPool<TSession>();
		}
		WorkerFriendProxy() = delete;

		~WorkerFriendProxy() = delete;

	};
}  // namespace FeedTheDog
