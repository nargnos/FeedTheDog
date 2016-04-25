#pragma once
#include "WorkerBase.h"
namespace FeedTheDog
{
	template<typename TWorkerPolicy>
	class Worker :
		public WorkerBase
	{
	public:
		typedef typename TWorkerPolicy::TTcp TTcp;
		typedef typename TWorkerPolicy::TUdp TUdp;

		template<typename TProtocol>
		struct TSessionPool
		{
			typedef typename TWorkerPolicy::template TSessionPool<TProtocol, Worker>::TSessionPoolType TSessionPoolType;
		};

		typedef typename TSessionPool<TTcp>::TSessionPoolType TTcpSessionPool;
		typedef typename TSessionPool<TUdp>::TSessionPoolType TUdpSessionPool;

		Worker()
		{

			tcpSessionPool = make_unique<TTcpSessionPool>(this, ioService);
			udpSessionPool = make_unique<TUdpSessionPool>(this, ioService);
		}
		~Worker()
		{
		}

		unsigned int GetSessionCount() const
		{
			return tcpSessionPool->GetSessionCount() +
				udpSessionPool->GetSessionCount();
		}
		template<typename TProtocol>
		inline const unique_ptr<typename TSessionPool<TProtocol>::TSessionPoolType>& GetSessionPool() const;

		template<>
		inline const unique_ptr<TTcpSessionPool>& GetSessionPool<TTcp>() const
		{
			return tcpSessionPool;
		}
		template<>
		inline const unique_ptr<TUdpSessionPool>& GetSessionPool<TUdp>() const
		{
			return udpSessionPool;
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
