#pragma once
#include "IService.h"
#include "Core.h"
#include "Worker.h"
#include "Session.h"
#include "SessionPool.h"

namespace FeedTheDog
{
	struct ServiceBase :
		public IService<Core>
	{
		typedef typename Core::TWorker::TTcp TTcp;
		typedef typename Core::TWorker::TUdp TUdp;
		typedef typename Core::TWorker::TTcpSessionPool TTcpSessionPool;
		typedef typename Core::TWorker::TTcpSession TTcpSession;
		typedef typename Core::TWorker::TUdpSessionPool TUdpSessionPool;
		typedef typename Core::TWorker::TUdpSession TUdpSession;

		template<typename TProtocol>
		struct TSession
		{
			typedef typename Core::TWorker::TSession<TProtocol>::TSessionType TSessionType;
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
