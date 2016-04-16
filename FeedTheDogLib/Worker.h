#pragma once
#include "WorkerBase.h"
#include "Owner.h"
namespace FeedTheDog
{
	template<typename TOwner,
		typename TWorkerPolicy>
	class Worker :
		public Owner<TOwner>,
		public WorkerBase
	{
	public:
		typedef typename TWorkerPolicy::TTcp TTcp;
		typedef typename TWorkerPolicy::TUdp TUdp;

		typedef typename TWorkerPolicy::template TSessionPool<TTcp, Worker>::TSessionPoolType TTcpSessionPool;
		typedef typename TWorkerPolicy::template TSessionPool<TUdp, Worker>::TSessionPoolType TUdpSessionPool;

		typedef typename TWorkerPolicy::template TSessionPool<TTcp, Worker>::TSessionType TTcpSession;
		typedef typename TWorkerPolicy::template TSessionPool<TUdp, Worker>::TSessionType TUdpSession;

		template<typename TProtocol>
		struct TSession
		{
			typedef typename TWorkerPolicy::template TSessionPool<TProtocol, Worker>::TSessionType TSessionType;
		};

		template<typename TProtocol>
		struct TSessionPool
		{
			typedef typename TWorkerPolicy::template TSessionPool<TProtocol, Worker>::TSessionPoolType TSessionPoolType;
		};

		Worker(TOwner* owner) :
			Owner(owner)
		{
			
			tcpSessionPool = make_unique<TTcpSessionPool>(this, ioService);
			udpSessionPool = make_unique<TUdpSessionPool>(this, ioService);
		}
		~Worker()
		{
		}
	

		unsigned int GetSessionCount()
		{
			unsigned int sum = 0;
			sum += tcpSessionPool->GetSessionCount();
			sum += udpSessionPool->GetSessionCount();
			return sum;
		}
		template<typename TProtocol>
		inline unique_ptr<typename TSessionPool<TProtocol>::TSessionPoolType>& GetSessionPool();

		template<>
		inline unique_ptr<TTcpSessionPool>& GetSessionPool<TTcp>()
		{
			return tcpSessionPool;
		}
		template<>
		inline unique_ptr<TUdpSessionPool>& GetSessionPool<TUdp>()
		{
			return udpSessionPool;
		}
		template<typename TProtocol>
		inline shared_ptr<typename TSession<TProtocol>::TSessionType> NewSession()
		{
			return GetSessionPool<TProtocol>()->NewSession();
		}
		template<typename TProtocol>
		inline typename TSessionPool<TProtocol>::TSessionPoolType::TResolver& GetResolver()
		{
			return GetSessionPool<TProtocol>()->GetResolver();
		}
	private:
		unique_ptr<TTcpSessionPool> tcpSessionPool;
		unique_ptr<TUdpSessionPool> udpSessionPool;

		virtual void CloseAllSessions() override
		{
			tcpSessionPool->__PreDestruct();
			tcpSessionPool->CloseAll();

			udpSessionPool->__PreDestruct();
			udpSessionPool->CloseAll();

		}
	};
}  // namespace FeedTheDog
