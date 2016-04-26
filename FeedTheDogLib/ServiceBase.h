#pragma once
#include "ServiceBaseImpl.h"
namespace FeedTheDog
{
	class ServiceBase :
		public IService<typename ServiceBaseImpl::TServiceManager>
	{
	public:
		// 这些类型可以替换成Wapper
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
		ServiceBase(const char* name):
			impl_(make_unique<ServiceBaseImpl>(name))
		{
		}

		~ServiceBase()
		{
		}

		virtual const char * Name() const override
		{
			return impl_->Name();
		}
		virtual bool Init(TServiceManager * managerPtr) override
		{
			impl_->Init(managerPtr);
			return InitService();
		}
		virtual bool AddAddon(shared_ptr<IAddon>& addon) override
		{
			return impl_ ->AddAddon(addon);
		}
		virtual void RemoveAddon(const char * ptr) override
		{
			return impl_->RemoveAddon(ptr);
		}
	protected:
		virtual bool InitService() = 0;
		inline TWorker* SelectIdleWorker()
		{
			return impl_->SelectIdleWorker();
		}
		inline const unique_ptr<TTraceSource>& GetTrace() const
		{
			return impl_->GetTrace();
		}
		inline TWorker* SelectWorker()
		{
			return impl_->SelectWorker();
		}

		template<typename TProtocol, bool hasBuffer = true>
		inline shared_ptr<typename TSession<TProtocol, hasBuffer>::TSessionType> NewSession()
		{
			return impl_->NewSession<TProtocol,hasBuffer>();
		}

		template<typename TProtocol>
		inline typename TSessionPool<TProtocol>::TSessionPoolType::TResolver& GetResolver()
		{
			return impl_->GetResolver<TProtocol>();
		}

	private:
		unique_ptr<ServiceBaseImpl> impl_;

	};


}  // namespace FeedTheDog
