#pragma once
#include "Define.h"
#include "WorkerBase.h"
#include "SessionPoolPolicy.h"
namespace FeedTheDog
{
	class Worker :
		public WorkerBase
	{
	public:
		typedef _ASIO ip::tcp TTcp;
		typedef _ASIO ip::udp TUdp;
		
		template<typename TProtocol>
		struct TSessionPool
		{
			// 修改SessionPool类型在此处
			typedef SessionPool<TProtocol, SessionPoolPolicy, MemoryPoolPolicy, SessionStoragePolicy> TSessionPoolType;
		};

		typedef typename TSessionPool<TTcp>::TSessionPoolType TTcpSessionPool;
		typedef typename TSessionPool<TUdp>::TSessionPoolType TUdpSessionPool;

		Worker();
		~Worker();

		unsigned int GetSessionCount() const;

		friend class WorkerFriendProxy;
	private:
		unique_ptr<TTcpSessionPool> tcpSessionPool;
		unique_ptr<TUdpSessionPool> udpSessionPool;
		virtual void CloseAllSessions() override;
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
	};


}  // namespace FeedTheDog
