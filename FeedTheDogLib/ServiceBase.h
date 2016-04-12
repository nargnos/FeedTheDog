#pragma once
#include "ServiceManager.h"

namespace FeedTheDog
{
	struct ServiceBase :
		public IService<ServiceManager>
	{
		typedef typename ServiceManager::TWorker::TTcp TTcp;
		typedef typename ServiceManager::TWorker::TUdp TUdp;
		typedef typename ServiceManager::TWorker::TTcpSessionPool TTcpSessionPool;
		typedef typename ServiceManager::TWorker::TTcpSession TTcpSession;
		typedef typename ServiceManager::TWorker::TUdpSessionPool TUdpSessionPool;
		typedef typename ServiceManager::TWorker::TUdpSession TUdpSession;
		typedef typename ServiceManager::TTraceSource::TLevel TLevel;


		template<typename TProtocol>
		struct TSession
		{
			typedef typename ServiceManager::TWorker::TSession<TProtocol>::TSessionType TSessionType;
		};

		ServiceBase(const char* name)
		{
			name_ = name;
		}
		virtual const char* Name() const override
		{
			return name_;
		}
		virtual ~ServiceBase() = default;
	protected:
		const char* name_;
	};

}  // namespace FeedTheDog
