#pragma once
#include "Config.h"
#include "WorkerTrait.h"
#include "CoreTrait.h"
#include "ISessionPool.h"
#include "WorkerBase.h"
#include "SessionPool.h"
namespace FeedTheDog
{
	class Core;
//	template<typename TOwner>
	class Worker :
		public WorkerBase<Core>
	{
	public:
		//typedef Worker TWorker;
		typedef typename WorkerTrait::TService TService;

		typedef typename SessionPoolTrait::TTcp TTcp;
		typedef typename SessionPoolTrait::TUdp TUdp;

		typedef typename CoreTrait::TSessionPool<TTcp>::type TTcpSessionPool;
		typedef typename CoreTrait::TSessionPool<TUdp>::type TUdpSessionPool;

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
		inline shared_ptr<typename CoreTrait::TSessionPool<TProtocol>::type::TSession> NewSession()
		{
			return GetSessionPool<TProtocol>()->NewSession();
		}
		template<typename TProtocol>
		inline typename typename CoreTrait::TSessionPool<TProtocol>::type::TResolver& GetResolver()
		{
			return GetSessionPool<TProtocol>()->GetResolver();
		}
	private:
		unique_ptr<TTcpSessionPool> tcpSessionPool;
		unique_ptr<TUdpSessionPool> udpSessionPool;

		template<typename TProtocol>
		inline unique_ptr<typename CoreTrait::TSessionPool<TProtocol>::type>& GetSessionPool();

		template<>
		inline unique_ptr<typename CoreTrait::TSessionPool<TTcp>::type>& GetSessionPool<TTcp>()
		{
			return tcpSessionPool;
		}
		template<>
		inline unique_ptr<typename CoreTrait::TSessionPool<TUdp>::type>& GetSessionPool<TUdp>()
		{
			return udpSessionPool;
		}
	};
}  // namespace FeedTheDog
