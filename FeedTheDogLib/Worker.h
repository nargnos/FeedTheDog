#pragma once
#include "WorkerTrait.h"
#include "WorkerBase.h"
#include "SessionPool.h"
namespace FeedTheDog
{
	template<typename TOwner>
	class Worker :
		public WorkerBase<Core>
	{
	public:
		//typedef Worker TWorker;
		typedef WorkerTrait TWorkerTrait;
		//typedef typename TWorkerTrait::TService TService;

		typedef typename TWorkerTrait::TTcp TTcp;
		typedef typename TWorkerTrait::TUdp TUdp;

		typedef typename TWorkerTrait::TSessionPool<TTcp, Worker>::TSessionPoolType TTcpSessionPool;
		typedef typename TWorkerTrait::TSessionPool<TUdp, Worker>::TSessionPoolType TUdpSessionPool;

		typedef typename TWorkerTrait::TSessionPool<TTcp, Worker>::TSessionType TTcpSession;
		typedef typename TWorkerTrait::TSessionPool<TUdp, Worker>::TSessionType TUdpSession;

		template<typename TProtocol>
		struct TSession
		{
			typedef typename TWorkerTrait::TSessionPool<TProtocol, Worker>::TSessionType TSessionType;
		};

		template<typename TProtocol>
		struct TSessionPool
		{
			typedef typename TWorkerTrait::TSessionPool<TProtocol, Worker>::TSessionPoolType TSessionPoolType;
		};

		Worker(TOwner* core) :
			WorkerBase(core)
		{
			tcpSessionPool = make_unique<TTcpSessionPool>(this, ioService);
			udpSessionPool = make_unique<TUdpSessionPool>(this, ioService);
		}
		~Worker()
		{
		}

		//void RemoveAllServiceSession(const char* serviceName)
		//{
		//	if (!isRunning)
		//	{
		//		// worker停止，由其它函数移除session
		//		return;
		//	}
		//	tcpSessionPool->RemoveServiceSession(serviceName);
		//	udpSessionPool->RemoveServiceSession(serviceName);

		//}
		virtual void CloseAllSessions() override
		{
			tcpSessionPool->__PreDestruct();
			tcpSessionPool->CloseAll();

			udpSessionPool->__PreDestruct();
			udpSessionPool->CloseAll();

		}

		unsigned int GetSessionCount()
		{
			unsigned int sum = 0;
			sum += tcpSessionPool->GetSessionCount();
			sum += udpSessionPool->GetSessionCount();
			return sum;
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
	};
}  // namespace FeedTheDog
