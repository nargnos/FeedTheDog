#pragma once
#include "Config.h"
#include "WorkerTrait.h"
#include "CoreTrait.h"
#include "ISessionPool.h"
#include "WorkerBase.h"
#include "SessionPool.h"
namespace FeedTheDog
{

	class Worker :
		public WorkerBase
	{
	public:
		typedef Worker TWorker;
		typedef typename WorkerTrait::TService TService;

		typedef typename SessionPoolTrait::TTcp TTcp;
		typedef typename SessionPoolTrait::TUdp TUdp;

		typedef typename CoreTrait::TSessionPool<TTcp>::type TTcpSessionPool;
		typedef typename CoreTrait::TSessionPool<TUdp>::type TUdpSessionPool;
		enum SessionPoolType
		{
			TcpSessionPool,
			UdpSessionPool,
			_EndSessionPoolType
		};
		Worker(TCore* core) :
			WorkerBase(core)
		{
			sessionPools[TcpSessionPool] = make_unique<TTcpSessionPool>(core, this, ioService);
			sessionPools[UdpSessionPool] = make_unique<TUdpSessionPool>(core, this, ioService);
		}
		~Worker()
		{
		}

		void RemoveAllServiceSession(const char* serviceName)
		{
			if (!isRunning)
			{
				// worker停止，由其它函数移除session
				return;
			}
			for each (auto& var in sessionPools)
			{
				var->RemoveServiceSession(serviceName);
			}
		}
		virtual void CloseAllSessions() override
		{
			for each (auto& var in sessionPools)
			{
				var->__PreDestruct();
				var->CloseAll();
			}
		}

		unsigned int GetSessionCount()
		{
			unsigned int sum = 0;
			for each (auto& var in sessionPools)
			{
				sum += var->GetSessionCount();
			}
			return sum;
		}
		

		template<typename TProtocol>
		inline shared_ptr<typename SessionPool<TProtocol>::TSession> NewSession(const char* serviceName)
		{
			return GetSessionPool<TProtocol>()->NewSession(serviceName);
		}
		template<typename TProtocol>
		inline typename SessionPool<TProtocol>::TResolver& GetResolver()
		{
			return GetSessionPool<TProtocol>()->GetResolver();
		}
	private:
		unique_ptr<ISessionPoolBase> sessionPools[_EndSessionPoolType];
		template<typename TProtocol>
		inline ISessionPool<TProtocol>* GetSessionPool();
		template<>
		inline ISessionPool<TTcp>* GetSessionPool<TTcp>()
		{
			auto result = static_cast<ISessionPool<TTcp>*>(sessionPools[TcpSessionPool].get());
			assert(result);
			return result;
		}
		template<>
		inline ISessionPool<TUdp>* GetSessionPool<TUdp>()
		{
			auto result = static_cast<ISessionPool<TUdp>*>(sessionPools[UdpSessionPool].get());
			assert(result);
			return result;
		}
	};
}  // namespace FeedTheDog
